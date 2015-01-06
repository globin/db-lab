#include "Schema.hpp"
#include <sstream>

using namespace std;

static string type(const Schema::Relation::Attribute &attr) {
    Types::Tag type = attr.type;
    switch (type) {
        case Types::Tag::Timestamp:
            return "Timestamp";
        case Types::Tag::Integer:
            return "Integer";
        case Types::Tag::Numeric: {
           stringstream ss;
           ss << "Numeric<" << attr.len << ", " << attr.len2 << ">";
           return ss.str();
        }
        case Types::Tag::Char: {
            stringstream ss;
            ss << "Char<" << attr.len << ">";
            return ss.str();
        }
        case Types::Tag::Varchar: {
            stringstream ss;
            ss << "Varchar<" << attr.len << ">";
            return ss.str();
        }
    }
    throw;
}

string Schema::toString() const {
    stringstream out;
    for (const Schema::Relation &rel : relations) {
        out << rel.name << endl;
        out << "\tPrimary Key:";
        for (unsigned keyId : rel.primaryKey)
            out << ' ' << rel.attributes[keyId].name;
        out << endl;
        for (const auto &attr : rel.attributes)
            out << '\t' << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << endl;
    }
    return out.str();
}

void generate_attribute(stringstream &cpp_gen_tables, const Schema::Relation::Attribute &attribute) {
    cpp_gen_tables << "    " << type(attribute) << ' ' << attribute.name << ';' << endl;
}

void generate_constructor(stringstream &cpp_gen_tables, const Schema::Relation &relation) {
    cpp_gen_tables << "    " << relation.name << "(" << endl;
    for (size_t i = 0; i < relation.attributes.size(); i++) {
        cpp_gen_tables << "        " << type(relation.attributes[i]) << " " << relation.attributes[i].name;
        if (i + 1 < relation.attributes.size()) {
            cpp_gen_tables << ",";
        }
        cpp_gen_tables << endl;
    }
    cpp_gen_tables << "    ) : " << endl;
    for (size_t i = 0; i < relation.attributes.size(); i++) {
        cpp_gen_tables << "        " << relation.attributes[i].name << "(" << relation.attributes[i].name << ")";
        if (i + 1 < relation.attributes.size()) {
            cpp_gen_tables << ",";
        }
        cpp_gen_tables << endl;
    }
    cpp_gen_tables << "    { }" << endl;
}

void generate_from_row(stringstream &cpp_gen_tables, const Schema::Relation &relation) {
    cpp_gen_tables << "    static unique_ptr<" << relation.name << "> from_row(const vector<string> &row) {" << endl;
    cpp_gen_tables << "        unique_ptr<" << relation.name << "> data(new " << relation.name << "(" << endl;
    for (size_t i = 0; i < relation.attributes.size(); i++) {
        cpp_gen_tables << "            " << type(relation.attributes[i]) <<
                "::castString(row[" << i << "].c_str(), row[" << i << "].length())";
        if (i + 1 < relation.attributes.size()) {
            cpp_gen_tables << ',';
        }
        cpp_gen_tables << endl;
    }
    cpp_gen_tables << "        ));" << endl;
    cpp_gen_tables << "        return data;" << endl;
    cpp_gen_tables << "    }" << endl;
}

void primary_key_type(stringstream &out, const Schema::Relation &relation) {
    if (relation.primaryKey.size() == 1) {
        out << type(relation.attributes[relation.primaryKey[0]]);
    } else {
        out << "tuple<";
        for (size_t i = 0; i < relation.primaryKey.size(); i++) {
            out << type(relation.attributes[relation.primaryKey[i]]);

            if (relation.primaryKey.size() > i + 1) {
                out << ", ";
            }
        }
        out << ">";
    }
}

void generate_index(stringstream &cpp_gen_tables, const Schema::Relation &relation) {
    cpp_gen_tables << "    ";
    primary_key_type(cpp_gen_tables, relation);
    cpp_gen_tables << " index() {" << endl;
    cpp_gen_tables << "        return ";
    if (relation.primaryKey.size() == 1) {
        cpp_gen_tables << relation.attributes[relation.primaryKey[0]].name;
    } else {
        primary_key_type(cpp_gen_tables, relation);
        cpp_gen_tables << "(";
        for (size_t i = 0; i < relation.primaryKey.size(); i++) {
            cpp_gen_tables << relation.attributes[relation.primaryKey[i]].name;

            if (relation.primaryKey.size() > i + 1) {
                cpp_gen_tables << ", ";
            }
        }
        cpp_gen_tables << ')';
    }
    cpp_gen_tables << ";" << endl;
    cpp_gen_tables << "    }" << endl;
}

void generate_relation(stringstream &cpp_gen_tables, const Schema::Relation &table) {
    cpp_gen_tables << "struct " << table.name << " {" << endl;
    for (auto attribute : table.attributes) {
        generate_attribute(cpp_gen_tables, attribute);
    }

    cpp_gen_tables << endl;
    generate_constructor(cpp_gen_tables, table);
    cpp_gen_tables << endl;
    generate_from_row(cpp_gen_tables, table);
    cpp_gen_tables << endl;
    generate_index(cpp_gen_tables, table);

    cpp_gen_tables << "};" << endl << endl;
}

void generate_table_type(stringstream &cpp_gen_tables, const Schema::Relation &table) {
    cpp_gen_tables << "typedef Table<" << table.name << ", ";
    primary_key_type(cpp_gen_tables, table);
    cpp_gen_tables << "> " << table.name << "Table;" << endl << endl;
}

string Schema::toCppHeader() const {
    stringstream out;
    for (const Schema::Relation &rel : relations) {
        generate_relation(out, rel);
        generate_table_type(out, rel);
    }

    out << "struct Tables {" << endl;
    for (const Schema::Relation &rel : relations) {
        out << "    " << rel.name << "Table " << rel.name << "Table;" << endl;
    }
    out << "};" << endl << endl;

    out << "static map<string, string> TYPES;" << endl;

    out << "void fill_types() {" << endl;
    for (const Schema::Relation &rel : relations) {
        for (const Schema::Relation::Attribute &attr : rel.attributes) {
            out << "TYPES.emplace(\"" << rel.name << "::" << attr.name << "\", \"" << type(attr) << "\");" << endl;
        }
    }
    out << "}" << endl;

    return out.str();
}
