#pragma once

#include "types.hpp"
#include "table.hpp"

struct Warehouse {
    Integer w_id;
    Varchar<10> w_name;
    Varchar<20> w_street_1;
    Varchar<20> w_street_2;
    Varchar<20> w_city;
    Char<2> w_state;
    Char<9> w_zip;
    Numeric<4, 4> w_tax;
    Numeric<12, 2> w_ytd;

    Warehouse(
        Integer w_id,
        Varchar<10> w_name,
        Varchar<20> w_street_1,
        Varchar<20> w_street_2,
        Varchar<20> w_city,
        Char<2> w_state,
        Char<9> w_zip,
        Numeric<4, 4> w_tax,
        Numeric<12, 2> w_ytd
    ) : 
        w_id(w_id),
        w_name(w_name),
        w_street_1(w_street_1),
        w_street_2(w_street_2),
        w_city(w_city),
        w_state(w_state),
        w_zip(w_zip),
        w_tax(w_tax),
        w_ytd(w_ytd)
    { }

    static Warehouse from_row(vector<string> row) {
        return Warehouse(
            Integer::castString(row[0].c_str(), row[0].length()),
            Varchar<10>::castString(row[1].c_str(), row[1].length()),
            Varchar<20>::castString(row[2].c_str(), row[2].length()),
            Varchar<20>::castString(row[3].c_str(), row[3].length()),
            Varchar<20>::castString(row[4].c_str(), row[4].length()),
            Char<2>::castString(row[5].c_str(), row[5].length()),
            Char<9>::castString(row[6].c_str(), row[6].length()),
            Numeric<4, 4>::castString(row[7].c_str(), row[7].length()),
            Numeric<12, 2>::castString(row[8].c_str(), row[8].length())
        );
    }

    Integer index() {
        return w_id;
    }
};

typedef Table<Warehouse, Integer> WarehouseTable;

struct District {
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

    District(
        Integer d_id,
        Integer d_w_id,
        Varchar<10> d_name,
        Varchar<20> d_street_1,
        Varchar<20> d_street_2,
        Varchar<20> d_city,
        Char<2> d_state,
        Char<9> d_zip,
        Numeric<4, 4> d_tax,
        Numeric<12, 2> d_ytd,
        Integer d_next_o_id
    ) : 
        d_id(d_id),
        d_w_id(d_w_id),
        d_name(d_name),
        d_street_1(d_street_1),
        d_street_2(d_street_2),
        d_city(d_city),
        d_state(d_state),
        d_zip(d_zip),
        d_tax(d_tax),
        d_ytd(d_ytd),
        d_next_o_id(d_next_o_id)
    { }

    static District from_row(vector<string> row) {
        return District(
            Integer::castString(row[0].c_str(), row[0].length()),
            Integer::castString(row[1].c_str(), row[1].length()),
            Varchar<10>::castString(row[2].c_str(), row[2].length()),
            Varchar<20>::castString(row[3].c_str(), row[3].length()),
            Varchar<20>::castString(row[4].c_str(), row[4].length()),
            Varchar<20>::castString(row[5].c_str(), row[5].length()),
            Char<2>::castString(row[6].c_str(), row[6].length()),
            Char<9>::castString(row[7].c_str(), row[7].length()),
            Numeric<4, 4>::castString(row[8].c_str(), row[8].length()),
            Numeric<12, 2>::castString(row[9].c_str(), row[9].length()),
            Integer::castString(row[10].c_str(), row[10].length())
        );
    }

    std::tuple<Integer, Integer> index() {
        return std::tuple<Integer, Integer>(d_w_id, d_id);
    }
};

typedef Table<District, std::tuple<Integer, Integer>> DistrictTable;

struct Customer {
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
    Timestamp c_since;
    Char<2> c_credit;
    Numeric<12, 2> c_credit_lim;
    Numeric<4, 4> c_discount;
    Numeric<12, 2> c_balance;
    Numeric<12, 2> c_ytd_paymenr;
    Numeric<4, 0> c_payment_cnt;
    Numeric<4, 0> c_delivery_cnt;
    Varchar<500> c_data;

