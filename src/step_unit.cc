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

template <> long Unit::value<long>() const {
    return map_.unit_to_long(internal_value_);
}

template <> Unit::Value Unit::value<Unit::Value>() const {
    return internal_value_;
}

template <> std::string Unit::value<std::string>() const {
    return map_.unit_to_name(internal_value_);
}
