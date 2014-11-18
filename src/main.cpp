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
void read_file_to_table(const string &file_name, Table<T, Index> &table) {
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

void read_tables(const string &base_path) {
    read_file_to_table<warehouse>(base_path + "/tpcc_warehouse.tbl", TABLES.warehouseTable);
    read_file_to_table<customer>(base_path + "/tpcc_customer.tbl", TABLES.customerTable);
    read_file_to_table<district>(base_path + "/tpcc_district.tbl", TABLES.districtTable);
    read_file_to_table<item>(base_path + "/tpcc_item.tbl", TABLES.itemTable);
    read_file_to_table<neworder>(base_path + "/tpcc_neworder.tbl", TABLES.neworderTable);
    read_file_to_table<order>(base_path + "/tpcc_order.tbl", TABLES.orderTable);
    read_file_to_table<orderline>(base_path + "/tpcc_orderline.tbl", TABLES.orderlineTable);
    read_file_to_table<stock>(base_path + "/tpcc_stock.tbl", TABLES.stockTable);
}

static char* line_read = (char *)NULL;

char* rl_gets() {
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
            auto ra_tree = Operator::from_query(*query);
            ra_tree->produce();
        } catch (ParserError e) {
            cerr << "Parse Error: " << e.what() << endl;
        }
    }
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " path_to_table_files" << endl;
        return 1;
    }

    auto start = high_resolution_clock::now();
    read_tables(argv[1]);
    cout << "read tables " << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "s" << endl;

    while (true) {
        repl();
    }
}

