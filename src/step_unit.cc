#include "step_unit.h"

Unit::Map Unit::map_{};

std::vector<Unit::Value> Unit::unit_order_ = {
    Unit::Value::SECOND,
    Unit::Value::MINUTE,
    Unit::Value::HOUR,
    //Unit::Value::DAY,
};

std::vector<Unit::Value> Unit::complete_unit_order_ = {
    Unit::Value::MISSING   ,
    Unit::Value::SECOND    ,
    Unit::Value::MINUTE    ,
    Unit::Value::MINUTES15 ,
    Unit::Value::MINUTES30 ,
    Unit::Value::HOUR      ,
    Unit::Value::HOURS3    ,
    Unit::Value::HOURS6    ,
    Unit::Value::HOURS12   ,
    Unit::Value::DAY       ,
    Unit::Value::MONTH     ,
    Unit::Value::YEAR      ,
    Unit::Value::YEARS10   ,
    Unit::Value::YEARS30   ,
    Unit::Value::CENTURY
};

