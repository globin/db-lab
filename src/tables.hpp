#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include <vector>
#include <map>
#include <string>
#include <tuple>
#include "types.hpp"

template<typename T>
class Indexable {
    virtual T index() = 0;
};

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

struct Warehouse : Indexable<Integer> {
    Integer w_id;
    Varchar<10> w_name;
    Varchar<20> w_street_1;
    Varchar<20> w_street_2;
    Varchar<20> w_city;
    Char<2> w_state;
    Char<9> w_zip;
    Numeric<4, 4> w_tax;
    Numeric<12, 2> w_ytd;

    Warehouse(std::vector<std::string> row) {
        w_id = Integer::castString(row[0].c_str(), row[0].length());
        w_name = Varchar<10>::castString(row[1].c_str(), row[1].length());
        w_street_1 = Varchar<20>::castString(row[2].c_str(), row[2].length());
        w_street_2 = Varchar<20>::castString(row[3].c_str(), row[3].length());
        w_city = Varchar<20>::castString(row[4].c_str(), row[4].length());
        w_state = Char<2>::castString(row[5].c_str(), row[5].length());
        w_zip = Char<9>::castString(row[6].c_str(), row[6].length());
        w_tax = Numeric<4, 4>::castString(row[7].c_str(), row[7].length());
        w_ytd = Numeric<12, 2>::castString(row[8].c_str(), row[8].length());
    };

    inline Integer index() {
        return w_id;
    }
};

typedef Table<Warehouse, Integer> WarehouseTable;

struct District : Indexable<std::tuple<Integer, Integer>> {
    Integer d_id;
    Integer d_w_id;
    Varchar<10> d_name;
    Varchar<20> d_street_1;
    Varchar<20> d_street_2;
    Varchar<20> d_city;
    Char<2> d_state;
    Char<9> d_zip;
    Numeric<4, 4> d_tax;
    Numeric<12, 2> d_ytd;
    Integer d_next_o_id;

    inline std::tuple<Integer, Integer> index() {
        return std::tuple<Integer, Integer>(d_id, d_w_id);
    }

    District(std::vector<std::string> row) {
        d_id = Integer::castString(row[0].c_str(), row[0].length());
        d_w_id = Integer::castString(row[1].c_str(), row[1].length());
        d_name = Varchar<10>::castString(row[2].c_str(), row[2].length());
        d_street_1 = Varchar<20>::castString(row[3].c_str(), row[3].length());
        d_street_2 = Varchar<20>::castString(row[4].c_str(), row[4].length());
        d_city = Varchar<20>::castString(row[5].c_str(), row[5].length());
        d_state = Char<2>::castString(row[6].c_str(), row[6].length());
        d_zip = Char<9>::castString(row[7].c_str(), row[7].length());
        d_tax = Numeric<4, 4>::castString(row[8].c_str(), row[8].length());
        d_ytd = Numeric<12, 2>::castString(row[9].c_str(), row[9].length());
        d_next_o_id = Integer::castString(row[10].c_str(), row[10].length());
    };
};

typedef Table<District, std::tuple<Integer, Integer>> DistrictTable;

struct Customer : Indexable<std::tuple<Integer, Integer, Integer>> {
    Integer c_id;
    Integer c_d_id;
    Integer c_w_id;
    Varchar<16> c_first;
    Char<2> c_middle;
    Varchar<16> c_last;
    Varchar<20> c_street_1;
    Varchar<20> c_street_2;
    Varchar<20> c_city;
    Char<2> c_state;
    Char<9> c_zip;
    Char<16> c_phone;
    Integer c_since;
    Char<2> c_credit;
    Numeric<12, 2> c_credit_lim;
    Numeric<4, 4> c_discount;
    Numeric<12, 2> c_balance;
    Numeric<12, 2> c_ytd_paymenr;
    Numeric<4, 0> c_payment_cnt;
    Numeric<4, 0> c_delivery_cnt;
    Varchar<500> c_data;

    inline std::tuple<Integer, Integer, Integer> index() {
        return std::tuple<Integer, Integer, Integer>(c_w_id, c_d_id, c_id);
    }

    Customer(std::vector<std::string> row) {
        c_id = Integer::castString(row[0].c_str(), row[0].length());
        c_d_id = Integer::castString(row[1].c_str(), row[1].length());
        c_w_id = Integer::castString(row[2].c_str(), row[2].length());
        c_first = Varchar<16>::castString(row[3].c_str(), row[3].length());
        c_middle = Char<2>::castString(row[4].c_str(), row[4].length());
        c_last = Varchar<16>::castString(row[5].c_str(), row[5].length());
        c_street_1 = Varchar<20>::castString(row[6].c_str(), row[6].length());
        c_street_2 = Varchar<20>::castString(row[7].c_str(), row[7].length());
        c_city = Varchar<20>::castString(row[8].c_str(), row[8].length());
        c_state = Char<2>::castString(row[9].c_str(), row[9].length());
        c_zip = Char<9>::castString(row[10].c_str(), row[10].length());
        c_phone = Char<16>::castString(row[11].c_str(), row[11].length());
        c_since = Integer::castString(row[12].c_str(), row[12].length());
        c_credit = Char<2>::castString(row[13].c_str(), row[13].length());
        c_credit_lim = Numeric<12, 2>::castString(row[14].c_str(), row[14].length());
        c_discount = Numeric<4, 4>::castString(row[15].c_str(), row[15].length());
        c_balance = Numeric<12, 2>::castString(row[16].c_str(), row[16].length());
        c_ytd_paymenr = Numeric<12, 2>::castString(row[17].c_str(), row[17].length());
        c_payment_cnt = Numeric<4, 0>::castString(row[18].c_str(), row[18].length());
        c_delivery_cnt = Numeric<4, 0>::castString(row[19].c_str(), row[19].length());
        c_data = Varchar<500>::castString(row[20].c_str(), row[20].length());
    };
};

