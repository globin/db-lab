#include "Schema.hpp"
#include <sstream>

static std::string type(const Schema::Relation::Attribute &attr) {
    Types::Tag type = attr.type;
    switch (type) {
        case Types::Tag::Timestamp:
            return "Timestamp";
        case Types::Tag::Integer:
            return "Integer";
        case Types::Tag::Numeric: {
           std::stringstream ss;
           ss << "Numeric<" << attr.len << ", " << attr.len2 << ">";
           return ss.str();
        }
        case Types::Tag::Char: {
            std::stringstream ss;
            ss << "Char<" << attr.len << ">";
            return ss.str();
        }
        case Types::Tag::Varchar: {
            std::stringstream ss;
            ss << "Varchar<" << attr.len << ">";
            return ss.str();
        }
    }
    throw;
}

std::string Schema::toString() const {
    std::stringstream out;
    for (const Schema::Relation &rel : relations) {
        out << rel.name << std::endl;
        out << "\tPrimary Key:";
        for (unsigned keyId : rel.primaryKey)
            out << ' ' << rel.attributes[keyId].name;
        out << std::endl;
        for (const auto &attr : rel.attributes)
            out << '\t' << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << std::endl;
    }
    return out.str();
}

void generate_attribute(std::stringstream &cpp_gen_tables, const Schema::Relation::Attribute &attribute) {
    cpp_gen_tables << "    " << type(attribute) << ' ' << attribute.name << ';' << std::endl;
}

void generate_constructor(std::stringstream &cpp_gen_tables, const Schema::Relation &relation) {
    cpp_gen_tables << "    " << relation.name << "(" << std::endl;
    for (size_t i = 0; i < relation.attributes.size(); i++) {
        cpp_gen_tables << "        " << type(relation.attributes[i]) << " " << relation.attributes[i].name;
        if (i + 1 < relation.attributes.size()) {
            cpp_gen_tables << ",";
        }
        cpp_gen_tables << std::endl;
    }
    cpp_gen_tables << "    ) : " << std::endl;
    for (size_t i = 0; i < relation.attributes.size(); i++) {
        cpp_gen_tables << "        " << relation.attributes[i].name << "(" << relation.attributes[i].name << ")";
        if (i + 1 < relation.attributes.size()) {
            cpp_gen_tables << ",";
        }
        cpp_gen_tables << std::endl;
    }
    cpp_gen_tables << "    { }" << std::endl;
}

void generate_from_row(std::stringstream &cpp_gen_tables, const Schema::Relation &relation) {
    cpp_gen_tables << "    static " << relation.name << " from_row(vector<string> row) {" << std::endl;
    cpp_gen_tables << "        return " << relation.name << "(" << std::endl;
    for (size_t i = 0; i < relation.attributes.size(); i++) {
        cpp_gen_tables << "            " << type(relation.attributes[i]) <<
                "::castString(row[" << i << "].c_str(), row[" << i << "].length())";
        if (i + 1 < relation.attributes.size()) {
            cpp_gen_tables << ',';
        }
        cpp_gen_tables << std::endl;
    }
    cpp_gen_tables << "        );" << std::endl;
    cpp_gen_tables << "    }" << std::endl;
}

void primary_key_type(std::stringstream &out, const Schema::Relation &relation) {
    if (relation.primaryKey.size() == 1) {
        out << type(relation.attributes[relation.primaryKey[0]]);
    } else {
        out << "std::tuple<";
        for (size_t i = 0; i < relation.primaryKey.size(); i++) {
            out << type(relation.attributes[relation.primaryKey[i]]);

            if (relation.primaryKey.size() > i + 1) {
                out << ", ";
            }
        }
        out << ">";
    }
}

void generate_index(std::stringstream &cpp_gen_tables, const Schema::Relation &relation) {
    cpp_gen_tables << "    ";
    primary_key_type(cpp_gen_tables, relation);
    cpp_gen_tables << " index() {" << std::endl;
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
    cpp_gen_tables << ";" << std::endl;
    cpp_gen_tables << "    }" << std::endl;
}

void generate_relation(std::stringstream &cpp_gen_tables, const Schema::Relation &table) {
    cpp_gen_tables << "struct " << table.name << " {" << std::endl;
    for (auto attribute : table.attributes) {
        generate_attribute(cpp_gen_tables, attribute);
    }

    cpp_gen_tables << std::endl;
    generate_constructor(cpp_gen_tables, table);
    cpp_gen_tables << std::endl;
    generate_from_row(cpp_gen_tables, table);
    cpp_gen_tables << std::endl;
    generate_index(cpp_gen_tables, table);

    cpp_gen_tables << "};" << std::endl << std::endl;
}

void generate_table_type(std::stringstream &cpp_gen_tables, const Schema::Relation &table) {
    cpp_gen_tables << "typedef Table<" << table.name << ", ";
    primary_key_type(cpp_gen_tables, table);
    cpp_gen_tables << "> " << table.name << "Table;" << std::endl << std::endl;
}

std::string Schema::toCppHeader() const {
    std::stringstream out;
    for (const Schema::Relation &rel : relations) {
        generate_relation(out, rel);
        generate_table_type(out, rel);
    }

    out << "struct Tables {" << std::endl;
    for (const Schema::Relation &rel : relations) {
        out << "    " << rel.name << "Table " << rel.name << "Table;" << std::endl;
    }
    out << "};" << std::endl;

    return out.str();
}
