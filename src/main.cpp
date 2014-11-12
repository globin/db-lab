#include <fstream>
#include <sstream>
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include <chrono>

#include "oltp.hpp"
#include "parser.hpp"
#include "operator.hpp"

using namespace std;
using namespace chrono;

template<typename T, typename Index>
void read_file_to_table(char const *file_name, Table<T, Index> &table) {
    string line;
    ifstream warehouse_file(file_name);
    if (warehouse_file.is_open()) {
        while (getline(warehouse_file, line)) {
            vector<string> column_data;
            stringstream ss(line);
            string cell;
            while (getline(ss, cell, '|')) {
                column_data.push_back(cell);
            }

            T row = T::from_row(column_data);

            table.insert(row);
        }
        warehouse_file.close();
    } else {
        cerr << "Warehouse file doesn't exist!" << endl;
    }
}

void read_tables() {
    read_file_to_table<Warehouse>("../data/tpcc_warehouse.tbl", WAREHOUSE_TABLE);
    read_file_to_table<Customer>("../data/tpcc_customer.tbl", CUSTOMER_TABLE);
    read_file_to_table<District>("../data/tpcc_district.tbl", DISTRICT_TABLE);
    read_file_to_table<Item>("../data/tpcc_item.tbl", ITEM_TABLE);
    read_file_to_table<Neworder>("../data/tpcc_neworder.tbl", NEWORDER_TABLE);
    read_file_to_table<Order>("../data/tpcc_order.tbl", ORDER_TABLE);
    read_file_to_table<Orderline>("../data/tpcc_orderline.tbl", ORDERLINE_TABLE);
    read_file_to_table<Stock>("../data/tpcc_stock.tbl", STOCK_TABLE);
}

static char *line_read = (char *)NULL;

char *rl_gets() {
    if (line_read) {
        free(line_read);
        line_read = (char *)NULL;
    }

    line_read = readline("SQL> ");

    if (line_read && *line_read) {
        add_history(line_read);
    }

    return line_read;
}

void repl() {
    char* sql_line = rl_gets();
    if (sql_line) {
        try {
            auto parser = Parser(sql_line);
            auto query = parser.parse();
//            auto ra_tree = Operator::from_query(*query);
        } catch (ParserError e) {
            cerr << "Parse Error: " << e.what() << endl;
        }
    }
}

int main(int argc, char const *argv[]) {

    auto start = chrono::high_resolution_clock::now();
//    read_tables();
    cout << "read tables " << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "s" << endl;

    while (true) {
        repl();
    }
}

