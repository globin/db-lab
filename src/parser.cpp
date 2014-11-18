#include <algorithm>
#include <iterator>
#include <cctype>
#include <iostream>

#include "parser.hpp"

using namespace std;

namespace keyword {
    const std::string Select = "select";
    const std::string From = "from";
    const std::string Where = "where";
    const std::string And = "and";
}

namespace literal {
    const char Equals = '=';
    const char Comma = ',';
    const char Semicolon = ';';
}

unique_ptr<Query> Parser::parse() {
    string token;
    unique_ptr<Query> query(new Query());
    istringstream in = istringstream(input);
    while (in >> token) {
        string::size_type pos;
        string::size_type prevPos = 0;

        while ((pos = token.find_first_of(",;=", prevPos)) != string::npos) {
            nextToken(token.substr(prevPos, pos - prevPos), *query);
            nextToken(token.substr(pos, 1), *query);
            prevPos = pos + 1;
        }
        nextToken(token.substr(prevPos), *query);
    }

    if (state != State::Semicolon) {
        throw ParserError("Unterminated Query");
    }
    return std::move(query);
}

static bool isIdentifier(const string &str) {
    if (str == keyword::Select ||
        str == keyword::From ||
        str == keyword::Where ||
        str == keyword::And
    ) {
        return false;
    }
    return str.find_first_not_of("abcdefghijklmnopqrstuvwxyz_1234567890") == string::npos;
}

static bool isColumnIdentifier(const string &str) {
    if (str == keyword::Select ||
        str == keyword::From ||
        str == keyword::Where ||
        str == keyword::And ||
        str.find_first_of("0123456789") == 0
    ) {
        return false;
    }
    return str.find_first_not_of("abcdefghijklmnopqrstuvwxyz_.1234567890") == string::npos;
}

static bool isInt(const string &str) {
    return str.find_first_not_of("0123456789") == string::npos;
}

void Parser::nextToken(const string &token, Query &query) {
    if (getenv("DEBUG"))
        cerr << static_cast<int>(state) << ": " << token << " " << isIdentifier(token) << std::endl;
    if (token.empty())
        return;
//    string tok;
//    transform(token.begin(), token.end(), back_inserter(tok), tolower);
    string tok = token; // FIXME
    switch (state) {
        case State::Init:
            if (tok == keyword::Select)
                state = State::Select;
            else
                throw ParserError("Expected 'SELECT', found '" + token + "'");
            break;
        case State::Select:
        case State::ColumnSeperator:
            if (isColumnIdentifier(tok)) {
                state = State::ColumnName;
                query.select_columns.push_back(token);
            } else {
                throw ParserError("Expected column name, found '" + token + "'");
            }
            break;
        case State::ColumnName:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::ColumnSeperator;
            } else if (tok == keyword::From) {
                state = State::From;
            } else {
                throw ParserError("Expected ',' or 'FROM' , found '" + token + "'");
            }
            break;
        case State::From:
        case State::TableSeperator:
            if (isIdentifier(tok)) {
                state = State::TableName;
                temp = token;
            } else {
                throw ParserError("Expected table name, found '" + token + "'");
            }
            break;
        case State::TableName:
            if (isIdentifier(tok)) {
                state = State::TableAlias;
                query.tables.push_back(tuple<string, string>(temp, token));
            } else {
                throw ParserError("Expected table alias, found '" + token + "'");
            }
            break;
        case State::TableAlias:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::TableSeperator;
            } else if (tok.size() == 1 && tok[0] == literal::Semicolon) {
                state = State::Semicolon;
            } else if (tok == keyword::Where) {
                state = State::Where;
            } else {
                throw ParserError("Expected ',' OR 'WHERE', found '" + token + "'");
            }
            break;
        case State::Where:
            if (isColumnIdentifier(tok)) {
                temp = token;
                state = State::PredicateLHS;
            } else {
                throw ParserError("Expected column name, found '" + token + "'");
            }
            break;
        case State::PredicateLHS:
            if (tok.size() == 1 && tok[0] == literal::Equals) {
                state = State::PredicateEquals;
            } else {
                throw ParserError("Expected '=' before list of index attributes, found '" + token + "'");
            }
            break;
        case State::PredicateEquals:
            if (isColumnIdentifier(tok)) {
                query.join_columns.push_back(tuple<string, string>(temp, token));
            } else if (isInt(tok)) {
                query.selections.push_back(tuple<string, string>(temp, token));
            } else {
                throw ParserError("Expected column name or integer, found '" + token + "'");
            }
            state = State::PredicateRHS;
            break;
        case State::PredicateRHS:
            if (tok.size() == 1 && tok[0] == literal::Comma)
                state = State::PredicateLHS;
            else if (tok.size() == 1 && tok[0] == literal::Semicolon)
                state = State::Semicolon;
            else
                throw ParserError("Expected ',' or ';', found '" + token + "'");
            break;
        case State::Semicolon:
            break;
    }
}
