#pragma once

#include <iostream>
#include <vector>
#include <set>
#include "parser.hpp"

using namespace std;

string join(vector<string> strs) {
    string res;
    for (string str : strs) {
        res += str;
        if (str != strs.back()) {
            res += ", ";
        }
    }
    return res;
}

struct IU {
    string name;
    string type;
    string member_name;

    IU(string name, string type) : type(type) {
        string var_name = string(name);
        replace(var_name.begin(), var_name.end(), '.', '_');
        this->name = var_name;

        name.erase(name.begin(), ++::find(name.begin(), name.end(), '.'));
        this->member_name = name;
    }

    static IU* find(const set<IU*>& ius, string iu_str) {
        replace(iu_str.begin(), iu_str.end(), '.', '_');

        for (IU* iu : ius) {
            if (iu->name == iu_str) {
                return iu;
            }
        }
        throw "no matching attr " + iu_str;
    }

    static set<IU*> find_multiple(const set<IU*>& ius, vector<string> iu_strs) {
        set<IU*> res;

        for (auto iu_str : iu_strs) {
            res.insert(IU::find(ius, iu_str));
        }

        return res;
    }

    template <typename T>
    static vector<string> names(T attrs) {
        vector<string> res;

        for (IU* attr : attrs) {
            res.push_back(attr->name);
        }

        return res;
    }
};

class Operator {
protected:
    ostream* os;
public:
    Operator* parent;
    virtual void produce() = 0;
    virtual void consume(Operator* source) = 0;
    virtual set<IU*> requiredAttributes() = 0;
    virtual set<IU*> producedAttributes() = 0;

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

    set<IU*> requiredAttributes() {
        set<IU*> attrs;
        if (parent != nullptr) {
            attrs = parent->requiredAttributes();
        }

        if (find(attrs.begin(), attrs.end(), lhs) == attrs.end()) {
            attrs.insert(lhs);
        }

        return attrs;
    }

    set<IU*> producedAttributes() {
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
    set<IU*> attributes;
    string table_name;

public:
    TableScan(tuple<string, string> table, vector<string> attributes, ostream* os) {
        string table_alias;

        this->os = os;
        tie(this->table_name, table_alias) = table;

        for (auto attr : attributes) {
            if (attr.substr(0, table_alias.size() + 1) == table_alias + ".") {
                this->attributes.insert(new IU(attr, "Integer"));
            }
        }
    }

    void produce() {
        *os << "for (" << table_name << " data_row : tables." << table_name << "Table.get_rows()) {" << endl;

        for (auto attribute : requiredAttributes()) {
            if (find(attributes.begin(), attributes.end(), attribute) != attributes.end()) {
                *os << attribute->type << " " << attribute->name << " = data_row." << attribute->member_name << ";" << endl;
            }
        }
        parent->consume(this);
        *os << "}" << endl;
    }
    void consume(Operator* source) {}

    set<IU*> requiredAttributes() {
        return parent->requiredAttributes();
    }
    set<IU*> producedAttributes() {
        return attributes;
    }
};

class Print : public Operator {
protected:
    Operator* input;
    set<IU*> attributes;

public:
    Print(Operator* input, set<IU*> attributes, ostream* os) : input(input), attributes(attributes) {
        input->parent = this;
        this->os = os;
    };

    void produce() {
        input->produce();
    }

    void consume(Operator* source) {
        size_t i = 0;
        for (auto attribute : attributes) {
            i++;

            *os << "cout << " << attribute->name;
            if (i != attributes.size()) {
                *os << " << \" | \";" << endl;
            } else {
                *os << " << endl;" << endl;
            }
        }
    }

    set<IU*> requiredAttributes() {
        return attributes;
    }

    set<IU*> producedAttributes() {
        return set<IU*>();
    }
};

class Join : public Operator {
protected:
    Operator *lhs, *rhs;
    vector<IU*> lhs_attrs, rhs_attrs;
    string payload_type() {
        size_t i = 0;
        string res;
        for (auto attribute : lhs->producedAttributes()) {
            i++;
            res += attribute->type;
            if (i != lhs->producedAttributes().size()) {
                res += ", ";
            }
        }

        return res;
    }

public:
    Join(Operator* lhs, Operator* rhs, vector<IU*> lhs_attrs, vector<IU*> rhs_attrs, ostream* os) :
            lhs(lhs), rhs(rhs), lhs_attrs(lhs_attrs), rhs_attrs(rhs_attrs) {
        this->os = os;
        lhs->parent = this;
        rhs->parent = this;
        assert(lhs_attrs.size() == rhs_attrs.size());
    }

