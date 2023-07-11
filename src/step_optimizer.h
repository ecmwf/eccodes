#pragma once

#include <tuple>
#include <map>
#include <exception>
#include <stdexcept>
#include <string>

enum class Unit {
    SECOND,
    MINUTE,
    HOUR,
    DAY,
    UNKNOWN
};

class Step {
public:
    Step() : value_(0), unit_(Unit::SECOND) {}
    Step(int value, long indicatorOfUnitOfTimeRange);
    Step(int value, Unit unit);

    int value() const { return value_; }
    Unit unit() const { return unit_; }
    char* unit_str() const;

    Step& optimizeUnit();
    Step& setUnit(Unit new_unit);
    bool operator==(const Step& other) const;
    friend std::pair<Step, Step> findCommonUnits(Step startStep, Step endStep);
    friend Step operator+(const Step step1, const Step step2);

private:
    std::map<Unit, int> unitMap_ = {
        {Unit::SECOND, 0},
        {Unit::MINUTE, 60},
        {Unit::HOUR, 60},
        {Unit::DAY, 24}
    };
    int value_;
    Unit unit_;
};

std::pair<Step, Step> findCommonUnits(Step, Step);
