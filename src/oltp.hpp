#pragma once

#include "table.hpp"
#include "tables_gen.hpp"

static WarehouseTable WAREHOUSE_TABLE = WarehouseTable();
static CustomerTable CUSTOMER_TABLE = CustomerTable();
static DistrictTable DISTRICT_TABLE = DistrictTable();
static OrderTable ORDER_TABLE = OrderTable();
static NeworderTable NEWORDER_TABLE = NeworderTable();
static ItemTable ITEM_TABLE = ItemTable();
static StockTable STOCK_TABLE = StockTable();
static OrderlineTable ORDERLINE_TABLE = OrderlineTable();

void benchmark();
