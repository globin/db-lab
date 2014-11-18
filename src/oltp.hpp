#pragma once

#include "table.hpp"
#include "tables_gen.hpp"


void benchmark();

static Tables TABLES = Tables {
    .warehouseTable = warehouseTable(),
    .customerTable = customerTable(),
    .districtTable = districtTable(),
    .orderTable = orderTable(),
    .neworderTable = neworderTable(),
    .itemTable = itemTable(),
    .stockTable = stockTable(),
    .orderlineTable = orderlineTable()
};