typedef Table<Customer, std::tuple<Integer, Integer, Integer>> CustomerTable;

//create index customer_wdl on customer(c_w_id,c_d_id,c_last,c_first);

struct History {
    Integer h_c_id;
    Integer h_c_d_id;
    Integer h_c_w_id;
    Integer h_d_id;
    Integer h_w_id;
    Timestamp h_date;
    Numeric<6, 2> h_amount;
    Varchar<24> h_data;
};

struct Neworder : Indexable<std::tuple<Integer, Integer, Integer>> {
    Integer no_o_id;
    Integer no_d_id;
    Integer no_w_id;

    inline std::tuple<Integer, Integer, Integer> index() {
        return std::tuple<Integer, Integer, Integer>(no_w_id, no_d_id, no_o_id);
    }

    Neworder(std::vector<std::string> row) {
        no_o_id = Integer::castString(row[0].c_str(), row[0].length());
        no_d_id = Integer::castString(row[1].c_str(), row[1].length());
        no_w_id = Integer::castString(row[2].c_str(), row[2].length());
    };

    Neworder(int32_t o_id, int32_t d_id, int32_t w_id)
            : no_o_id(Integer(o_id)), no_d_id(Integer(d_id)), no_w_id(Integer(w_id)) {

    }
};

typedef Table<Neworder, std::tuple<Integer, Integer, Integer>> NeworderTable;

struct Order : Indexable<std::tuple<Integer, Integer, Integer>> {
    Integer o_id;
    Integer o_d_id;
    Integer o_w_id;
    Integer o_c_id;
    Integer o_entry_d;
    Integer o_carrier_id;
    Numeric<2, 0> o_ol_cnt;
    Numeric<1, 0> o_all_local;

    inline std::tuple<Integer, Integer, Integer> index() {
        return std::tuple<Integer, Integer, Integer>(o_w_id, o_d_id, o_id);
    }

    Order(std::vector<std::string> row) {
        o_id = Integer::castString(row[0].c_str(), row[0].length());
        o_d_id = Integer::castString(row[1].c_str(), row[1].length());
        o_w_id = Integer::castString(row[2].c_str(), row[2].length());
        o_c_id = Integer::castString(row[3].c_str(), row[3].length());
        o_entry_d = Integer::castString(row[4].c_str(), row[4].length());
        o_carrier_id = Integer::castString(row[5].c_str(), row[5].length());
        o_ol_cnt = Numeric<2, 0>::castString(row[6].c_str(), row[6].length());
        o_all_local = Numeric<1, 0>::castString(row[7].c_str(), row[7].length());
    };

    Order(int32_t o_id, int32_t d_id, int32_t w_id, int32_t c_id, uint64_t entry_d, int32_t carrier_id, int64_t ol_cnt,
            int64_t all_local) :
            o_id(Integer(o_id)),
            o_d_id(Integer(d_id)),
            o_w_id(Integer(w_id)),
            o_c_id(Integer(c_id)),
            o_entry_d(Integer(entry_d)),
            o_carrier_id(Integer(carrier_id)),
            o_ol_cnt(Numeric<2, 0>(ol_cnt)),
            o_all_local(Numeric<1, 0>(all_local)) {
    }
};

typedef Table<Order, std::tuple<Integer, Integer, Integer>> OrderTable;

//create index order_wdc on "order"(o_w_id,o_d_id,o_c_id,o_id);

struct Orderline : Indexable<std::tuple<Integer, Integer, Integer, Integer>> {
    Orderline(Integer o_id, int32_t d_id, int32_t w_id, int i, int32_t itemid, int32_t supware, int i2, int32_t qty, double ol_amount, Char<24> s_dist);

    Integer ol_o_id;
    Integer ol_d_id;
    Integer ol_w_id;
    Integer ol_number;
    Integer ol_i_id;
    Integer ol_supply_w_id;
    Integer ol_delivery_d;
    Numeric<2, 0> ol_quantity;
    Numeric<6, 2> ol_amount;
    Char<24> ol_dist_info;

    inline std::tuple<Integer, Integer, Integer, Integer> index() {
        return std::tuple<Integer, Integer, Integer, Integer>(ol_w_id, ol_d_id, ol_o_id, ol_number);
    }

