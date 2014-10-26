#include <cstdint>
#include <ctime>
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <atomic>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "types.hpp"
#include "tables_gen.hpp"

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


void newOrder(int32_t w_id, int32_t d_id, int32_t c_id, int32_t ol_cnt, int32_t *supware,
        int32_t *itemid, int32_t *qty, Timestamp now) {

    auto w_tax = WAREHOUSE_TABLE.lookup(Integer(w_id)).w_tax;
    auto c_discount = CUSTOMER_TABLE.lookup(
            tuple<Integer, Integer, Integer>(Integer(w_id), Integer(d_id), Integer(c_id))
    ).c_discount;
    auto district = DISTRICT_TABLE.lookup(tuple<Integer, Integer>(Integer(w_id), Integer(d_id)));
    auto o_id = district.d_next_o_id;
    auto d_tax = district.d_tax;

    district.d_next_o_id = o_id;

    int all_local = 1;
    for (int i = 0; i < ol_cnt; i++) {
        if (w_id != supware[i]) all_local=0;
    }

    ORDER_TABLE.insert(Order(o_id, Integer(w_id), Integer(d_id), Integer(c_id), now, Integer(0),
            Numeric<2, 0>(ol_cnt), Numeric<1, 0>(all_local)));
    NEWORDER_TABLE.insert(Neworder(Integer(w_id), Integer(d_id), o_id));

    for (int i = 0; i < ol_cnt; i++) {
        auto i_price = ITEM_TABLE.lookup(Integer(itemid[i])).i_price;
        auto stock = STOCK_TABLE.lookup(tuple<Integer, Integer>(Integer(supware[i]), Integer(itemid[i])));
        auto s_quantity = stock.s_quantity;
        auto s_remote_cnt = stock.s_remote_cnt;
        auto s_order_cnt = stock.s_order_cnt;
        Char<24> s_dist;
        switch (d_id) {
            case 1:
                s_dist = stock.s_dist_01; break;
            case 2:
                s_dist = stock.s_dist_02; break;
            case 3:
                s_dist = stock.s_dist_03; break;
            case 4:
                s_dist = stock.s_dist_04; break;
            case 5:
                s_dist = stock.s_dist_05; break;
            case 6:
                s_dist = stock.s_dist_06; break;
            case 7:
                s_dist = stock.s_dist_07; break;
            case 8:
                s_dist = stock.s_dist_08; break;
            case 9:
                s_dist = stock.s_dist_09; break;
            case 10:
                s_dist = stock.s_dist_10; break;
            default:
                break;
        }

        if (s_quantity > qty[i]) {
            stock.s_quantity -= Numeric<4, 0>(qty[i]);
        } else {
            stock.s_quantity += Numeric<4, 0>(91 - qty[i]);
        }

        if (supware[i] != w_id) {
            stock.s_remote_cnt.value += s_remote_cnt.value;
        } else {
            stock.s_order_cnt.value = s_order_cnt.value + 1;
        }

        auto ol_amount = (Numeric<5, 2>(qty[i]) * i_price * (Numeric<4, 4>(1) + w_tax + d_tax).castLP1() *
                (Numeric<4, 4>(1) - c_discount).castLP1().castP2().castP2()).castLP1().castM2().castM2().castM2()
                .castM2().castM2().castM2().castM2();
        ORDERLINE_TABLE.insert(
                Orderline(o_id, Integer(d_id), Integer(w_id), Integer(i + 1), Integer(itemid[i]), Integer(supware[i]),
                        Timestamp(0), Numeric<2, 0>(qty[i]), ol_amount, s_dist));
    }
}

