#pragma once

#include <iostream>
#include <vector>
#include "parser.hpp"

using namespace std;


struct IU {
    string name;
    string type;
    string member_name;
};

class Operator {
protected:
    ostream* os;
public:
    Operator* parent;
    virtual void produce() = 0;
    virtual void consume(Operator* source) = 0;
    virtual vector<IU*> requiredAttributes() = 0;
    virtual vector<IU*> producedAttributes() = 0;

    static Operator* from_query(Query& query, ostream* os);
};

class Selection : public Operator {
protected:
    Operator* input;
    IU* lhs;
    int32_t rhs;
public:
    Selection(Operator* input, IU* lhs, int32_t rhs, ostream* os) : input(input), lhs(lhs), rhs(rhs) {
        this->os = os;
        input->parent = this;
    }

    vector<IU*> requiredAttributes() {
        auto attrs = parent->requiredAttributes();
        if (find(attrs.begin(), attrs.end(), lhs) == attrs.end()) {
            attrs.push_back(lhs);
        }

        return attrs;
    }

    vector<IU*> producedAttributes() {
        return input->producedAttributes();
    }

    void produce() {
        input->produce();
    }
    void consume(Operator* source) {
        *os << "if (" << lhs->name << " == Integer(" << rhs << ")) {" << endl;
        parent->consume(this);
        *os << "}" << endl;
    }
};

class TableScan : public Operator {
protected:
    vector<IU*> attributes;
    string table_name;

public:
    TableScan(string table_name, vector<IU*> attributes, ostream* os) : attributes(attributes), table_name(table_name) {
        this->os = os;
    }

    void produce() {
        *os << "for (" << table_name << " data_row : tables." << table_name << "Table.get_rows()) {" << endl;

        for (auto attribute : requiredAttributes()) {
            *os << attribute->type << " " << attribute->name << " = data_row." << attribute->member_name << ";" << endl;
        }
        parent->consume(this);
        *os << "}" << endl;
    }
    void consume(Operator* source) {}

    vector<IU*> requiredAttributes() {
        return parent->requiredAttributes();
    }
    vector<IU*> producedAttributes() {
        return attributes;
    }
};

class Print : public Operator {
protected:
    Operator* input;
    vector<IU*> attributes;

public:
    Print(Operator* input, vector<IU*> attributes, ostream* os) : input(input), attributes(attributes) {
        input->parent = this;
        this->os = os;
    };

    void produce() {
        input->produce();
    }

    void consume(Operator* source) {
        for (auto attribute : attributes) {
            *os << "cout << " << attribute->name << " << endl;" << endl;
        }
    }

    vector<IU*> requiredAttributes() {
        return attributes;
    }

    vector<IU*> producedAttributes() {
        return vector<IU*>();
    }
};

class Join : public Operator {
protected:
    Operator* lhs;
    Operator* rhs;

public:
    Join(Operator* lhs, Operator* rhs, ostream* os) : lhs(lhs), rhs(rhs) {
        this->os = os;
        lhs->parent = this;
        rhs->parent = this;
    }

    void produce() {
        lhs->produce();
        rhs->produce();
    }

    void consume(Operator* source) {
        if (source == lhs) {
//            print materialize tuple in hash table;
        } else {
//            *os << "for (auto elem : hashtable) {"
//                    "    "
//            print for each match in hashtable[" +a.joinattr+\]";
            parent->consume(this);
        }
    }

    vector<IU *> requiredAttributes() {
        return vector<IU *>();
    }

    vector<IU *> producedAttributes() {
        auto res = lhs->producedAttributes();
        res.insert(res.end(), rhs->producedAttributes().begin(), rhs->producedAttributes().end());

        return res;
    }
};

Operator* Operator::from_query(Query& query, ostream* os) {
    tuple<string, string> base_table = query.tables[0];
    string base_table_name, base_table_alias;
    tie(base_table_name, base_table_alias) = base_table;
    query.tables.erase(query.tables.begin());

    vector<IU*> print_attrs;
    for (string column : query.select_columns) {
        string var_name = string(column);
        replace(var_name.begin(), var_name.end(), '.', '_');

        column.erase(column.begin(), ++find(column.begin(), column.end(), '.'));

        print_attrs.push_back(new IU {.type = "auto", .name = var_name, .member_name = column});
    }
    Operator* op = new TableScan(base_table_name, print_attrs, os);

    for (tuple<string, string> selection : query.selections) {
        string lhs, rhs;
        int32_t rhs_int;
        tie(lhs, rhs) = selection;
        stringstream strstream;
        strstream << rhs;
        strstream >> rhs_int;

        string var_name = string(lhs);
        replace(var_name.begin(), var_name.end(), '.', '_');
        lhs.erase(lhs.begin(), ++find(lhs.begin(), lhs.end(), '.'));

        IU* lhs_attr = nullptr;
        for (IU* attr : print_attrs) {
            if (attr->name == var_name)  {
                lhs_attr = attr;
                break;
            }
        }
        if (lhs_attr == nullptr) {
            lhs_attr = new IU {.type = "auto", .name = var_name, .member_name = lhs};
        }

        op = new Selection(op, lhs_attr, rhs_int, os);
    }
    op = new Print(op, print_attrs, os);

    return op;
}
