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

    Data &select(Index lower_bound, Index upper_bound, std::function<bool(const Data&)> pred) {
        auto l = index.lower_bound(lower_bound);
        auto u = index.upper_bound(upper_bound);

        return rows.at((*find_if(l, u, [pred, this](pair<Index, size_t> pair) { return pred(rows.at(pair.second)); })).second);
    }

    void insert(Data data) {
        rows.push_back(data);
        index[data.index()] = rows.size() - 1;
    }

    void remove(Index index_element) {
        if (index.count(index_element) != 0) {
            size_t original_pos = index.at(index_element);
            Data replacement = *rows.end();
            index[replacement.index()] = original_pos;
            rows[original_pos] = replacement;
            index.erase(index_element);
            rows.pop_back();
        }
    }

    size_t size() {
        return rows.size();
    }
};
