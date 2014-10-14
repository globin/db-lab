#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include <vector>
#include <map>
#include <string>
#include <tuple>
#include <string.h>
#include "types.hpp"

template<typename Data, typename Index>
class Table {
    std::vector<Data> rows;
    std::map<Index, size_t> index;

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

struct Warehouse {
    int32_t w_id;
    char w_name[10];
    char w_street_1[20];
    char w_street_2[20];
    char w_city[20];
    char w_state[2];
    char w_zip[9];
    int64_t w_tax;
    int64_t w_ytd;

    Warehouse(std::vector<std::string> row) {
        w_id = Integer::castString(row[0].c_str(), row[0].length()).value;
        memcpy(w_name, Varchar<10>::castString(row[1].c_str(), row[1].length()).value, 10);
        memcpy(w_street_1, Varchar<20>::castString(row[2].c_str(), row[2].length()).value, 20);
        memcpy(w_street_2, Varchar<20>::castString(row[3].c_str(), row[3].length()).value, 20);
        memcpy(w_city, Varchar<20>::castString(row[4].c_str(), row[4].length()).value, 20);
        memcpy(w_state, Char<2>::castString(row[5].c_str(), row[5].length()).value, 2);
        memcpy(w_zip, Char<9>::castString(row[6].c_str(), row[6].length()).value, 9);
        w_tax = Numeric<4, 4>::castString(row[7].c_str(), row[7].length()).value;
        w_ytd = Numeric<12, 2>::castString(row[8].c_str(), row[8].length()).value;
    };

    inline int32_t index() {
        return w_id;
    }
};

typedef Table<Warehouse, int32_t> WarehouseTable;

struct District {
    int32_t d_id;
    int32_t d_w_id;
    char d_name[10];
    char d_street_1[20];
    char d_street_2[20];
    char d_city[20];
    char d_state[2];
    char d_zip[9];
    int64_t d_tax;
    int64_t d_ytd;
    int32_t d_next_o_id;

    inline std::tuple<int32_t, int32_t> index() {
        return std::tuple<int32_t, int32_t>(d_id, d_w_id);
    }

    District(std::vector<std::string> row) {
        d_id = Integer::castString(row[0].c_str(), row[0].length()).value;
        d_w_id = Integer::castString(row[1].c_str(), row[1].length()).value;
        memcpy(d_name, Varchar<10>::castString(row[2].c_str(), row[2].length()).value, 10);
        memcpy(d_street_1, Varchar<20>::castString(row[3].c_str(), row[3].length()).value, 20);
        memcpy(d_street_2, Varchar<20>::castString(row[4].c_str(), row[4].length()).value, 20);
        memcpy(d_city, Varchar<20>::castString(row[5].c_str(), row[5].length()).value, 20);
        memcpy(d_state, Char<2>::castString(row[6].c_str(), row[6].length()).value, 2);
        memcpy(d_zip, Char<9>::castString(row[7].c_str(), row[7].length()).value, 9);
        d_tax = Numeric<4, 4>::castString(row[8].c_str(), row[8].length()).value;
        d_ytd = Numeric<12, 2>::castString(row[9].c_str(), row[9].length()).value;
        d_next_o_id = Integer::castString(row[10].c_str(), row[10].length()).value;
    };
};

typedef Table<District, std::tuple<int32_t, int32_t>> DistrictTable;

struct Customer {
    int32_t c_id;
    int32_t c_d_id;
    int32_t c_w_id;
    char c_first[16];
    char c_middle[2];
    char c_last[16];
    char c_street_1[20];
    char c_street_2[20];
    char c_city[20];
    char c_state[2];
    char c_zip[9];
    char c_phone[16];
    int32_t c_since;
    char c_credit[2];
    int64_t c_credit_lim;
    int64_t c_discount;
    int64_t c_balance;
    int64_t c_ytd_paymenr;
    int64_t c_payment_cnt;
    int64_t c_delivery_cnt;
    char c_data[500];

    inline std::tuple<int32_t, int32_t, int32_t> index() {
        return std::tuple<int32_t, int32_t, int32_t>(c_w_id, c_d_id, c_id);
    }