    Orderline(std::vector<std::string> row) {
        ol_o_id = Integer::castString(row[0].c_str(), row[0].length());
        ol_d_id = Integer::castString(row[1].c_str(), row[1].length());
        ol_w_id = Integer::castString(row[2].c_str(), row[2].length());
        ol_number = Integer::castString(row[3].c_str(), row[3].length());
        ol_i_id = Integer::castString(row[4].c_str(), row[4].length());
        ol_supply_w_id = Integer::castString(row[5].c_str(), row[5].length());
        ol_delivery_d = Integer::castString(row[6].c_str(), row[6].length());
        ol_quantity = Numeric<2, 0>::castString(row[7].c_str(), row[7].length());
        ol_amount = Numeric<6, 2>::castString(row[8].c_str(), row[8].length());
        ol_dist_info = Char<24>::castString(row[9].c_str(), row[9].length());
    };

    Orderline(int32_t o_id, int32_t d_id, int32_t w_id, int32_t number, int32_t i_id, int32_t supply_w_id,
            int32_t delivery_d, int64_t qty, int64_t amount, char* s_dist) :
        ol_o_id(Integer(o_id)), ol_d_id(Integer(d_id)), ol_w_id(Integer(w_id)), ol_number(Integer(number)),
        ol_i_id(Integer(i_id)), ol_supply_w_id(Integer(supply_w_id)), ol_delivery_d(Integer(delivery_d)),
        ol_quantity(Numeric<2, 0>(qty)), ol_amount(Numeric<6, 2>(amount)), ol_dist_info(Char<24>::build(s_dist)) { }
};

typedef Table<Orderline, std::tuple<Integer, Integer, Integer, Integer>> OrderlineTable;

struct Item : Indexable<Integer> {
    Integer i_id;
    Integer i_im_id;
    Varchar<24> i_name;
    Numeric<5, 2> i_price;
    Varchar<50> i_data;

    inline Integer index() {
        return i_id;
    }

    Item(std::vector<std::string> row) {
        i_id = Integer::castString(row[0].c_str(), row[0].length());
        i_im_id = Integer::castString(row[1].c_str(), row[1].length());
        i_name = Varchar<24>::castString(row[2].c_str(), row[2].length());
        i_price = Numeric<5, 2>::castString(row[3].c_str(), row[3].length());
        i_data = Varchar<50>::castString(row[4].c_str(), row[4].length());
    };
};

typedef Table<Item, Integer> ItemTable;

struct Stock : Indexable<std::tuple<Integer, Integer>> {
    Integer s_i_id;
    Integer s_w_id;
    Numeric<4, 0> s_quantity;
    Char<24> s_dist_01;
    Char<24> s_dist_02;
    Char<24> s_dist_03;
    Char<24> s_dist_04;
    Char<24> s_dist_05;
    Char<24> s_dist_06;
    Char<24> s_dist_07;
    Char<24> s_dist_08;
    Char<24> s_dist_09;
    Char<24> s_dist_10;
    Numeric<8, 0> s_ytd;
    Numeric<4, 0> s_order_cnt;
    Numeric<4, 0> s_remote_cnt;
    Varchar<50> s_data;

    inline std::tuple<Integer, Integer> index() {
        return std::tuple<Integer, Integer>(s_w_id, s_i_id);
    }

    Stock(std::vector<std::string> row) {
        s_i_id = Integer::castString(row[0].c_str(), row[0].length());
        s_w_id = Integer::castString(row[1].c_str(), row[1].length());
        s_quantity = Numeric<4, 0>::castString(row[2].c_str(), row[2].length());
        s_dist_01 = Char<24>::castString(row[3].c_str(), row[3].length());
        s_dist_02 = Char<24>::castString(row[4].c_str(), row[4].length());
        s_dist_03 = Char<24>::castString(row[5].c_str(), row[5].length());
        s_dist_04 = Char<24>::castString(row[6].c_str(), row[6].length());
        s_dist_05 = Char<24>::castString(row[7].c_str(), row[7].length());
        s_dist_06 = Char<24>::castString(row[8].c_str(), row[8].length());
        s_dist_07 = Char<24>::castString(row[9].c_str(), row[9].length());
        s_dist_08 = Char<24>::castString(row[10].c_str(), row[10].length());
        s_dist_09 = Char<24>::castString(row[11].c_str(), row[11].length());
        s_dist_10 = Char<24>::castString(row[12].c_str(), row[12].length());
        s_ytd = Numeric<8, 0>::castString(row[13].c_str(), row[13].length());
        s_order_cnt = Numeric<4, 0>::castString(row[14].c_str(), row[14].length());
        s_remote_cnt = Numeric<4, 0>::castString(row[15].c_str(), row[15].length());
        s_data = Varchar<50>::castString(row[16].c_str(), row[16].length());
    };
};

typedef Table<Stock, std::tuple<Integer, Integer>> StockTable;

#pragma clang diagnostic pop



