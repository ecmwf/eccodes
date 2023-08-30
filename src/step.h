#pragma once

#include <tuple>
#include <map>
#include <exception>
#include <stdexcept>
#include <string>
#include <chrono>
#include <vector>
#include <tuple>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <sstream>
#include <optional>


template <typename T> using Minutes = std::chrono::duration<T, std::ratio<60>>;
template <typename T> using Hours = std::chrono::duration<T, std::ratio<3600>>;
template <typename T> using Days = std::chrono::duration<T, std::ratio<86400>>;
template <typename T> using Months = std::chrono::duration<T, std::ratio<2592000>>;
template <typename T> using Years = std::chrono::duration<T, std::ratio<31536000>>;
template <typename T> using Years10 = std::chrono::duration<T, std::ratio<315360000>>;
template <typename T> using Years30 = std::chrono::duration<T, std::ratio<946080000>>;
template <typename T> using Centuries = std::chrono::duration<T, std::ratio<3153600000>>;
template <typename T> using Hours3 = std::chrono::duration<T, std::ratio<10800>>;
template <typename T> using Hours6 = std::chrono::duration<T, std::ratio<21600>>;
template <typename T> using Hours12 = std::chrono::duration<T, std::ratio<43200>>;
template <typename T> using Seconds = std::chrono::duration<T, std::ratio<1>>;
template <typename T> using Minutes15 = std::chrono::duration<T, std::ratio<900>>;
template <typename T> using Minutes30 = std::chrono::duration<T, std::ratio<1800>>;
template <typename T> using Missing = std::chrono::duration<T, std::ratio<0>>;

enum class Unit {
    MINUTE = 0,
    HOUR = 1,
    DAY = 2,
    MONTH = 3,
    YEAR = 4,
    YEARS10 = 5,
    YEARS30 = 6,
    CENTURY = 7,
    HOURS3 = 10,
    HOURS6 = 11,
    HOURS12 = 12,
    SECOND = 13,
    MINUTES15 = 14,
    MINUTES30 = 15,
    MISSING = 255,
};

class UnitType;
template <typename T> Seconds<T> to_seconds(long value, const UnitType& unit);
template <typename T> T from_seconds(Seconds<T> seconds, const UnitType& unit);

class UnitType {
public:
    UnitType() : internal_value_(Unit::HOUR) {}
    explicit UnitType(Unit unit_value) : internal_value_(unit_value) {}
    explicit UnitType(const std::string& unit_value) {internal_value_ = map_.name_to_unit(unit_value);}
    explicit UnitType(long unit_value) {internal_value_ = map_.long_to_unit(unit_value);}

    bool operator>(const UnitType& other) const {return map_.unit_to_duration(internal_value_) > map_.unit_to_duration(other.internal_value_);}
    bool operator==(const Unit value) const {return map_.unit_to_duration(internal_value_) == map_.unit_to_duration(value);}
    bool operator==(const UnitType& unit) const {return map_.unit_to_duration(internal_value_) == map_.unit_to_duration(unit.internal_value_);}
    bool operator!=(const UnitType& unit) const {return !(*this == unit);}
    bool operator!=(const Unit value) const {return !(*this == value);}

    UnitType& operator=(const Unit value) {
        internal_value_ = value;
        return *this;
    }

    std::string to_string() const {
        if ((internal_value_ == Unit::HOUR) && hide_hour_unit_) {
            return "";
        }
        else {
            return map_.unit_to_name(internal_value_);
        }
    }
    long to_long() const {return map_.unit_to_long(internal_value_);}
    Unit to_value() const {return internal_value_;}
    void hide_hour_unit() {hide_hour_unit_ = true;}
    void show_hour_unit() {hide_hour_unit_ = false;}
    static std::vector<Unit> unit_order_;
    static std::vector<Unit> complete_unit_order_;

private:
    bool hide_hour_unit_ = false;
    class Map {
    public:
        Map() {
            for (const auto& entry : tab_) {
                // unit_value <-> unit_name
                name_to_value_[entry.unit_name] = entry.unit_value;
                value_to_name_[entry.unit_value] = entry.unit_name;

                // unit_value <-> duration in seconds
                value_to_duration_[entry.unit_value] = entry.duration;
                duration_to_value_[entry.duration] = entry.unit_value;

                // unit_value <-> wmo_code
                value_to_long_[entry.unit_value] = static_cast<long>(entry.unit_value);
                long_to_value_[static_cast<long>(entry.unit_value)] = entry.unit_value;
            }
        }

