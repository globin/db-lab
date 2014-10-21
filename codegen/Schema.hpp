#pragma once

#include <vector>
#include <string>
#include "Types.hpp"

struct Schema {
    struct Relation {
        struct Attribute {
            std::string name;
            Types::Tag type;
            unsigned len;
            unsigned len2;
            bool notNull;

            Attribute() : len(~0), notNull(true) {
            }
        };

        std::string name;
        std::vector <Schema::Relation::Attribute> attributes;
        std::vector<unsigned> primaryKey;

        Relation(const std::string &name) : name(name) {
        }
    };
    struct Index {
        std::string name;
        std::string relationName;
        std::vector<std::string> columnNames;

        Index(const std::string &name) : name(name) {
        }
    };

    std::vector <Schema::Relation> relations;
    std::vector <Schema::Index> indexes;

    std::string toString() const;
};
