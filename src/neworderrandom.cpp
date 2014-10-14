#include <cstdint>
#include <ctime>
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "types.hpp"
#include "tables.hpp"

using namespace std;
using namespace chrono;

const int32_t warehouses = 5;
WarehouseTable WAREHOUSE_TABLE = WarehouseTable();
CustomerTable CUSTOMER_TABLE = CustomerTable();
DistrictTable DISTRICT_TABLE = DistrictTable();
OrderTable ORDER_TABLE = OrderTable();
NeworderTable NEWORDER_TABLE = NeworderTable();
ItemTable ITEM_TABLE = ItemTable();
StockTable STOCK_TABLE = StockTable();
OrderlineTable ORDERLINE_TABLE = OrderlineTable();

int32_t urand(int32_t min, int32_t max) {
    return (random() % (max - min + 1)) + min;
}

int32_t urandexcept(int32_t min, int32_t max, int32_t v) {
    if (max <= min)
        return min;
    int32_t r = (random() % (max - min)) + min;
    if (r >= v)
        return r + 1;
    else
        return r;
}

int32_t nurand(int32_t A, int32_t x, int32_t y) {
    return ((((random() % A) | (random() % (y - x + 1) + x)) + 42) % (y - x + 1)) + x;
}

void newOrder(int32_t w_id, int32_t d_id, int32_t c_id, int32_t ol_cnt, int32_t *supware,
        int32_t *itemid, int32_t *qty, uint64_t now) {

    auto w_tax = WAREHOUSE_TABLE.lookup(w_id).w_tax;
    auto c_discount = CUSTOMER_TABLE.lookup(
            tuple<int32_t, int32_t, int32_t>(w_id, d_id, c_id)
    ).c_discount;
    auto district = DISTRICT_TABLE.lookup(tuple<int32_t, int32_t>(d_id, w_id));
    auto o_id = district.d_next_o_id;
    auto d_tax = district.d_tax;

    district.d_next_o_id = o_id;

    int all_local = 1;
    for (int i = 0; i < ol_cnt; i++) {
        if (w_id != supware[i]) all_local=0;
    }

    ORDER_TABLE.insert(Order(o_id, d_id, w_id, c_id, now, 0, ol_cnt, all_local));
    NEWORDER_TABLE.insert(Neworder(o_id, d_id, w_id));

    for (int i = 0; i < ol_cnt; i++) {
        auto i_price = ITEM_TABLE.lookup(itemid[i]).i_price;
        auto stock = STOCK_TABLE.lookup(tuple<int32_t, int32_t>(supware[i], itemid[i]));
        auto s_quantity = stock.s_quantity;
        auto s_remote_cnt = stock.s_remote_cnt;
        auto s_order_cnt = stock.s_order_cnt;
        char s_dist[24];
        switch (d_id) {
            case 1:
                memcpy(s_dist, stock.s_dist_01, 24);
                break;
            case 2:
                memcpy(s_dist, stock.s_dist_02, 24);
                break;
            case 3:
                memcpy(s_dist, stock.s_dist_03, 24);
                break;
            case 4:
                memcpy(s_dist, stock.s_dist_04, 24);
                break;
            case 5:
                memcpy(s_dist, stock.s_dist_05, 24);
                break;
            case 6:
                memcpy(s_dist, stock.s_dist_06, 24);
                break;
            case 7:
                memcpy(s_dist, stock.s_dist_07, 24);
                break;
            case 8:
                memcpy(s_dist, stock.s_dist_08, 24);
                break;
            case 9:
                memcpy(s_dist, stock.s_dist_09, 24);
                break;
            case 10:
                memcpy(s_dist, stock.s_dist_10, 24);
                break;
            default:
                break;
        }

        if (s_quantity > qty[i]) {
            stock.s_quantity -= qty[i];
        } else {
            stock.s_quantity += 91 - qty[i];
        }

        if (supware[i] != w_id) {
            stock.s_remote_cnt += s_remote_cnt;
        } else {
            stock.s_order_cnt = s_order_cnt + 1;
        }

        int64_t ol_amount = qty[i] * i_price * (1 + w_tax + d_tax) * (1 - c_discount);
        ORDERLINE_TABLE.insert(Orderline(o_id, d_id, w_id, i + 1, itemid[i], supware[i], 0, qty[i], ol_amount, s_dist));
    }
}

void newOrderRandom(uint64_t now, int32_t w_id) {
    int32_t d_id = urand(1, 1);
    int32_t c_id = nurand(1023, 1, 3000);
    int32_t ol_cnt = urand(5, 15);

    int32_t supware[15];
    int32_t itemid[15];
    int32_t qty[15];
    for (int32_t i = 0; i < ol_cnt; i++) {
        if (urand(1, 100) > 1)
            supware[i] = w_id;
        else
            supware[i] = urandexcept(1, warehouses, w_id);
        itemid[i] = nurand(8191, 1, 100000);
        qty[i] = urand(1, 10);
    }

    newOrder(w_id, d_id, c_id, ol_cnt, supware, itemid, qty, now);
}

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

            T row = T(column_data);

            table.insert(row);
        }
        warehouse_file.close();
    } else {
        cerr << "Warehouse file doesn't exist!" << endl;
    }
}

void read_tables() {
    read_file_to_table<Warehouse>("data/tpcc_warehouse.tbl", WAREHOUSE_TABLE);
    read_file_to_table<Customer>("data/tpcc_customer.tbl", CUSTOMER_TABLE);
    read_file_to_table<District>("data/tpcc_district.tbl", DISTRICT_TABLE);
    read_file_to_table<Item>("data/tpcc_item.tbl", ITEM_TABLE);
    read_file_to_table<Neworder>("data/tpcc_neworder.tbl", NEWORDER_TABLE);
    read_file_to_table<Order>("data/tpcc_order.tbl", ORDER_TABLE);
    read_file_to_table<Orderline>("data/tpcc_orderline.tbl", ORDERLINE_TABLE);
    read_file_to_table<Stock>("data/tpcc_stock.tbl", STOCK_TABLE);
}

int main(int argc, char const *argv[]) {

    auto start = chrono::high_resolution_clock::now();
    read_tables();
    cout << "read tables " << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "s" << endl;

    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
        newOrderRandom(time(0), urand(1, warehouses));
    }
    cout << "1000000 iterations " << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "s" << endl;

    return 0;
}
