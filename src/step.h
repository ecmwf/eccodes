#pragma once

#include <tuple>
#include <map>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <sstream>
#include <optional>

#include "step_unit.h"

class Step {
public:
    // Constructors
    Step() : internal_value_(0), internal_unit_(Unit::Value::SECOND) {}

    Step(double value, const Unit& unit) : internal_unit_{unit}, unit_{internal_unit_} {init_double(value, unit);}
    Step(double value, Unit::Value unit) {init_double(value, Unit{unit});}
    Step(double value, long unit) {init_double(value, Unit{unit});}
    Step(double value, const std::string& unit) {init_double(value, Unit{unit});}

    Step(long value, const Unit& unit) { init_long(value, unit);}
    Step(long value, Unit::Value unit) {init_long(value, Unit{unit});}
    Step(long value, long unit) {init_long(value, Unit{unit});}
    Step(long value, const std::string& unit) {init_long(value, Unit{unit});}

    // Getters
    template <typename T> T value() const;
    template <typename T> T value(const Unit& unit) const;
    Unit unit() const { return unit_; }

    // Setters
    Step& set_unit(const std::string& unit_name) {unit_ = Unit{unit_name}; return *this;}
    Step& set_unit(long unit_code) {unit_ = Unit{unit_code}; return *this;}
    Step& set_unit(const Unit& new_unit) {unit_ = new_unit; return *this;}
    Step& set_unit(const Unit::Value new_unit) {unit_ = new_unit; return *this;}

    // Operators
    bool operator==(const Step& other) const;
    bool operator!=(const Step& other) const;
    Step operator+(const Step& step) const;
    Step operator-(const Step& step) const;
    bool operator>(const Step& step) const;
    bool operator<(const Step& step) const;
    Step copy() const {
        Step ret{};
        ret.internal_value_ = internal_value_;
        ret.internal_unit_ = internal_unit_;
        ret.unit_ = unit_;
        return ret;
    }

    // Methods
    Step& optimize_unit();
    friend std::pair<Step, Step> find_common_units(const Step& startStep, const Step& endStep);

    std::string to_string(const std::string& format) const {
        constexpr int max_size = 128;
        char output[max_size];
        std::string u;

        if (unit_ == Unit::Value::HOUR)
            u = "";
        else
            u =  unit_.to_string();

        int err = snprintf(output, max_size, (format + "%s").c_str(), value<double>(), u.c_str());
        if (err < 0 || err >= max_size) {
            throw std::runtime_error("Error while formatting Step to string");
        }
        return output;
    }

private:
    void init_long(long value, const Unit& unit);
    void init_double(double value, const Unit& unit);
    void sanity_check() const;
    Step& recalculateValue() {
        if (internal_value_ == 0) {
            internal_unit_ = unit_;
            return *this;
        }

        Seconds<long> seconds = to_seconds<long>(internal_value_, internal_unit_);
        long multiplier = Unit::get_converter().unit_to_duration(unit_.to_value());
        internal_value_ = seconds.count() / multiplier;
        internal_unit_ = unit_;

        return *this;
    }

    long internal_value_;
    Unit internal_unit_;
    Unit unit_;
};


Step step_from_string(std::string step);
std::vector<Step> parse_range(const std::string& range_str);
std::pair<Step, Step> find_common_units(const Step& startStep, const Step& endStep);


template <typename T> T Step::value() const {
    if (internal_value_ == 0) {
        return 0;
    }
    if (internal_unit_ == unit_) {
        return internal_value_;
    }
    Seconds<T> seconds = to_seconds<T>(internal_value_, internal_unit_);
    T value = from_seconds<T>(seconds, unit_);
    return value;
}

template <typename T> T Step::value(const Unit& unit) const {
    if (internal_value_ == 0) {
        return 0;
    }
    if (internal_unit_ == unit) {
        return internal_value_;
    }
    Seconds<T> seconds = to_seconds<T>(internal_value_, internal_unit_);
    T value = from_seconds<T>(seconds, unit);
    return value;
}


template <typename T>
Seconds<T> to_seconds(long value, const Unit& unit) {
    Seconds<T> seconds;
    switch (unit.to_value()) {
        case Unit::Value::SECOND: seconds = Seconds<T>(value); break;
        case Unit::Value::MINUTE: seconds = Minutes<T>(value); break;
        case Unit::Value::MINUTES15: seconds = Minutes15<T>(value); break;
        case Unit::Value::MINUTES30: seconds = Minutes30<T>(value); break;
        case Unit::Value::HOUR: seconds = Hours<T>(value); break;
        case Unit::Value::HOURS3: seconds = Hours3<T>(value); break;
        case Unit::Value::HOURS6: seconds = Hours6<T>(value); break;
        case Unit::Value::HOURS12: seconds = Hours12<T>(value); break;
        case Unit::Value::DAY: seconds = Days<T>(value); break;
        case Unit::Value::MONTH: seconds = Months<T>(value); break;
        case Unit::Value::YEAR: seconds = Years<T>(value); break;
        case Unit::Value::YEARS10: seconds = Years10<T>(value); break;
        case Unit::Value::YEARS30: seconds = Years30<T>(value); break;
        case Unit::Value::CENTURY: seconds = Centuries<T>(value); break;
        default:
            std::string msg = "Unknown unit: " + unit.to_string();
            throw std::runtime_error(msg);
    }
    return seconds;
}


template <typename T>
T from_seconds(Seconds<T> seconds, const Unit& unit) {
    T value = 0;
    switch (unit.to_value()) {
        case Unit::Value::SECOND: value = std::chrono::duration_cast<Seconds<T>>(seconds).count(); break;
        case Unit::Value::MINUTE: value = std::chrono::duration_cast<Minutes<T>>(seconds).count(); break;
        case Unit::Value::MINUTES15: value = std::chrono::duration_cast<Minutes15<T>>(seconds).count(); break;
        case Unit::Value::MINUTES30: value = std::chrono::duration_cast<Minutes30<T>>(seconds).count(); break;
        case Unit::Value::HOUR: value = std::chrono::duration_cast<Hours<T>>(seconds).count(); break;
        case Unit::Value::HOURS3: value = std::chrono::duration_cast<Hours3<T>>(seconds).count(); break;
        case Unit::Value::HOURS6: value = std::chrono::duration_cast<Hours6<T>>(seconds).count(); break;
        case Unit::Value::HOURS12: value = std::chrono::duration_cast<Hours12<T>>(seconds).count(); break;
        case Unit::Value::DAY: value = std::chrono::duration_cast<Days<T>>(seconds).count(); break;
        case Unit::Value::MONTH: value = std::chrono::duration_cast<Months<T>>(seconds).count(); break;
        case Unit::Value::YEAR: value = std::chrono::duration_cast<Years<T>>(seconds).count(); break;
        case Unit::Value::YEARS10: value = std::chrono::duration_cast<Years10<T>>(seconds).count(); break;
        case Unit::Value::YEARS30: value = std::chrono::duration_cast<Years30<T>>(seconds).count(); break;
        case Unit::Value::CENTURY: value = std::chrono::duration_cast<Centuries<T>>(seconds).count(); break;
        default:
            std::string msg = "Unknown unit: " + unit.to_string();
            throw std::runtime_error(msg);
    }
    return value;
}

