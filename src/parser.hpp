#pragma once

#include <exception>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

struct Query {
    vector<string> select_columns;
    vector<tuple<string, string>> tables;
    vector<tuple<string, string>> selections;
    vector<tuple<string, string>> join_columns;

    Query() {
        select_columns = vector<string>();
        tables = vector<tuple<string, string>>();
        selections = vector<tuple<string, string>>();
        join_columns = vector<tuple<string, string>>();
    }
};


class ParserError : exception {
    string msg;
public:
    ParserError(const string &m) : msg(m) {
    }

    ~ParserError() throw() {
    }

    const char *what() const throw() {
        return msg.c_str();
    }
};

struct Parser {
    string input;
    enum class State : unsigned {
        Init, Select, ColumnName, ColumnSeperator, From, TableName, TableAlias, TableSeperator, Where,
        PredicateLHS, PredicateEquals, PredicateRHS, Semicolon
    };
    State state;

    Parser(const string &input) : input(input), state(State::Init) {
    }

    ~Parser() {
    };

    unique_ptr<Query> parse();

private:
    string temp;
    void nextToken(const string &token, Query &s);
};
