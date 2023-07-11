#include <map>
#include <stdexcept>
#include <utility>
#include <limits>

#include "step_optimizer.h"

char* Step::unit_str() const {
    static char seconds[] = "s";
    static char minutes[] = "m";
    static char hours[] = "h";
    static char days[] = "d";
    switch (unit_) {
        case Unit::SECOND:
            return seconds;
        case Unit::MINUTE:
            return minutes;
        case Unit::HOUR:
            return hours;
        case Unit::DAY:
            return days;
        default:
            std::string msg = "Unknown unit: " + std::to_string(static_cast<int>(unit_));
            throw std::runtime_error(msg);
    }
}

Step::Step(int value, long indicatorOfUnitOfTimeRange) {
    static_assert(sizeof(int) == 4, "int is not 4 bytes");
    if (!(value >= 0 && value <= std::numeric_limits<int>::max())) {
        throw std::out_of_range("Step is out of range.");
    }

    Unit u = Unit::UNKNOWN;
    switch (indicatorOfUnitOfTimeRange) {
        case 0:
            u = Unit::MINUTE;
            break;
        case 1:
            u = Unit::HOUR;
            break;
        case 2:
            u = Unit::DAY;
            break;
        case 254:
            u = Unit::SECOND;
            break;
        default:
            std::string msg = "Unknown indicatorOfUnitOfTimeRange: " + std::to_string(indicatorOfUnitOfTimeRange);
            throw std::runtime_error(msg);
    }
    value_ = value;
    unit_ = u;
}

Step& Step::optimizeUnit() {
    if (value_ == 0) {
        return *this;
    }
    std::map<Unit, int>::iterator it = unitMap_.find(unit_);
    ++it;
    for (; it != unitMap_.end(); ++it){
        Unit u = it->first;
        int multiplier = it->second;
        if (value_ % multiplier == 0) {
            value_ /= multiplier;
            unit_ = u;
        }
        else {
            break;
        }
    }
    return *this;
}

Step& Step::setUnit(Unit new_unit) {
    if (unit_ == new_unit) {
        return *this;
    }
    std::map<Unit, int>::iterator it = unitMap_.find(unit_);
    if (new_unit > unit_) {
        ++it;
        for (; it != unitMap_.find(new_unit); ++it) {
            int multiplier = it->second;
            if (value_ % multiplier == 0) {
                throw std::exception();
            }
            value_ /= multiplier;
            unit_ = it->first;
        }
    }
    else {
        int multiplier = it->second;
        for (; it != unitMap_.find(new_unit);) {
            value_ *= multiplier;
            --it;
            unit_ = it->first;
            multiplier = it->second;
        }
    }
    return *this;
}

bool Step::operator==(const Step& other) const {
    if (value_ == other.value_ && unit_ == other.unit_) {
        return true;
    }
    return false;
}

std::pair<Step, Step> findCommonUnits(Step startStep, Step endStep) {
    if (startStep.value_ == 0 || endStep.value_ == 0) {
        return {startStep, endStep};
    }

    Unit unit = std::min(startStep.optimizeUnit().unit_, endStep.optimizeUnit().unit_);
    startStep.setUnit(unit);
    endStep.setUnit(unit);

    return {startStep, endStep};
}