    Customer(std::vector<std::string> row) {
        c_id = Integer::castString(row[0].c_str(), row[0].length()).value;
        c_d_id = Integer::castString(row[1].c_str(), row[1].length()).value;
        c_w_id = Integer::castString(row[2].c_str(), row[2].length()).value;
        memcpy(c_first, Varchar<16>::castString(row[3].c_str(), row[3].length()).value, 16);
        memcpy(c_middle, Char<2>::castString(row[4].c_str(), row[4].length()).value, 2);
        memcpy(c_last, Varchar<16>::castString(row[5].c_str(), row[5].length()).value, 16);
        memcpy(c_street_1, Varchar<20>::castString(row[6].c_str(), row[6].length()).value, 20);
        memcpy(c_street_2, Varchar<20>::castString(row[7].c_str(), row[7].length()).value, 20);
        memcpy(c_city, Varchar<20>::castString(row[8].c_str(), row[8].length()).value, 20);
        memcpy(c_state, Char<2>::castString(row[9].c_str(), row[9].length()).value, 2);
        memcpy(c_zip, Char<9>::castString(row[10].c_str(), row[10].length()).value, 9);
        memcpy(c_phone, Char<16>::castString(row[11].c_str(), row[11].length()).value, 16);
        c_since = Integer::castString(row[12].c_str(), row[12].length()).value;
        memcpy(c_credit, Char<2>::castString(row[13].c_str(), row[13].length()).value, 2);
        c_credit_lim = Numeric<12, 2>::castString(row[14].c_str(), row[14].length()).value;
        c_discount = Numeric<4, 4>::castString(row[15].c_str(), row[15].length()).value;
        c_balance = Numeric<12, 2>::castString(row[16].c_str(), row[16].length()).value;
        c_ytd_paymenr = Numeric<12, 2>::castString(row[17].c_str(), row[17].length()).value;
        c_payment_cnt = Numeric<4, 0>::castString(row[18].c_str(), row[18].length()).value;
        c_delivery_cnt = Numeric<4, 0>::castString(row[19].c_str(), row[19].length()).value;
        memcpy(c_data, Varchar<500>::castString(row[20].c_str(), row[20].length()).value, 500);
    };
};

typedef Table<Customer, std::tuple<int32_t, int32_t, int32_t>> CustomerTable;

//create index customer_wdl on customer(c_w_id,c_d_id,c_last,c_first);

struct History {
    int32_t h_c_id;
    int32_t h_c_d_id;
    int32_t h_c_w_id;
    int32_t h_d_id;
    int32_t h_w_id;
    int32_t h_date;
    int64_t h_amount;
    char h_data[24];
};

struct Neworder {
    int32_t no_o_id;
    int32_t no_d_id;
    int32_t no_w_id;

    inline std::tuple<int32_t, int32_t, int32_t> index() {
        return std::tuple<int32_t, int32_t, int32_t>(no_w_id, no_d_id, no_o_id);
    }

    Neworder(std::vector<std::string> row) {
        no_o_id = Integer::castString(row[0].c_str(), row[0].length()).value;
        no_d_id = Integer::castString(row[1].c_str(), row[1].length()).value;
        no_w_id = Integer::castString(row[2].c_str(), row[2].length()).value;
    };

    Neworder(int32_t o_id, int32_t d_id, int32_t w_id)
            : no_o_id(o_id), no_d_id(d_id), no_w_id(w_id) {
    }
};

typedef Table<Neworder, std::tuple<int32_t, int32_t, int32_t>> NeworderTable;

struct Order {
    int32_t o_id;
    int32_t o_d_id;
    int32_t o_w_id;
    int32_t o_c_id;
    int32_t o_entry_d;
    int32_t o_carrier_id;
    int64_t o_ol_cnt;
    int64_t o_all_local;

    inline std::tuple<int32_t, int32_t, int32_t> index() {
        return std::tuple<int32_t, int32_t, int32_t>(o_w_id, o_d_id, o_id);
    }

    Order(std::vector<std::string> row) {
        o_id = Integer::castString(row[0].c_str(), row[0].length()).value;
        o_d_id = Integer::castString(row[1].c_str(), row[1].length()).value;
        o_w_id = Integer::castString(row[2].c_str(), row[2].length()).value;
        o_c_id = Integer::castString(row[3].c_str(), row[3].length()).value;
        o_entry_d = Integer::castString(row[4].c_str(), row[4].length()).value;
        o_carrier_id = Integer::castString(row[5].c_str(), row[5].length()).value;
        o_ol_cnt = Numeric<2, 0>::castString(row[6].c_str(), row[6].length()).value;
        o_all_local = Numeric<1, 0>::castString(row[7].c_str(), row[7].length()).value;
    };

    Order(int32_t o_id, int32_t d_id, int32_t w_id, int32_t c_id, int32_t entry_d, int32_t carrier_id, int64_t ol_cnt,
            int64_t all_local) :
            o_id(o_id),
            o_d_id(d_id),
            o_w_id(w_id),
            o_c_id(c_id),
            o_entry_d(entry_d),
            o_carrier_id(carrier_id),
            o_ol_cnt(ol_cnt),
            o_all_local(all_local) {
    }
};

typedef Table<Order, std::tuple<int32_t, int32_t, int32_t>> OrderTable;

//create index order_wdc on "order"(o_w_id,o_d_id,o_c_id,o_id);

struct Orderline {
    int32_t ol_o_id;
    int32_t ol_d_id;
    int32_t ol_w_id;
    int32_t ol_number;
    int32_t ol_i_id;
    int32_t ol_supply_w_id;
    int32_t ol_delivery_d;
    int64_t ol_quantity;
    int64_t ol_amount;
    char ol_dist_info[24];

    inline std::tuple<int32_t, int32_t, int32_t, int32_t> index() {
        return std::tuple<int32_t, int32_t, int32_t, int32_t>(ol_w_id, ol_d_id, ol_o_id, ol_number);
    }