    Customer(
        Integer c_id,
        Integer c_d_id,
        Integer c_w_id,
        Varchar<16> c_first,
        Char<2> c_middle,
        Varchar<16> c_last,
        Varchar<20> c_street_1,
        Varchar<20> c_street_2,
        Varchar<20> c_city,
        Char<2> c_state,
        Char<9> c_zip,
        Char<16> c_phone,
        Timestamp c_since,
        Char<2> c_credit,
        Numeric<12, 2> c_credit_lim,
        Numeric<4, 4> c_discount,
        Numeric<12, 2> c_balance,
        Numeric<12, 2> c_ytd_paymenr,
        Numeric<4, 0> c_payment_cnt,
        Numeric<4, 0> c_delivery_cnt,
        Varchar<500> c_data
    ) : 
        c_id(c_id),
        c_d_id(c_d_id),
        c_w_id(c_w_id),
        c_first(c_first),
        c_middle(c_middle),
        c_last(c_last),
        c_street_1(c_street_1),
        c_street_2(c_street_2),
        c_city(c_city),
        c_state(c_state),
        c_zip(c_zip),
        c_phone(c_phone),
        c_since(c_since),
        c_credit(c_credit),
        c_credit_lim(c_credit_lim),
        c_discount(c_discount),
        c_balance(c_balance),
        c_ytd_paymenr(c_ytd_paymenr),
        c_payment_cnt(c_payment_cnt),
        c_delivery_cnt(c_delivery_cnt),
        c_data(c_data)
    { }

    static Customer from_row(vector<string> row) {
        return Customer(
            Integer::castString(row[0].c_str(), row[0].length()),
            Integer::castString(row[1].c_str(), row[1].length()),
            Integer::castString(row[2].c_str(), row[2].length()),
            Varchar<16>::castString(row[3].c_str(), row[3].length()),
            Char<2>::castString(row[4].c_str(), row[4].length()),
            Varchar<16>::castString(row[5].c_str(), row[5].length()),
            Varchar<20>::castString(row[6].c_str(), row[6].length()),
            Varchar<20>::castString(row[7].c_str(), row[7].length()),
            Varchar<20>::castString(row[8].c_str(), row[8].length()),
            Char<2>::castString(row[9].c_str(), row[9].length()),
            Char<9>::castString(row[10].c_str(), row[10].length()),
            Char<16>::castString(row[11].c_str(), row[11].length()),
            Timestamp::castString(row[12].c_str(), row[12].length()),
            Char<2>::castString(row[13].c_str(), row[13].length()),
            Numeric<12, 2>::castString(row[14].c_str(), row[14].length()),
            Numeric<4, 4>::castString(row[15].c_str(), row[15].length()),
            Numeric<12, 2>::castString(row[16].c_str(), row[16].length()),
            Numeric<12, 2>::castString(row[17].c_str(), row[17].length()),
            Numeric<4, 0>::castString(row[18].c_str(), row[18].length()),
            Numeric<4, 0>::castString(row[19].c_str(), row[19].length()),
            Varchar<500>::castString(row[20].c_str(), row[20].length())
        );
    }

    std::tuple<Integer, Integer, Integer> index() {
        return std::tuple<Integer, Integer, Integer>(c_w_id, c_d_id, c_id);
    }
};

typedef Table<Customer, std::tuple<Integer, Integer, Integer>> CustomerTable;

struct History {
    Integer h_c_id;
    Integer h_c_d_id;
    Integer h_c_w_id;
    Integer h_d_id;
    Integer h_w_id;
    Timestamp h_date;
    Numeric<6, 2> h_amount;
    Varchar<24> h_data;

    History(
        Integer h_c_id,
        Integer h_c_d_id,
        Integer h_c_w_id,
        Integer h_d_id,
        Integer h_w_id,
        Timestamp h_date,
        Numeric<6, 2> h_amount,
        Varchar<24> h_data
    ) : 
        h_c_id(h_c_id),
        h_c_d_id(h_c_d_id),
        h_c_w_id(h_c_w_id),
        h_d_id(h_d_id),
        h_w_id(h_w_id),
        h_date(h_date),
        h_amount(h_amount),
        h_data(h_data)
    { }

    static History from_row(vector<string> row) {
        return History(
            Integer::castString(row[0].c_str(), row[0].length()),
            Integer::castString(row[1].c_str(), row[1].length()),
            Integer::castString(row[2].c_str(), row[2].length()),
            Integer::castString(row[3].c_str(), row[3].length()),
            Integer::castString(row[4].c_str(), row[4].length()),
            Timestamp::castString(row[5].c_str(), row[5].length()),
            Numeric<6, 2>::castString(row[6].c_str(), row[6].length()),
            Varchar<24>::castString(row[7].c_str(), row[7].length())
        );
    }

