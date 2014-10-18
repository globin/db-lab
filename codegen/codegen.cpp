#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

enum ParserState {
    MAIN_SCOPE,
    CREATE_TABLE_SCOPE,
    PARSE_ERROR,
};
struct Type {
    string type;
    string type_parameters;

    Type(string type, string type_parameters) : type(type), type_parameters(type_parameters) {}
    Type(string type) : Type(type, string()) {}

    virtual string cpp_type_string() {
        string res = type;
        res[0] = toupper(res[0]);
        if (!type_parameters.empty()) {
            res += '<' + type_parameters + '>';
        }
        return res;
    }
};

struct Table {
    string name;
    vector<pair<string, Type>> columns;
    vector<string> primary_key;
    string primary_key_str;
    string primary_key_type;
};


ParserState PARSER_STATE = MAIN_SCOPE;
vector<Table> TABLES = vector<Table>();


string &eat_whitespace(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

Type parse_type(string &type_str) {
    size_t delim_pos;
    if ((delim_pos = type_str.find('(')) != string::npos) {
        string type = type_str.substr(0, delim_pos);
        type_str.erase(0, delim_pos + 1);
        string type_parameters = type_str.substr(0, type_str.find(')'));
        return Type(type, type_parameters);
    }

    return Type(type_str.substr(0, type_str.find(' ')));
}

void parse_create_table(string &line) {
    line.erase(0, string("create table ").length());
    string name = line.substr(0, line.find(' '));

    size_t quote_pos;
    while ((quote_pos = name.find('"')) != string::npos) {
        name.erase(quote_pos, quote_pos + 1);
    }

    name[0] = toupper(name[0]);

    TABLES.push_back(Table {.name = name});
    PARSER_STATE = CREATE_TABLE_SCOPE;
}

void parse_main_scope(string &line) {
    if (line.find("create table ") == 0) {
        parse_create_table(line);
    } else if (line.find("create index ") == 0) {
        // TODO: ignore for now
    } else {
        PARSER_STATE = PARSE_ERROR;
        cerr << "Expected `create table`, found: " << line <<  endl;
    }
}

void parse_column(string &line, Table &table) {
    size_t delim_pos = line.find(' ');
    string name = line.substr(0, delim_pos);
    line.erase(0, delim_pos + 1);
    string type_definition = line.substr(0, line.find(' '));

    Type type = parse_type(type_definition);
    table.columns.push_back(pair<string, Type>(name, type));
}

void parse_primary_key(string &line, Table &table) {
    line.erase(0, string("primary key (").length());
    string names = line.substr(0, line.find(')'));

    stringstream ss(names);
    string key;
    while (getline(ss, key, ',')) {
        table.primary_key.push_back(key);
    }

    table.primary_key_str = names;
}

string find_column_type(string col_name, Table &table) {
    for (auto column : table.columns) {
        if (column.first == col_name) {
            return column.second.cpp_type_string();
        }
    }
    return string();
}

void finalize_table(Table &table) {
    if (table.primary_key.size() == 1) {
        table.primary_key_type = find_column_type(table.primary_key[0], table);
    } else {
        string primary_key_type = "tuple<";
        for (size_t i = 0; i < table.primary_key.size(); i++) {
            if (i != 0) primary_key_type += ", ";
            primary_key_type += find_column_type(table.primary_key[i], table);
        }
        primary_key_type += ">";

        table.primary_key_type = primary_key_type;

        table.primary_key_str = primary_key_type + '(' + table.primary_key_str + ")";
    }
}

void parse_create_table_scope(string &line, Table &table) {
    if (line.find(");") == 0) {
        finalize_table(table);
        PARSER_STATE = MAIN_SCOPE;
        return;
    } else if (line.find("primary key") == 0) {
        parse_primary_key(line, table);
    } else {
        parse_column(line, table);
    }
}

void parse_sql_schema(ifstream &schema_file) {
    string line;
    while (getline(schema_file, line)) {

        line = eat_whitespace(line);
        if (line.empty()) continue;

        switch (PARSER_STATE) {
            case MAIN_SCOPE:
                parse_main_scope(line);
                break;
            case CREATE_TABLE_SCOPE:
                parse_create_table_scope(line, TABLES[TABLES.size() - 1]);
                break;
            default:
            case PARSE_ERROR:
                cerr << "Could not parse schema." << endl;
                return;
        }
    }
}

void generate_header(ofstream &cpp_gen_tables) {
    cpp_gen_tables << "#pragma once" << endl << endl;
    cpp_gen_tables << "#include \"types.hpp\"" << endl;
    cpp_gen_tables << "#include \"table.hpp\"" << endl << endl;
}

void generate_column(ofstream &cpp_gen_tables, pair<string, Type> &column) {
    cpp_gen_tables << "    " << column.second.cpp_type_string() << ' ' << column.first << ';' << endl;
}

void generate_constructor(ofstream &cpp_gen_tables, Table &table) {
    cpp_gen_tables << "    " << table.name << "(" << endl;
    for (size_t i = 0; i < table.columns.size(); i++) {
        cpp_gen_tables << "        " << table.columns[i].second.cpp_type_string() << " " << table.columns[i].first;
        if (i + 1 < table.columns.size()) {
            cpp_gen_tables << ",";
        }
        cpp_gen_tables << endl;
    }
    cpp_gen_tables << "    ) : " << endl;
    for (size_t i = 0; i < table.columns.size(); i++) {
        cpp_gen_tables << "        " << table.columns[i].first << "(" << table.columns[i].first << ")";
        if (i + 1 < table.columns.size()) {
            cpp_gen_tables << ",";
        }
        cpp_gen_tables << endl;
    }
    cpp_gen_tables << "    { }" << endl;
}

void generate_from_row(ofstream &cpp_gen_tables, Table &table) {
    cpp_gen_tables << "    static " << table.name << " from_row(vector<string> row) {" << endl;
    cpp_gen_tables << "        return " << table.name << "(" << endl;
    for (size_t i = 0; i < table.columns.size(); i++) {
        cpp_gen_tables << "            " << table.columns[i].second.cpp_type_string() <<
                "::castString(row[" << i << "].c_str(), row[" << i << "].length())";
        if (i + 1 < table.columns.size()) {
            cpp_gen_tables << ',';
        }
        cpp_gen_tables << endl;
    }
    cpp_gen_tables << "        );" << endl;
    cpp_gen_tables << "    }" << endl;
}

void generate_index(ofstream &cpp_gen_tables, Table &table) {
    cpp_gen_tables << "    " << table.primary_key_type << " index() {" << endl;
    cpp_gen_tables << "        return " << table.primary_key_str << ";" << endl;
    cpp_gen_tables << "    }" << endl;
}

void generate_struct(ofstream &cpp_gen_tables, Table &table) {
    cpp_gen_tables << "struct " << table.name << " {" << endl;
    for (auto column : table.columns) {
        generate_column(cpp_gen_tables, column);
    }

    cpp_gen_tables << endl;
    generate_constructor(cpp_gen_tables, table);
    cpp_gen_tables << endl;
    generate_from_row(cpp_gen_tables, table);
    cpp_gen_tables << endl;
    generate_index(cpp_gen_tables, table);

    cpp_gen_tables << "};" << endl << endl;
}

void generate_table_type(ofstream &cpp_gen_tables, Table &table) {
    cpp_gen_tables << "typedef Table<" << table.name << ", " << table.primary_key_type << "> "
            << table.name << "Table;" << endl << endl;
}

int main(int argc, char const *argv[]) {
    ifstream schema_file("../data/schema.sql");
    if (schema_file.is_open()) {
        parse_sql_schema(schema_file);
    }

    ofstream cpp_gen_tables("src/tables_gen.hpp", ios::trunc);

    generate_header(cpp_gen_tables);

    for (Table table : TABLES) {
        generate_struct(cpp_gen_tables, table);
        generate_table_type(cpp_gen_tables, table);
    }

    return 0;
}

