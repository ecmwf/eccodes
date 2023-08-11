#include <map>
#include <stdexcept>
#include <utility>
#include <limits>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "step_optimizer.h"

std::vector<std::pair<Unit, long>> Step::unitOrder = {
        {Unit::SECOND, 1},
        {Unit::MINUTE, 60},
        //{Unit::MINUTES15, 900},
        //{Unit::MINUTES30, 1800},
        {Unit::HOUR, 3600},
        //{Unit::HOURS3, 10800},
        //{Unit::HOURS6, 21600},
        //{Unit::HOURS12, 43200},
        {Unit::DAY, 86400},
        {Unit::MONTH, 2592000},
        //{Unit::YEAR, 31536000},
        //{Unit::YEARS10, 315360000},
        //{Unit::YEARS30, 946080000},
        //{Unit::CENTURY, 3153600000},
    };

std::string parse_step(std::string step) {
    if (step.find_first_of("smhdMYC") == std::string::npos) {
        step += "h";
    }
    return step;
}


std::vector<Step> parse_range(const std::string& range_str) {
    std::vector<Step> steps;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = range_str.find("-", prev)) != std::string::npos) {
        std::string token = parse_step(range_str.substr(prev, pos - prev));
        if (token.size() > 0) {
            steps.push_back(Step(token));
        }
        prev = pos + 1;
    }
    std::string token = parse_step(range_str.substr(prev));
    if (token.size() > 0) {
        steps.push_back(Step(token));
    }
    return steps;
}


std::string Step::unit_as_str() const {
    if ((unit_ == Unit::HOUR) && hide_hour_unit_)
        return std::string("");
    else
        return StepUnitsTable::to_str(unit_);
}

long Step::unit_as_long() const {
    return unit_;
}

Step::Step(int value, long unit) {
    static_assert(sizeof(int) == 4, "int is not 4 bytes");
    if (!(value >= 0 && value <= std::numeric_limits<int>::max())) {
        throw std::out_of_range("Step is out of range.");
    }

    value_ = value;
    unit_ = StepUnitsTable::to_unit(unit);
}

Step::Step(const std::string& str) {
    size_t pos = str.find_first_of("smhdMYC");
    if (pos == std::string::npos) {
        throw std::runtime_error("Unknown unit.");
    }
    std::string v_str = str.substr(0, pos);
    std::string u_str = str.substr(pos);
    int v = std::stoi(v_str);
    value_ = v;
    unit_ = StepUnitsTable::to_unit(u_str);
}

Step::Step(int value, Unit u) {
    static_assert(sizeof(int) == 4, "int is not 4 bytes");
    if (!(value >= 0 && value <= std::numeric_limits<int>::max())) {
        throw std::out_of_range("Step is out of range.");
    }
    value_ = value;
    unit_ = u;
}

Step& Step::optimizeUnit() {
    if (value_ == 0) {
        return *this;
    }

    Seconds duration(0);
    switch (unit_) {
        case Unit::SECOND:
            duration = Seconds(value_);
            break;
        case Unit::MINUTE:
            duration = Minutes(value_);
            break;
        case Unit::HOUR:
            duration = Hours(value_);
            break;
        case Unit::DAY:
            duration = Days(value_);
            break;
        case Unit::MONTH:
            duration = Months(value_);
            break;
        default:
            std::string msg = "Unknown unit: " + StepUnitsTable::to_str(unit_);
            throw std::runtime_error(msg);
    }

    Seconds d = std::chrono::duration_cast<Seconds>(duration);

    for (auto it = unitOrder.rbegin(); it != unitOrder.rend(); ++it) {
        int multiplier = it->second;
        if (d.count() % multiplier == 0) {
            value_ = duration.count() / multiplier;
            unit_ = it->first;
            return *this;
        }
    }

    return *this;
}

Step& Step::setUnit(std::string new_unit) {
    setUnit(StepUnitsTable::to_unit(new_unit));
    return *this;
}

Step& Step::setUnit(long new_unit) {
    setUnit(StepUnitsTable::to_unit(new_unit));
    return *this;
}