    Orderline(std::vector<std::string> row) {
        ol_o_id = Integer::castString(row[0].c_str(), row[0].length()).value;
        ol_d_id = Integer::castString(row[1].c_str(), row[1].length()).value;
        ol_w_id = Integer::castString(row[2].c_str(), row[2].length()).value;
        ol_number = Integer::castString(row[3].c_str(), row[3].length()).value;
        ol_i_id = Integer::castString(row[4].c_str(), row[4].length()).value;
        ol_supply_w_id = Integer::castString(row[5].c_str(), row[5].length()).value;
        ol_delivery_d = Integer::castString(row[6].c_str(), row[6].length()).value;
        ol_quantity = Numeric<2, 0>::castString(row[7].c_str(), row[7].length()).value;
        ol_amount = Numeric<6, 2>::castString(row[8].c_str(), row[8].length()).value;
        memcpy(ol_dist_info, Char<24>::castString(row[9].c_str(), row[9].length()).value, 24);
    };

    Orderline(int32_t o_id, int32_t d_id, int32_t w_id, int32_t number, int32_t i_id, int32_t supply_w_id,
            int32_t delivery_d, int64_t qty, int64_t amount, char* s_dist) :
        ol_o_id(o_id), ol_d_id(d_id), ol_w_id(w_id), ol_number(number),
        ol_i_id(i_id), ol_supply_w_id(supply_w_id), ol_delivery_d(delivery_d),
        ol_quantity(qty), ol_amount(amount) {
        memcpy(ol_dist_info, s_dist, 24);
    }
};

typedef Table<Orderline, std::tuple<int32_t, int32_t, int32_t, int32_t>> OrderlineTable;

struct Item {
    int32_t i_id;
    int32_t i_im_id;
    char i_name[24];
    int64_t i_price;
    char i_data[50];

    inline Integer index() {
        return i_id;
    }

    Item(std::vector<std::string> row) {
        i_id = Integer::castString(row[0].c_str(), row[0].length()).value;
        i_im_id = Integer::castString(row[1].c_str(), row[1].length()).value;
        memcpy(i_name, Varchar<24>::castString(row[2].c_str(), row[2].length()).value, 24);
        i_price = Numeric<5, 2>::castString(row[3].c_str(), row[3].length()).value;
        memcpy(i_data, Varchar<50>::castString(row[4].c_str(), row[4].length()).value, 50);
    };
};

typedef Table<Item, Integer> ItemTable;

struct Stock {
    int32_t s_i_id;
    int32_t s_w_id;
    int64_t s_quantity;
    char s_dist_01[24];
    char s_dist_02[24];
    char s_dist_03[24];
    char s_dist_04[24];
    char s_dist_05[24];
    char s_dist_06[24];
    char s_dist_07[24];
    char s_dist_08[24];
    char s_dist_09[24];
    char s_dist_10[24];
    int64_t s_ytd;
    int64_t s_order_cnt;
    int64_t s_remote_cnt;
    char s_data[50];

    inline std::tuple<Integer, Integer> index() {
        return std::tuple<Integer, Integer>(s_w_id, s_i_id);
    }

    Stock(std::vector<std::string> row) {
        s_i_id = Integer::castString(row[0].c_str(), row[0].length()).value;
        s_w_id = Integer::castString(row[1].c_str(), row[1].length()).value;
        s_quantity = Numeric<4, 0>::castString(row[2].c_str(), row[2].length()).value;
        memcpy(s_dist_01, Char<24>::castString(row[3].c_str(), row[3].length()).value, 24);
        memcpy(s_dist_02, Char<24>::castString(row[4].c_str(), row[4].length()).value, 24);
        memcpy(s_dist_03, Char<24>::castString(row[5].c_str(), row[5].length()).value, 24);
        memcpy(s_dist_04, Char<24>::castString(row[6].c_str(), row[6].length()).value, 24);
        memcpy(s_dist_05, Char<24>::castString(row[7].c_str(), row[7].length()).value, 24);
        memcpy(s_dist_06, Char<24>::castString(row[8].c_str(), row[8].length()).value, 24);
        memcpy(s_dist_07, Char<24>::castString(row[9].c_str(), row[9].length()).value, 24);
        memcpy(s_dist_08, Char<24>::castString(row[10].c_str(), row[10].length()).value, 24);
        memcpy(s_dist_09, Char<24>::castString(row[11].c_str(), row[11].length()).value, 24);
        memcpy(s_dist_10, Char<24>::castString(row[12].c_str(), row[12].length()).value, 24);
        s_ytd = Numeric<8, 0>::castString(row[13].c_str(), row[13].length()).value;
        s_order_cnt = Numeric<4, 0>::castString(row[14].c_str(), row[14].length()).value;
        s_remote_cnt = Numeric<4, 0>::castString(row[15].c_str(), row[15].length()).value;
        memcpy(s_data, Varchar<50>::castString(row[16].c_str(), row[16].length()).value, 50);
    };
};

typedef Table<Stock, std::tuple<Integer, Integer>> StockTable;

#pragma clang diagnostic pop