    std::tuple<> index() {
        return std::tuple<>();
    }
};

typedef Table<History, std::tuple<>> HistoryTable;

struct Neworder {
    Integer no_o_id;
    Integer no_d_id;
    Integer no_w_id;

    Neworder(
        Integer no_o_id,
        Integer no_d_id,
        Integer no_w_id
    ) : 
        no_o_id(no_o_id),
        no_d_id(no_d_id),
        no_w_id(no_w_id)
    { }

    static Neworder from_row(vector<string> row) {
        return Neworder(
            Integer::castString(row[0].c_str(), row[0].length()),
            Integer::castString(row[1].c_str(), row[1].length()),
            Integer::castString(row[2].c_str(), row[2].length())
        );
    }

    std::tuple<Integer, Integer, Integer> index() {
        return std::tuple<Integer, Integer, Integer>(no_w_id, no_d_id, no_o_id);
    }
};

typedef Table<Neworder, std::tuple<Integer, Integer, Integer>> NeworderTable;

struct Order {
    Integer o_id;
    Integer o_d_id;
    Integer o_w_id;
    Integer o_c_id;
    Timestamp o_entry_d;
    Integer o_carrier_id;
    Numeric<2, 0> o_ol_cnt;
    Numeric<1, 0> o_all_local;

    Order(
        Integer o_id,
        Integer o_d_id,
        Integer o_w_id,
        Integer o_c_id,
        Timestamp o_entry_d,
        Integer o_carrier_id,
        Numeric<2, 0> o_ol_cnt,
        Numeric<1, 0> o_all_local
    ) : 
        o_id(o_id),
        o_d_id(o_d_id),
        o_w_id(o_w_id),
        o_c_id(o_c_id),
        o_entry_d(o_entry_d),
        o_carrier_id(o_carrier_id),
        o_ol_cnt(o_ol_cnt),
        o_all_local(o_all_local)
    { }

    static Order from_row(vector<string> row) {
        return Order(
            Integer::castString(row[0].c_str(), row[0].length()),
            Integer::castString(row[1].c_str(), row[1].length()),
            Integer::castString(row[2].c_str(), row[2].length()),
            Integer::castString(row[3].c_str(), row[3].length()),
            Timestamp::castString(row[4].c_str(), row[4].length()),
            Integer::castString(row[5].c_str(), row[5].length()),
            Numeric<2, 0>::castString(row[6].c_str(), row[6].length()),
            Numeric<1, 0>::castString(row[7].c_str(), row[7].length())
        );
    }

    std::tuple<Integer, Integer, Integer> index() {
        return std::tuple<Integer, Integer, Integer>(o_w_id, o_d_id, o_id);
    }
};

typedef Table<Order, std::tuple<Integer, Integer, Integer>> OrderTable;

struct Orderline {
    Integer ol_o_id;
    Integer ol_d_id;
    Integer ol_w_id;
    Integer ol_number;
    Integer ol_i_id;
    Integer ol_supply_w_id;
    Timestamp ol_delivery_d;
    Numeric<2, 0> ol_quantity;
    Numeric<6, 2> ol_amount;
    Char<24> ol_dist_info;

    Orderline(
        Integer ol_o_id,
        Integer ol_d_id,
        Integer ol_w_id,
        Integer ol_number,
        Integer ol_i_id,
        Integer ol_supply_w_id,
        Timestamp ol_delivery_d,
        Numeric<2, 0> ol_quantity,
        Numeric<6, 2> ol_amount,
        Char<24> ol_dist_info
    ) : 
        ol_o_id(ol_o_id),
        ol_d_id(ol_d_id),
        ol_w_id(ol_w_id),
        ol_number(ol_number),
        ol_i_id(ol_i_id),
        ol_supply_w_id(ol_supply_w_id),
        ol_delivery_d(ol_delivery_d),
        ol_quantity(ol_quantity),
        ol_amount(ol_amount),
        ol_dist_info(ol_dist_info)
    { }

    static Orderline from_row(vector<string> row) {
        return Orderline(
            Integer::castString(row[0].c_str(), row[0].length()),
            Integer::castString(row[1].c_str(), row[1].length()),
            Integer::castString(row[2].c_str(), row[2].length()),
            Integer::castString(row[3].c_str(), row[3].length()),
            Integer::castString(row[4].c_str(), row[4].length()),
            Integer::castString(row[5].c_str(), row[5].length()),
            Timestamp::castString(row[6].c_str(), row[6].length()),
            Numeric<2, 0>::castString(row[7].c_str(), row[7].length()),
            Numeric<6, 2>::castString(row[8].c_str(), row[8].length()),
            Char<24>::castString(row[9].c_str(), row[9].length())
        );
    }

