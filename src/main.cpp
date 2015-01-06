#include <fstream>
#include <sstream>
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include <dlfcn.h>

#include "tables_gen.hpp"
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
                column_data.push_back(move(cell));
            }

            table.insert(T::from_row(column_data));
        }

        warehouse_file.close();
    } else {
        cerr << "Warehouse file doesn't exist!" << endl;
    }
}

void read_tables(const string &base_path, Tables &tables) {
    read_file_to_table<warehouse>(base_path + "/tpcc_warehouse.tbl", tables.warehouseTable);
    read_file_to_table<customer>(base_path + "/tpcc_customer.tbl", tables.customerTable);
    read_file_to_table<district>(base_path + "/tpcc_district.tbl", tables.districtTable);
    read_file_to_table<item>(base_path + "/tpcc_item.tbl", tables.itemTable);
    read_file_to_table<neworder>(base_path + "/tpcc_neworder.tbl", tables.neworderTable);
    read_file_to_table<order>(base_path + "/tpcc_order.tbl", tables.orderTable);
    read_file_to_table<orderline>(base_path + "/tpcc_orderline.tbl", tables.orderlineTable);
    read_file_to_table<stock>(base_path + "/tpcc_stock.tbl", tables.stockTable);
}

static char* line_read = (char *)NULL;

char* rl_gets() {
    if (line_read) {
        free(line_read);
        line_read = (char *)NULL;
    }

    line_read = readline("SQL> ");

    if (line_read == NULL || strcmp(line_read, "exit") == 0) {
        cout << "exit" << endl;
        rl_callback_handler_remove();

        exit(0);
    } else if (*line_read) {
        add_history(line_read);
    }

    return line_read;
}

void generate_header(ostream& os) {
    os << "#include <iostream>" << endl;
    os << "#include \"/home/robin/dev/db-lab/code/src/tables_gen.hpp\"" << endl;
    os << "using namespace std;" << endl;
    os << "extern \"C\"" << endl;
    os << "void run_query(Tables& tables) {" << endl;
}
void generate_footer(ostream& os) {
    os << "}" << endl;
}
void compile_query() {
    system("clang++ -fPIC -shared /tmp/query-gen.cpp -std=c++1y -O3 -Wall -Wno-unknown-pragmas -Wno-unused-variable -stdlib=libc++ -o /tmp/query-gen.so");
}
void run_query(Tables& tables) {
   void* handle = dlopen("/tmp/query-gen.so", RTLD_NOW);
   if (!handle) {
      cerr << "error: " << dlerror() << endl;
      exit(1);
   }

   auto query = reinterpret_cast<void(*)(Tables&)>(dlsym(handle, "run_query"));
   if (!query) {
      cerr << "error: " << dlerror() << endl;
      exit(1);
   }

   query(tables);

   if (dlclose(handle)) {
      cerr << "error: " << dlerror() << endl;
      exit(1);
   }
}


void repl(Tables &tables) {
    char* sql_line = rl_gets();
    auto filename = "/tmp/query-gen.cpp";
    ofstream os = ofstream(filename);

    if (sql_line) {
        if (strcmp(sql_line, "\\output") == 0) {
            OUTPUT = !OUTPUT;
            cout << "output: " << (OUTPUT ? "on" : "off") << endl;
        } else {
            try {
                auto parser = Parser(sql_line);
                auto query = parser.parse();

                generate_header(os);
                auto ra_tree = Operator::from_query(*query, &os);
                ra_tree->produce();
                generate_footer(os);

                auto start = high_resolution_clock::now();
                compile_query();
                cout << "compiling query: " << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() * 1000 << "ms" << endl;

                start = high_resolution_clock::now();
                run_query(tables);
                cout << "running query: " << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() * 1000 << "ms" << endl;
            } catch (ParserError e) {
                cerr << "Parse Error: " << e.what() << endl;
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " path_to_table_files" << endl;
        return 1;
    }

    Tables tables;

    auto start = high_resolution_clock::now();
    read_tables(argv[1], tables);
    cout << "read tables: " << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "s" << endl;

    fill_types();

    while (true) {
        repl(tables);
    }
}

