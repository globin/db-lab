#pragma once

#include <iostream>
#include <vector>
#include "parser.hpp"

using namespace std;


struct IU {
    string name;
    string type;
};

class Operator {
public:
    Operator* parent;
    virtual void produce() = 0;
    virtual void consume(Operator* source) = 0;
    virtual vector<IU*> requiredAttributes() = 0;
    virtual vector<IU*> producedAttributes() = 0;

    static Operator* from_query(Query& query);
};

class Selection : public Operator {
private:
    Operator* input;
    IU* lhs;
    int32_t rhs;
public:
    Selection(Operator* input, IU* lhs, int32_t rhs) : input(input), lhs(lhs), rhs(rhs) {
        input->parent = this;
    }

    vector<IU*> requiredAttributes() {
        auto attrs = parent->requiredAttributes();
        attrs.push_back(lhs);
        return attrs;
    }

    vector<IU*> producedAttributes() {
        return input->producedAttributes();
    }

    void produce() {
        input->produce();
    }
    void consume(Operator* source) {
        cout << "if (" << lhs->name << " == Integer(" << rhs << ")) {" << endl;
        parent->consume(this);
        cout << "}" << endl;
    }
};

class TableScan : public Operator {
protected:
    vector<IU*> attributes;
    string table_name;

public:
    TableScan(string table_name, vector<IU*> attributes) : attributes(attributes), table_name(table_name) {}

    void produce() {
        cout << "for (" << table_name << " data_row : " << table_name << "Table.get_rows()) {" << endl;
        for (auto attribute : requiredAttributes()) {
            cout << attribute->type << " " << attribute->name << " = data_row." << attribute->name << ";" << endl;
        }
        parent->consume(this);
        cout << "}" << endl;
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
private:
    Operator* input;
    vector<IU*> attributes;

public:
    Print(Operator* input, vector<IU*> attributes) : input(input), attributes(attributes) {
        input->parent = this;
    };

    void produce() {
        input->produce();
    }

    void consume(Operator* source) {
        for (auto attribute : attributes) {
            cout << "cout << " << attribute->name << " << endl;" << endl;
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
    Join(Operator* lhs, Operator* rhs) : lhs(lhs), rhs(rhs) {
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

Operator* Operator::from_query(Query& query) {
    tuple<string, string> base_table = query.tables[0];
    string base_table_name, base_table_alias;
    tie(base_table_name, base_table_alias) = base_table;
    query.tables.erase(query.tables.begin());

    vector<IU*> attrs;
    for (string column : query.select_columns) {
        replace(column.begin(), column.end(), '.', '_');
        attrs.push_back(new IU {.type = "Integer", .name = column});
    }
    Operator* op = new TableScan(base_table_name, attrs);

    for (tuple<string, string> selection : query.selections) {
        string lhs, rhs;
        int32_t rhs_int;
        tie(lhs, rhs) = selection;
        std::stringstream strstream;
        strstream << rhs;
        strstream >> rhs_int;

        op = new Selection(op, attrs[0], rhs_int);
    }
    op = new Print(op, attrs);

    return op;
}