    std::tuple<Integer, Integer, Integer, Integer> index() {
        return std::tuple<Integer, Integer, Integer, Integer>(ol_w_id, ol_d_id, ol_o_id, ol_number);
    }
};

typedef Table<Orderline, std::tuple<Integer, Integer, Integer, Integer>> OrderlineTable;

struct Item {
    Integer i_id;
    Integer i_im_id;
    Varchar<24> i_name;
    Numeric<5, 2> i_price;
    Varchar<50> i_data;

    Item(
        Integer i_id,
        Integer i_im_id,
        Varchar<24> i_name,
        Numeric<5, 2> i_price,
        Varchar<50> i_data
    ) : 
        i_id(i_id),
        i_im_id(i_im_id),
        i_name(i_name),
        i_price(i_price),
        i_data(i_data)
    { }

    static Item from_row(vector<string> row) {
        return Item(
            Integer::castString(row[0].c_str(), row[0].length()),
            Integer::castString(row[1].c_str(), row[1].length()),
            Varchar<24>::castString(row[2].c_str(), row[2].length()),
            Numeric<5, 2>::castString(row[3].c_str(), row[3].length()),
            Varchar<50>::castString(row[4].c_str(), row[4].length())
        );
    }

    Integer index() {
        return i_id;
    }
};

typedef Table<Item, Integer> ItemTable;

struct Stock {
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

    Stock(
        Integer s_i_id,
        Integer s_w_id,
        Numeric<4, 0> s_quantity,
        Char<24> s_dist_01,
        Char<24> s_dist_02,
        Char<24> s_dist_03,
        Char<24> s_dist_04,
        Char<24> s_dist_05,
        Char<24> s_dist_06,
        Char<24> s_dist_07,
        Char<24> s_dist_08,
        Char<24> s_dist_09,
        Char<24> s_dist_10,
        Numeric<8, 0> s_ytd,
        Numeric<4, 0> s_order_cnt,
        Numeric<4, 0> s_remote_cnt,
        Varchar<50> s_data
    ) : 
        s_i_id(s_i_id),
        s_w_id(s_w_id),
        s_quantity(s_quantity),
        s_dist_01(s_dist_01),
        s_dist_02(s_dist_02),
        s_dist_03(s_dist_03),
        s_dist_04(s_dist_04),
        s_dist_05(s_dist_05),
        s_dist_06(s_dist_06),
        s_dist_07(s_dist_07),
        s_dist_08(s_dist_08),
        s_dist_09(s_dist_09),
        s_dist_10(s_dist_10),
        s_ytd(s_ytd),
        s_order_cnt(s_order_cnt),
        s_remote_cnt(s_remote_cnt),
        s_data(s_data)
    { }

    static Stock from_row(vector<string> row) {
        return Stock(
            Integer::castString(row[0].c_str(), row[0].length()),
            Integer::castString(row[1].c_str(), row[1].length()),
            Numeric<4, 0>::castString(row[2].c_str(), row[2].length()),
            Char<24>::castString(row[3].c_str(), row[3].length()),
            Char<24>::castString(row[4].c_str(), row[4].length()),
            Char<24>::castString(row[5].c_str(), row[5].length()),
            Char<24>::castString(row[6].c_str(), row[6].length()),
            Char<24>::castString(row[7].c_str(), row[7].length()),
            Char<24>::castString(row[8].c_str(), row[8].length()),
            Char<24>::castString(row[9].c_str(), row[9].length()),
            Char<24>::castString(row[10].c_str(), row[10].length()),
            Char<24>::castString(row[11].c_str(), row[11].length()),
            Char<24>::castString(row[12].c_str(), row[12].length()),
            Numeric<8, 0>::castString(row[13].c_str(), row[13].length()),
            Numeric<4, 0>::castString(row[14].c_str(), row[14].length()),
            Numeric<4, 0>::castString(row[15].c_str(), row[15].length()),
            Varchar<50>::castString(row[16].c_str(), row[16].length())
        );
    }

    std::tuple<Integer, Integer> index() {
        return std::tuple<Integer, Integer>(s_w_id, s_i_id);
    }
};

typedef Table<Stock, std::tuple<Integer, Integer>> StockTable;

