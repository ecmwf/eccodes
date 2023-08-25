#include <map>
#include <stdexcept>
#include <utility>
#include <limits>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <regex>

#include "step.h"


UnitType::Map UnitType::map_{};

std::vector<Unit> UnitType::unitOrder = {
            Unit::SECOND,
            Unit::MINUTE,
            Unit::HOUR,
        };


Step step_from_string(std::string step) {
    std::regex re("([0-9.]+)([smhDMYC]?)");
    std::smatch match;
    if (std::regex_match(step, match, re)) {
        if (match.size() == 3) {
            std::string value = match[1];
            std::string unit = match[2];
            if (unit.size() == 0) {
                unit = "h";
            }
            return Step{std::stod(value), UnitType{unit}};
        }
    }
    throw std::runtime_error("Could not parse step: " + step);
}

std::vector<Step> parseRange(const std::string& range_str) {
    std::vector<Step> steps;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = range_str.find("-", prev)) != std::string::npos) {
        steps.push_back(step_from_string(range_str.substr(prev, pos - prev)));
        prev = pos + 1;
    }
    steps.push_back(step_from_string(range_str.substr(prev)));
    return steps;
}


bool Step::operator==(const Step& other) const {
    if ((internal_value_ == other.internal_value_) && (internal_unit_ == other.internal_unit_)) {
        return true;
    }
    return false;
}

Step Step::operator+(const Step& step) {
    Step tmp = step;
    auto [a, b] = findCommonUnits(this->optimizeUnit(), tmp.optimizeUnit());
    assert(a.internal_unit_ == b.internal_unit_);
    return Step(a.internal_value_ + b.internal_value_, a.internal_unit_);
}

Step Step::operator-(const Step& step) {
    Step tmp = step;
    auto [a, b] = findCommonUnits(this->optimizeUnit(), tmp.optimizeUnit());
    assert(a.internal_unit_ == b.internal_unit_);
    if (a.internal_value_ < b.internal_value_) {
        throw std::runtime_error("Negative step not supported");
    }
    return Step(a.internal_value_ - b.internal_value_, a.internal_unit_);
}

std::pair<Step, Step> findCommonUnits(const Step& startStep, const Step& endStep) {
    Step a = startStep;
    Step b = endStep;

    if (a.internal_value_ == 0 && b.internal_value_ == 0) {
        UnitType unit = a.internal_unit_ > b.internal_unit_ ? a.internal_unit_ : b.internal_unit_;
        b.internal_unit_ = unit;
        b.unit_ = unit;
        a.internal_unit_ = unit;
        a.unit_ = unit;
    }
    else if (b.internal_value_ == 0) {
        b.internal_unit_ = a.internal_unit_;
        b.unit_ = a.internal_unit_;
        a.unit_ = a.internal_unit_;
        a.recalculateValue();
    }
    else if (a.internal_value_ == 0) {
        a.internal_unit_ = b.internal_unit_;
        a.unit_ = b.internal_unit_;
        b.unit_ = b.internal_unit_;
        b.recalculateValue();
    }
    else {
        // Find the highest common unit
        auto it = std::find_if(UnitType::unitOrder.begin(), UnitType::unitOrder.end(), [&](const auto& e) {
            return e == a.unit().toValue() || e == b.unit().toValue();
        });

        assert(it != UnitType::unitOrder.end());

        a.setUnit(*it);
        b.setUnit(*it);
        a.recalculateValue();
        b.recalculateValue();
        assert(a.internal_unit_ == b.internal_unit_);
    }

    return {a, b};
}

void Step::sanityCheck() const {
    static_assert(sizeof(int) == 4, "int is not 4 bytes");
    if (!(internal_value_ >= std::numeric_limits<int>::min() && internal_value_ <= std::numeric_limits<int>::max())) {
        throw std::out_of_range("Step is out of range.");
    }
}


void Step::initLong(long value, const UnitType& unit) {
    internal_value_ = value;
    internal_unit_ = unit;
    unit_ = internal_unit_;
    sanityCheck();
}

void Step::initDouble(double value, const UnitType& unit) {
    long seconds = UnitType::getConverter().unit_to_duration(unit.toValue());
    initLong(static_cast<long>(value * seconds), UnitType{Unit::SECOND});
    optimizeUnit();
}

Step::Step(double value, const UnitType& unit) : internal_unit_{unit}, unit_{internal_unit_} {initDouble(value, unit);}
Step::Step(double value, Unit unit) {initDouble(value, UnitType{unit});}
Step::Step(double value, long unit) {initDouble(value, UnitType{unit});}
Step::Step(double value, const std::string& unit) {initDouble(value, UnitType{unit});}

Step::Step(long value, const UnitType& unit) { initLong(value, unit);}
Step::Step(long value, Unit unit) {initLong(value, UnitType{unit});}
Step::Step(long value, long unit) {initLong(value, UnitType{unit});}
Step::Step(long value, const std::string& unit) {initLong(value, UnitType{unit});}

Step::Step(const std::string& str) {
    //size_t pos = str.find_first_of("smhDMYC");
    size_t pos = str.find_first_of("smh");
    if (pos == std::string::npos) {
        throw std::runtime_error("Unknown unit.");
    }
    std::string v_str = str.substr(0, pos);
    std::string u_str = str.substr(pos);
    double v = std::stod(v_str);

    initDouble(v, UnitType{u_str});
}


Step& Step::optimizeUnit() {
    if (internal_value_ == 0) {
        return *this;
    }

    unit_ = internal_unit_;
    Seconds<long> duration(0);
    switch (internal_unit_.toValue()) {
        case Unit::SECOND:
            duration = Seconds<long>(internal_value_);
            break;
        case Unit::MINUTE:
            duration = Minutes<long>(internal_value_);
            break;
        case Unit::HOUR:
            duration = Hours<long>(internal_value_);
            break;
        case Unit::DAY:
            duration = Days<long>(internal_value_);
            break;
        case Unit::MONTH:
            duration = Months<long>(internal_value_);
            break;
        default:
            std::string msg = "Unknown unit: " + internal_unit_.toString();
            throw std::runtime_error(msg);
    }

    Seconds<long> d = std::chrono::duration_cast<Seconds<long>>(duration);

    for (auto it = UnitType::unitOrder.rbegin(); it != UnitType::unitOrder.rend(); ++it) {
        long multiplier = UnitType::getConverter().unit_to_duration(*it);
        if (d.count() % multiplier == 0) {
            internal_value_ = duration.count() / multiplier;
            internal_unit_ = *it;
            unit_ = *it;
            return *this;
        }
    }

    return *this;
}

Step& Step::setUnit(const std::string& unit_name) {
    unit_ = UnitType{unit_name};
    return *this;
}

Step& Step::setUnit(long unit_code) {
    unit_ = UnitType{unit_code};
    return *this;
}

Step& Step::setUnit(const UnitType& new_unit) {
    unit_ = new_unit;
    return *this;
}

Step& Step::setUnit(const Unit new_unit) {
    unit_ = new_unit;
    return *this;
}

