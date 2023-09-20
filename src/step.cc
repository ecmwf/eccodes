#include <map>
#include <stdexcept>
#include <utility>
#include <limits>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <regex>

#include "step.h"



Step step_from_string(std::string step)
{
    std::regex re("([0-9.]+)([smhDMYC]?)");
    std::smatch match;
    if (std::regex_match(step, match, re)) {
        if (match.size() == 3) {
            std::string value = match[1];
            std::string unit = match[2];
            if (unit.size() == 0) {
                unit = "h";
            }
            Step ret{std::stod(value), Unit{unit}};
            return ret;
        }
    }
    throw std::runtime_error("Could not parse step: " + step);
}


std::vector<Step> parse_range(const std::string& range_str)
{
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


bool Step::operator==(const Step& other) const
{
    if ((internal_value_ == other.internal_value_) && (internal_unit_ == other.internal_unit_)) {
        return true;
    }
    return false;
}


bool Step::operator>(const Step& step) const
{
    auto [a, b] = find_common_units(this->copy().optimize_unit(), step.copy().optimize_unit());
    assert(a.internal_unit_ == b.internal_unit_);
    return a.internal_value_ > b.internal_value_;
}


bool Step::operator<(const Step& step) const
{
    auto [a, b] = find_common_units(this->copy().optimize_unit(), step.copy().optimize_unit());
    assert(a.internal_unit_ == b.internal_unit_);
    return a.internal_value_ < b.internal_value_;
}


Step Step::operator+(const Step& step) const
{
    Step tmp = step;
    auto [a, b] = find_common_units(this->copy().optimize_unit(), tmp.copy().optimize_unit());
    assert(a.internal_unit_ == b.internal_unit_);
    return Step(a.internal_value_ + b.internal_value_, a.internal_unit_);
}


Step Step::operator-(const Step& step) const
{
    Step tmp = step;
    auto [a, b] = find_common_units(this->copy().optimize_unit(), tmp.copy().optimize_unit());
    assert(a.internal_unit_ == b.internal_unit_);
    return Step(a.internal_value_ - b.internal_value_, a.internal_unit_);
}


std::pair<Step, Step> find_common_units(const Step& startStep, const Step& endStep)
{
    Step a = startStep;
    Step b = endStep;

    if (a.internal_value_ == 0 && b.internal_value_ == 0) {
        Unit unit = a.internal_unit_ > b.internal_unit_ ? a.internal_unit_ : b.internal_unit_;
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
        auto it = std::find_if(Unit::unit_order_.begin(), Unit::unit_order_.end(), [&](const auto& e) {
            return e == a.unit().value<Unit::Value>() || e == b.unit().value<Unit::Value>();
        });

        assert(it != Unit::unit_order_.end());

        a.set_unit(*it);
        b.set_unit(*it);
        a.recalculateValue();
        b.recalculateValue();
        assert(a.internal_unit_ == b.internal_unit_);
    }

    return {a, b};
}

void Step::sanity_check() const
{
    static_assert(sizeof(int) == 4, "int is not 4 bytes");
    //if (!(internal_value_ >= std::numeric_limits<int>::min() && internal_value_ <= std::numeric_limits<int>::max())) {
        //throw std::out_of_range("Step is out of range.");
    //}
}


void Step::init_long(long value, const Unit& unit)
{
    internal_value_ = value;
    internal_unit_ = unit;
    unit_ = internal_unit_;
    sanity_check();
}

void Step::init_double(double value, const Unit& unit)
{
    long seconds = Unit::get_converter().unit_to_duration(unit.value<Unit::Value>());
    init_long(static_cast<long>(value * seconds), Unit{Unit::Value::SECOND});
    unit_ = unit;
}

Step& Step::optimize_unit()
{
    if (internal_value_ == 0) {
        return *this;
    }

    unit_ = internal_unit_;
    Seconds<long> seconds = to_seconds<long>(internal_value_, internal_unit_);

    for (auto it = Unit::unit_order_.rbegin(); it != Unit::unit_order_.rend(); ++it) {
        long multiplier = Unit::get_converter().unit_to_duration(*it);
        if (seconds.count() % multiplier == 0) {
            internal_value_ = seconds.count() / multiplier;
            internal_unit_ = *it;
            unit_ = *it;
            return *this;
        }
    }

    return *this;
}
