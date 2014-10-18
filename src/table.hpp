#pragma once

#include <vector>
#include <map>

using namespace std;

template<typename Data, typename Index>
class Table {
    vector<Data> rows;
    map<Index, size_t> index;

public:
    Data &lookup(Index index_element) {
        return rows.at(index.at(index_element));
    }

    void insert(Data data) {
        rows.push_back(data);
        index[data.index()] = rows.size() - 1;
    }

    size_t size() {
        return rows.size();
    }
};