void newOrderRandom(Timestamp now, int32_t w_id) {
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

void delivery(Integer w_id, Integer o_carrier_id, Timestamp now) {
    for (Integer d_id = Integer(1); d_id <= 10; d_id += Integer(1)) {
         auto neworder = NEWORDER_TABLE.select(
                tuple<Integer, Integer, Integer>(w_id, d_id, Integer(0)),
                tuple<Integer, Integer, Integer>(w_id, d_id + Integer(1), Integer(0)),
                [](const Neworder& data) { return true; } // equivalent to min
         );
         if (!neworder) continue;

         auto o_id = neworder->no_o_id;
         NEWORDER_TABLE.remove(tuple<Integer, Integer, Integer>(w_id, d_id, o_id));

         auto order = ORDER_TABLE.lookup(tuple<Integer, Integer, Integer>(w_id, d_id, o_id));
         auto o_ol_cnt = order.o_ol_cnt;
         auto o_c_id = order.o_c_id;
         order.o_carrier_id = o_carrier_id;

         auto ol_total = Numeric<6, 2>(0);

         for (auto ol_number = Integer(1); Numeric<2, 0>(ol_number) < o_ol_cnt; ol_number += Integer(1)) {
             auto ol = ORDERLINE_TABLE.select(
                 tuple<Integer, Integer, Integer, Integer>(w_id, d_id, o_id, 0),
                 tuple<Integer, Integer, Integer, Integer>(w_id, d_id, o_id + 1, 0),
                 [ol_number](const Orderline& data) { return data.ol_number == ol_number; }
             );
             ol_total = ol_total + ol->ol_amount;
             ol->ol_delivery_d = now;
         }

         CUSTOMER_TABLE.lookup(tuple<Integer, Integer, Integer>(w_id, d_id, o_c_id));
    }
}

typedef tuple<Integer, Integer, Integer> CustomerOrderIndex;
typedef tuple<Integer, Integer, Integer> OrderOrderlineIndex;
typedef tuple<Customer, Order, vector<Orderline>> CustomerOrderOrderlineItem;
void customer_price_query() {
    auto customers = CUSTOMER_TABLE.get_rows();
    customers.erase(
        remove_if(customers.begin(), customers.end(), [](Customer c) { return c.c_last.value[0] != 'B'; }),
        customers.end()
    );

    auto customer_order_map = map<CustomerOrderIndex, pair<Customer, vector<Order>>>();
    for (auto c : customers) {
        customer_order_map.insert(pair<CustomerOrderIndex, pair<Customer, vector<Order>>>(
                CustomerOrderIndex(c.c_w_id, c.c_d_id, c.c_id), pair<Customer, vector<Order>>(c, vector<Order>())));
    }
    for (auto o : ORDER_TABLE.get_rows()) {
        if (customer_order_map.count(CustomerOrderIndex(o.o_w_id, o.o_d_id, o.o_c_id)) > 0) {
            customer_order_map.at(CustomerOrderIndex(o.o_w_id, o.o_d_id, o.o_c_id)).second.push_back(o);
        }
    }

    auto customer_order_orderline_map = map<OrderOrderlineIndex, CustomerOrderOrderlineItem>();
    for (pair<CustomerOrderIndex, pair<Customer, vector<Order>>> c_o_item : customer_order_map) {
        for (auto o : c_o_item.second.second) {
            customer_order_orderline_map.insert(pair<OrderOrderlineIndex, CustomerOrderOrderlineItem>(
                    OrderOrderlineIndex(o.o_w_id, o.o_d_id, o.o_id),
                    CustomerOrderOrderlineItem(c_o_item.second.first, o, vector<Orderline>())
            ));
        }
    }
    for (auto ol : ORDERLINE_TABLE.get_rows()) {
        if (customer_order_orderline_map.count(OrderOrderlineIndex(ol.ol_w_id, ol.ol_d_id, ol.ol_o_id)) > 0) {
            get<2>(customer_order_orderline_map.at(OrderOrderlineIndex(ol.ol_w_id, ol.ol_d_id, ol.ol_o_id))).push_back(ol);
        }
    }

    Numeric<6, 4> result = 0;
    for (auto c_o_ol_hash_item : customer_order_orderline_map) {
        for (Orderline ol : get<2>(c_o_ol_hash_item.second)) {
            result = result + ol.ol_quantity.castLP4().castP2() * ol.ol_amount;
        }
    }

    cout << "customer_price_query: " << result << endl;
}

void print_sizes() {
    cout << endl;
    cout << "Warehouse: " << WAREHOUSE_TABLE.size() << endl;
    cout << "Customer: " << CUSTOMER_TABLE.size() << endl;
    cout << "District: " << DISTRICT_TABLE.size() << endl;
    cout << "Item: " << ITEM_TABLE.size() << endl;
    cout << "Neworder: " << NEWORDER_TABLE.size() << endl;
    cout << "Order: " << ORDER_TABLE.size() << endl;
    cout << "Orderline: " << ORDERLINE_TABLE.size() << endl;
    cout << "Stock: " << STOCK_TABLE.size() << endl;
}

void deliveryRandom(Timestamp now, int32_t warehouses) {
   delivery(Integer(urand(1, warehouses)), Integer(urand(1, 10)), now);
}

void oltp(Timestamp now) {
   int rnd=urand(1,100);
   if (rnd<=10) {
      deliveryRandom(now, urand(1, warehouses));
   } else {
      newOrderRandom(now, urand(1, warehouses));
   }
}


atomic<bool> childRunning;

static void SIGCHLD_handler(int /*sig*/) {
    int status;
    wait(&status);
    // now the child with process id childPid is dead
    childRunning = false;
}

int main(int argc, char const *argv[]) {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = SIGCHLD_handler;
    sigaction(SIGCHLD, &sa, NULL);

    auto start = chrono::high_resolution_clock::now();
    read_tables();
    cout << "read tables " << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "s" << endl;

    customer_price_query();

    print_sizes();
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
        if (!childRunning) {
            childRunning = true;
            pid_t pid = fork();
            if (!pid) { // child
                customer_price_query();
                return 0;
            }
        }
        oltp(Timestamp(time(0)));
    }
    cout << "1000000 iterations " << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "s" << endl;
    print_sizes();

    customer_price_query();
    return 0;
}