    void produce() {
        *os << "multimap<int32_t, tuple<" << payload_type() << ">> hash_table;" << endl;
        lhs->produce();
        rhs->produce();
    }

    void consume(Operator* source) {
        size_t i = 0;
        string lhs_attrs_str = join(IU::names(lhs->producedAttributes()));
        string lhs_hash_attrs_str = join(IU::names(lhs_attrs));
        string rhs_hash_attrs_str = join(IU::names(rhs_attrs));

        if (source == lhs) {
            *os << "hash_table.emplace(hashKey(" << lhs_hash_attrs_str <<
                    "), tuple<" << payload_type() << ">(" << lhs_attrs_str << "));" << endl;
        } else {
            *os << "auto range = hash_table.equal_range(hashKey(" << rhs_hash_attrs_str << "));" << endl;
            *os << "for (auto iter = range.first; iter != range.second; iter++) {" << endl;
                for (auto attribute : lhs->producedAttributes()) {
                    *os << attribute->type << " " << attribute->name << ";" << endl;
                }
                *os << "tie(" << lhs_attrs_str << ") = iter->second;" << endl;

                parent->consume(this);
            *os << "}" << endl;
        }
    }

    set<IU *> requiredAttributes() {
        auto res = parent->requiredAttributes();
        for (auto attr : lhs_attrs) {
            res.insert(attr);
        }
        for (auto attr : rhs_attrs) {
            res.insert(attr);
        }

        return res;
    }

    set<IU *> producedAttributes() {
        auto res = lhs->producedAttributes();
        for (auto attr : rhs->producedAttributes()) {
            if (find(res.begin(), res.end(), attr) != res.end()) {
                res.insert(attr);
            }
        }

        return res;
    }
};

Operator* Operator::from_query(Query& query, ostream* os) {
    vector<tuple<string, Operator*>> tables;
    set<IU*> attrs;

    for (const tuple<string, string>& table : query.tables) {
        string table_alias = get<1>(table);
        Operator* table_op = new TableScan(table, query.attr_names(), os);
        for (auto attr : table_op->producedAttributes()) {
            attrs.insert(attr);
        }

        for (tuple<string, string> selection : query.selections) {
            if (get<0>(selection).substr(0, table_alias.size() + 1) == table_alias + ".") {
                int32_t rhs_int;
                stringstream strstream;
                strstream << get<1>(selection);
                strstream >> rhs_int;

                table_op = new Selection(table_op, IU::find(attrs, get<0>(selection)), rhs_int, os);
            }
        }

        tables.push_back(tuple<string, Operator*>(table_alias, table_op));
    }

    Operator* op = get<1>(tables[0]);

    for (size_t i = 1; i < tables.size(); i++) {
        vector<IU*> lhs_attrs, rhs_attrs;
        for (tuple<string, string> join : query.join_columns) {
            string attr1, attr2;
            tie(attr1, attr2) = join;

            string lhs_alias = get<0>(tables[i - 1]);
            string rhs_alias = get<0>(tables[i]);
            // FIXME
            if (attr1.substr(0, lhs_alias.size() + 1) == lhs_alias + '.' &&
                    attr2.substr(0, rhs_alias.size() + 1) == rhs_alias + '.') {
                lhs_attrs.push_back(IU::find(attrs, attr1));
                rhs_attrs.push_back(IU::find(attrs, attr2));
            }
            if (attr1.substr(0, rhs_alias.size() + 1) == rhs_alias + '.' &&
                    attr2.substr(0, lhs_alias.size() + 1) == lhs_alias + '.') {
                rhs_attrs.push_back(IU::find(attrs, attr1));
                lhs_attrs.push_back(IU::find(attrs, attr2));
            }
        }

        op = new Join(op, get<1>(tables[i]), lhs_attrs, rhs_attrs, os);
    }

    op = new Print(op, IU::find_multiple(attrs, query.select_columns), os);

    return op;
}
