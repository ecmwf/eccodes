#include "step_unit.h"

UnitType::Map UnitType::map_{};

std::vector<Unit> UnitType::unit_order_ = {
    Unit::SECOND,
    Unit::MINUTE,
    Unit::HOUR,
    //Unit::DAY,
};

std::vector<Unit> UnitType::complete_unit_order_ = {
    Unit::MISSING   ,
    Unit::SECOND    ,
    Unit::MINUTE    ,
    Unit::MINUTES15 ,
    Unit::MINUTES30 ,
    Unit::HOUR      ,
    Unit::HOURS3    ,
    Unit::HOURS6    ,
    Unit::HOURS12   ,
    Unit::DAY       ,
    Unit::MONTH     ,
    Unit::YEAR      ,
    Unit::YEARS10   ,
    Unit::YEARS30   ,
    Unit::CENTURY
};