        // wmo_code <-> unit_name
        std::string unit_to_name(const Unit& unit_value) const {return value_to_name_.at(unit_value);}
        Unit name_to_unit(const std::string& name) const {return name_to_value_.at(name);}

        // unit_value <-> duration
        long unit_to_duration(const Unit& unit_value) const {return value_to_duration_.at(unit_value);}
        Unit duration_to_unit(long duration) const {return duration_to_value_.at(duration);}

        // wmo_code <-> unit_name
        long unit_to_long(const Unit& unit_value) const {return value_to_long_.at(unit_value);}
        Unit long_to_unit(long wmo_code) const {return long_to_value_.at(wmo_code);}

    private:
        struct Entry {
            Unit unit_value;
            std::string unit_name;
            long duration;
        };

        const std::array<Entry, 15> tab_ = {
            Entry{Unit::MISSING   , "MISSING" , 0},
            Entry{Unit::SECOND    , "s"       , 1},
            Entry{Unit::MINUTE    , "m"       , 60},
            Entry{Unit::MINUTES15 , "15m"     , 900},
            Entry{Unit::MINUTES30 , "30m"     , 1800},
            Entry{Unit::HOUR      , "h"       , 3600},
            Entry{Unit::HOURS3    , "3h"      , 10800},
            Entry{Unit::HOURS6    , "6h"      , 21600},
            Entry{Unit::HOURS12   , "12h"     , 43200},
            Entry{Unit::DAY       , "D"       , 86400},
            Entry{Unit::MONTH     , "M"       , 2592000},
            Entry{Unit::YEAR      , "Y"       , 31536000},
            Entry{Unit::YEARS10   , "10Y"     , 315360000},
            Entry{Unit::YEARS30   , "30Y"     , 946080000},
            Entry{Unit::CENTURY   , "C"       , 3153600000},
        };

        std::unordered_map<std::string, Unit> name_to_value_;
        std::unordered_map<Unit, std::string> value_to_name_;

        std::unordered_map<Unit, long> value_to_long_;
        std::unordered_map<long, Unit> long_to_value_;

        std::unordered_map<Unit, long> value_to_duration_;
        std::unordered_map<long, Unit> duration_to_value_;
    };


    Unit internal_value_;
    static Map map_;
public:
    static Map& get_converter() {return map_;}
};


class Step {
public:
    // Constructors
    Step() : internal_value_(0), internal_unit_(Unit::SECOND) {}

    Step(double value, const UnitType& unit) : internal_unit_{unit}, unit_{internal_unit_} {init_double(value, unit);}
    Step(double value, Unit unit) {init_double(value, UnitType{unit});}
    Step(double value, long unit) {init_double(value, UnitType{unit});}
    Step(double value, const std::string& unit) {init_double(value, UnitType{unit});}

    Step(long value, const UnitType& unit) { init_long(value, unit);}
    Step(long value, Unit unit) {init_long(value, UnitType{unit});}
    Step(long value, long unit) {init_long(value, UnitType{unit});}
    Step(long value, const std::string& unit) {init_long(value, UnitType{unit});}

    // Getters
    template <typename T> T value() const;
    template <typename T> T value(const UnitType& unit) const;
    UnitType unit() const { return unit_; }

    // Setters
    Step& set_unit(const std::string& unit_name) {unit_ = UnitType{unit_name}; return *this;}
    Step& set_unit(long unit_code) {unit_ = UnitType{unit_code}; return *this;}
    Step& set_unit(const UnitType& new_unit) {unit_ = new_unit; return *this;}
    Step& set_unit(const Unit new_unit) {unit_ = new_unit; return *this;}

    // Operators
    bool operator==(const Step& other) const;
    bool operator!=(const Step& other) const;
    Step operator+(const Step& step) const;
    Step operator-(const Step& step) const;
    bool operator>(const Step& step) const;
    bool operator<(const Step& step) const;
    Step copy() const {return Step{internal_value_, internal_unit_};}

