#include <string>
#include <fstream>
#include "operator.hpp"

using namespace std;


void generate_header(ofstream &cpp_gen_tables) {
    cpp_gen_tables << "#pragma once" << endl << endl;
    cpp_gen_tables << "#include \"types.hpp\"" << endl;
    cpp_gen_tables << "#include \"table.hpp\"" << endl << endl;
}

void generate_header_file() {
    ofstream cpp_gen_tables("src/query_gen.hpp", ios::trunc);

    generate_header(cpp_gen_tables);
}

int main(int argc, char *argv[]) {
    generate_header_file();

    auto c_id = new IU {.type = "Integer", .name = "c_id"};
    auto c_w_id = new IU {.type = "Integer", .name = "c_w_id"};
    auto c_d_id = new IU {.type = "Integer", .name = "c_d_id"};
    auto c_first = new IU {.type = "Integer", .name = "c_first"};
    auto c_last = new IU {.type = "Integer", .name = "c_last"};
    auto o_id = new IU {.type = "Integer", .name = "o_id"};
    auto o_c_id = new IU {.type = "Integer", .name = "o_c_id"};
    auto o_w_id = new IU {.type = "Integer", .name = "o_w_id"};
    auto o_d_id = new IU {.type = "Integer", .name = "o_d_id"};
    auto o_all_local = new IU {.type = "Integer", .name = "o_all_local"};
    auto ol_amount = new IU {.type = "Integer", .name = "ol_amount"};

    vector<IU*> attributes = vector<IU*>();
    attributes.push_back(c_id);
    attributes.push_back(c_d_id);
    attributes.push_back(c_w_id);
    attributes.push_back(c_first);
    attributes.push_back(c_last);
    auto table_scan = new TableScan("Customer", attributes);

    auto selection1 = new Selection(table_scan, c_id, 322);
    auto selection2 = new Selection(selection1, c_w_id, 1);
    auto selection3 = new Selection(selection2, c_d_id, 1);

    vector<IU*> order_attributes = vector<IU*>();
    order_attributes.push_back(o_id);
    order_attributes.push_back(o_c_id);
    order_attributes.push_back(c_d_id);
    order_attributes.push_back(c_w_id);
    order_attributes.push_back(o_all_local);
    auto order_table_scan = new TableScan("Order", order_attributes);

    auto order_customer_join = Join(selection3, order_table_scan);

    auto print_attributes = vector<IU*>();
    print_attributes.push_back(c_first);
    print_attributes.push_back(c_last);
    print_attributes.push_back(o_all_local);
    print_attributes.push_back(ol_amount);

    auto ra_tree = Print(selection3, print_attributes);
    ra_tree.produce();

    return 0;
}
