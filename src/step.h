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


class UnitType {
public:
    explicit UnitType() : internal_value_(Unit::HOUR) {}
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

    std::string toString() const {
        if ((internal_value_ == Unit::HOUR) && hide_hour_unit_) {
            return "";
        }
        else {
            return map_.unit_to_name(internal_value_);
        }
    }
    long toLong() const {return map_.unit_to_long(internal_value_);}
    Unit toValue() const {return internal_value_;}
    void hideHourUnit() {hide_hour_unit_ = true;}
    void showHourUnit() {hide_hour_unit_ = false;}
    static std::vector<Unit> unitOrder;

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
    static Map& getConverter() {return map_;}
};


class Step {
public:
    // Constructors
    Step() : internal_value_(0), internal_unit_(Unit::SECOND) {}
    Step(double value, const UnitType& unit);
    Step(double value, Unit unit);
    Step(double value, long unit);
    Step(double value, const std::string& unit);

    Step(long value, const UnitType& unit);
    Step(long value, Unit unit);
    Step(long value, long unit);
    Step(long value, const std::string& unit);
    explicit Step(const std::string& str);

    // Getters
    template <typename T> T value() const;
    UnitType unit() const { return unit_; }

    // Setters
    Step& setUnit(long new_unit);
    Step& setUnit(const std::string& new_unit);
    Step& setUnit(const Unit new_unit);
    Step& setUnit(const UnitType& new_unit);

    // Operators
    bool operator==(const Step& other) const;
    bool operator!=(const Step& other) const;
    Step operator+(const Step& step);
    Step operator-(const Step& step);

    // Methods
    Step& optimizeUnit();
    friend std::pair<Step, Step> findCommonUnits(const Step& startStep, const Step& endStep);
    void hideHourUnit() {
        internal_unit_.hideHourUnit(); 
        unit_.hideHourUnit();
    }
    void showHourUnit() {
        internal_unit_.showHourUnit(); 
        unit_.showHourUnit();
    }

    std::string toString() const {
        std::stringstream ss;
        if (value<long>() == value<double>()) {
            ss << value<long>() << unit_.toString();
        } else {
            ss << value<double>() << unit_.toString();
        }
        return ss.str();
    }

private:
    void initLong(long value, const UnitType& unit);
    void initDouble(double value, const UnitType& unit);
    void sanityCheck() const;
    Step& recalculateValue() {
        if (internal_value_ == 0) {
            internal_unit_ = unit_;
            return *this;
        }

        Seconds<long> secs(0);
        switch (internal_unit_.toValue()) {
            case Unit::SECOND:
                secs = Seconds<long>(internal_value_);
                break;
            case Unit::MINUTE:
                secs = Minutes<long>(internal_value_);
                break;
            case Unit::HOUR:
                secs = Hours<long>(internal_value_);
                break;
            case Unit::DAY:
                secs = Days<long>(internal_value_);
                break;
            case Unit::MONTH:
                secs = Months<long>(internal_value_);
                break;
            default:
                std::string msg = "Unknown unit: " + internal_unit_.toString();
                throw std::runtime_error(msg);
        }

        long multiplier = UnitType::getConverter().unit_to_duration(unit_.toValue());
        internal_value_ = secs.count() / multiplier;
        internal_unit_ = unit_;

        return *this;
    }

    long internal_value_;
    UnitType internal_unit_;
    UnitType unit_;
};


Step step_from_string(std::string step);
std::vector<Step> parseRange(const std::string& range_str);
std::pair<Step, Step> findCommonUnits(const Step& startStep, const Step& endStep);


template <typename T> T Step::value() const {
    if (internal_value_ == 0) {
        return internal_value_;
    }
    if (internal_unit_ == unit_) {
        return internal_value_;
    }
    Seconds<T> duration(0);
    switch (internal_unit_.toValue()) {
        case Unit::SECOND:
            duration = Seconds<T>(internal_value_);
            break;
        case Unit::MINUTE:
            duration = Minutes<T>(internal_value_);
            break;
        case Unit::HOUR:
            duration = Hours<T>(internal_value_);
            break;
        case Unit::DAY:
            duration = Days<T>(internal_value_);
            break;
        case Unit::MONTH:
            duration = Months<T>(internal_value_);
            break;
        default:
            std::string msg = "Unknown unit: " + internal_unit_.toString();
            throw std::runtime_error(msg);
    }

    T value = 0;
    switch (unit_.toValue()) {
        case Unit::SECOND:
            value = duration.count();
            break;
        case Unit::MINUTE:
            value = std::chrono::duration_cast<Minutes<T>>(duration).count();
            break;
        case Unit::HOUR:
            value = std::chrono::duration_cast<Hours<T>>(duration).count();
            break;
        case Unit::DAY:
            value = std::chrono::duration_cast<Days<T>>(duration).count();
            break;
        case Unit::MONTH:
            value = std::chrono::duration_cast<Months<T>>(duration).count();
            break;
        default:
            std::string msg = "Unknown unit: " + UnitType{unit_}.toString();
            throw std::runtime_error(msg);
    }
    return value;
}