    // Methods
    Step& optimize_unit();
    friend std::pair<Step, Step> find_common_units(const Step& startStep, const Step& endStep);
    void hide_hour_unit() {
        internal_unit_.hide_hour_unit();
        unit_.hide_hour_unit();
    }
    void show_hour_unit() {
        internal_unit_.show_hour_unit();
        unit_.show_hour_unit();
    }

    std::string to_string() const {
        std::stringstream ss;
        if (value<long>() == value<double>()) {
            ss << value<long>() << unit_.to_string();
        } else {
            ss << value<double>() << unit_.to_string();
        }
        return ss.str();
    }

private:
    void init_long(long value, const UnitType& unit);
    void init_double(double value, const UnitType& unit);
    void sanity_check() const;
    Step& recalculateValue() {
        if (internal_value_ == 0) {
            internal_unit_ = unit_;
            return *this;
        }

        Seconds<long> seconds = to_seconds<long>(internal_value_, internal_unit_);
        long multiplier = UnitType::get_converter().unit_to_duration(unit_.to_value());
        internal_value_ = seconds.count() / multiplier;
        internal_unit_ = unit_;

        return *this;
    }

    long internal_value_;
    UnitType internal_unit_;
    UnitType unit_;
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

template <typename T> T Step::value(const UnitType& unit) const {
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
Seconds<T> to_seconds(long value, const UnitType& unit) {
    Seconds<T> seconds;
    switch (unit.to_value()) {
        case Unit::SECOND: seconds = Seconds<T>(value); break;
        case Unit::MINUTE: seconds = Minutes<T>(value); break;
        case Unit::MINUTES15: seconds = Minutes15<T>(value); break;
        case Unit::MINUTES30: seconds = Minutes30<T>(value); break;
        case Unit::HOUR: seconds = Hours<T>(value); break;
        case Unit::HOURS3: seconds = Hours3<T>(value); break;
        case Unit::HOURS6: seconds = Hours6<T>(value); break;
        case Unit::HOURS12: seconds = Hours12<T>(value); break;
        case Unit::DAY: seconds = Days<T>(value); break;
        case Unit::MONTH: seconds = Months<T>(value); break;
        case Unit::YEAR: seconds = Years<T>(value); break;
        case Unit::YEARS10: seconds = Years10<T>(value); break;
        case Unit::YEARS30: seconds = Years30<T>(value); break;
        case Unit::CENTURY: seconds = Centuries<T>(value); break;
        default:
            std::string msg = "Unknown unit: " + unit.to_string();
            throw std::runtime_error(msg);
    }
    return seconds;
}


template <typename T>
T from_seconds(Seconds<T> seconds, const UnitType& unit) {
    T value = 0;
    switch (unit.to_value()) {
        case Unit::SECOND: value = std::chrono::duration_cast<Seconds<T>>(seconds).count(); break;
        case Unit::MINUTE: value = std::chrono::duration_cast<Minutes<T>>(seconds).count(); break;
        case Unit::MINUTES15: value = std::chrono::duration_cast<Minutes15<T>>(seconds).count(); break;
        case Unit::MINUTES30: value = std::chrono::duration_cast<Minutes30<T>>(seconds).count(); break;
        case Unit::HOUR: value = std::chrono::duration_cast<Hours<T>>(seconds).count(); break;
        case Unit::HOURS3: value = std::chrono::duration_cast<Hours3<T>>(seconds).count(); break;
        case Unit::HOURS6: value = std::chrono::duration_cast<Hours6<T>>(seconds).count(); break;
        case Unit::HOURS12: value = std::chrono::duration_cast<Hours12<T>>(seconds).count(); break;
        case Unit::DAY: value = std::chrono::duration_cast<Days<T>>(seconds).count(); break;
        case Unit::MONTH: value = std::chrono::duration_cast<Months<T>>(seconds).count(); break;
        case Unit::YEAR: value = std::chrono::duration_cast<Years<T>>(seconds).count(); break;
        case Unit::YEARS10: value = std::chrono::duration_cast<Years10<T>>(seconds).count(); break;
        case Unit::YEARS30: value = std::chrono::duration_cast<Years30<T>>(seconds).count(); break;
        case Unit::CENTURY: value = std::chrono::duration_cast<Centuries<T>>(seconds).count(); break;
        default:
            std::string msg = "Unknown unit: " + unit.to_string();
            throw std::runtime_error(msg);
    }
    return value;
}