Step& Step::setUnit(Unit new_unit) {
    if (value_ == 0) {
        unit_ = new_unit;
        return *this;
    }
    if (unit_ == new_unit) {
        return *this;
    }
    Seconds duration(0);
    switch (unit_) {
        case Unit::SECOND:
            duration = Seconds(value_);
            break;
        case Unit::MINUTE:
            duration = Minutes(value_);
            break;
        case Unit::HOUR:
            duration = Hours(value_);
            break;
        case Unit::DAY:
            duration = Days(value_);
            break;
        case Unit::MONTH:
            duration = Months(value_);
            break;
        default:
            std::string msg = "Unknown unit: " + std::to_string(static_cast<int>(unit_));
            throw std::runtime_error(msg);
    }

    switch (new_unit) {
        case Unit::SECOND:
            value_ = duration.count();
            break;
        case Unit::MINUTE:
            value_ = std::chrono::duration_cast<Minutes>(duration).count();
            break;
        case Unit::HOUR:
            value_ = std::chrono::duration_cast<Hours>(duration).count();
            break;
        case Unit::DAY:
            value_ = std::chrono::duration_cast<Days>(duration).count();
            break;
        case Unit::MONTH:
            value_ = std::chrono::duration_cast<Months>(duration).count();
            break;
        default:
            std::string msg = "Unknown unit: " + std::to_string(static_cast<int>(new_unit));
            throw std::runtime_error(msg);
    }
    unit_ = new_unit;

    return *this;
}

double Step::getDoubleValue(long new_unit) const {
    Seconds duration(0);
    switch (unit_) {
        case Unit::SECOND:
            duration = Seconds(value_);
            break;
        case Unit::MINUTE:
            duration = Minutes(value_);
            break;
        case Unit::HOUR:
            duration = Hours(value_);
            break;
        case Unit::DAY:
            duration = Days(value_);
            break;
        case Unit::MONTH:
            duration = Months(value_);
            break;
        default:
            std::string msg = "Unknown unit: " + std::to_string(static_cast<int>(unit_));
            throw std::runtime_error(msg);
    }

    double value;
    switch (new_unit) {
        case Unit::SECOND:
            value = std::chrono::duration_cast<SecondsDouble>(duration).count();
            break;
        case Unit::MINUTE:
            value = std::chrono::duration_cast<MinutesDouble>(duration).count();
            break;
        case Unit::HOUR:
            value = std::chrono::duration_cast<HoursDouble>(duration).count();
            break;
        case Unit::DAY:
            value = std::chrono::duration_cast<DaysDouble>(duration).count();
            break;
        case Unit::MONTH:
            value = std::chrono::duration_cast<MonthsDouble>(duration).count();
            break;
        default:
            std::string msg = "Unknown unit: " + std::to_string(static_cast<int>(new_unit));
            throw std::runtime_error(msg);
    }

    return value;
}

bool Step::operator==(const Step& other) const {
    if (value_ == other.value_ && unit_ == other.unit_) {
        return true;
    }
    return false;
}

Step operator+(const Step step1, const Step step2) {
    auto [a, b] = findCommonUnits(step1, step2);
    return Step(a.value_ + b.value_, a.unit_);
}

std::pair<Step, Step> findCommonUnits(Step startStep, Step endStep) {
    if (startStep.value_ == 0 || endStep.value_ == 0) {
        if (startStep.value_ == 0 && endStep.value_ == 0) {
            Unit unit = StepUnitsTable::unit_duration(startStep.unit_) > StepUnitsTable::unit_duration(endStep.unit_) ? startStep.unit_ : endStep.unit_;
            startStep.setUnit(unit);
            endStep.setUnit(unit);
        }
        else if (startStep.value_ == 0) {
            startStep.setUnit(endStep.unit_);
        }
        else if (endStep.value_ == 0) {
            endStep.setUnit(startStep.unit_);
        }
        return {startStep, endStep};
    }

    auto it = std::find_if(Step::unitOrder.begin(), Step::unitOrder.end(), [&](const auto& e) {
        return e.first == startStep.unit_ || e.first == endStep.unit_;
    });

    assert(it != Step::unitOrder.end());

    startStep.setUnit(it->first);
    endStep.setUnit(it->first);

    return {startStep, endStep};
}
