#pragma once

#include "table.hpp"
#include "tables_gen.hpp"

static warehouseTable WAREHOUSE_TABLE = warehouseTable();
static customerTable CUSTOMER_TABLE = customerTable();
static districtTable DISTRICT_TABLE = districtTable();
static orderTable ORDER_TABLE = orderTable();
static neworderTable NEWORDER_TABLE = neworderTable();
static itemTable ITEM_TABLE = itemTable();
static stockTable STOCK_TABLE = stockTable();
static orderlineTable ORDERLINE_TABLE = orderlineTable();

void benchmark();
