/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <stdexcept>
#include <array>
#include <algorithm>

namespace eccodes {

template <typename T> using Minutes   = std::chrono::duration<T, std::ratio<60>>;
template <typename T> using Hours     = std::chrono::duration<T, std::ratio<3600>>;
template <typename T> using Days      = std::chrono::duration<T, std::ratio<86400>>;
template <typename T> using Months    = std::chrono::duration<T, std::ratio<2592000>>;
template <typename T> using Years     = std::chrono::duration<T, std::ratio<31536000>>;
template <typename T> using Years10   = std::chrono::duration<T, std::ratio<315360000>>;
template <typename T> using Years30   = std::chrono::duration<T, std::ratio<946080000>>;
template <typename T> using Centuries = std::chrono::duration<T, std::ratio<3153600000>>;
template <typename T> using Hours3    = std::chrono::duration<T, std::ratio<10800>>;
template <typename T> using Hours6    = std::chrono::duration<T, std::ratio<21600>>;
template <typename T> using Hours12   = std::chrono::duration<T, std::ratio<43200>>;
template <typename T> using Seconds   = std::chrono::duration<T, std::ratio<1>>;
template <typename T> using Minutes15 = std::chrono::duration<T, std::ratio<900>>;
template <typename T> using Minutes30 = std::chrono::duration<T, std::ratio<1800>>;
template <typename T> using Missing   = std::chrono::duration<T, std::ratio<0>>;



class Unit;
template <typename T> Seconds<T> to_seconds(long value, const Unit& unit);
template <typename T> T from_seconds(Seconds<T> seconds, const Unit& unit);

class Unit {
public:
    enum class Value {
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

    Unit() : internal_value_(Value::HOUR) {}

    explicit Unit(Value unit_value) : internal_value_(unit_value) {}

    explicit Unit(const std::string& unit_value) {
        try {
            internal_value_ = get_converter().name_to_unit(unit_value);
        } catch (std::exception& e) {
            throw std::runtime_error(std::string{"Unit not found "} + e.what());
        }
    }

    explicit Unit(long unit_value) {
        try {
            internal_value_ = get_converter().long_to_unit(unit_value);
        } catch (std::exception& e) {
            throw std::runtime_error(std::string{"Unit not found "} + e.what());
        }
    }

    bool operator>(const Unit& other) const {return get_converter().unit_to_duration(internal_value_) > get_converter().unit_to_duration(other.internal_value_);}
    bool operator==(const Value value) const {return get_converter().unit_to_duration(internal_value_) == get_converter().unit_to_duration(value);}
    bool operator==(const Unit& unit) const {return get_converter().unit_to_duration(internal_value_) == get_converter().unit_to_duration(unit.internal_value_);}
    bool operator!=(const Unit& unit) const {return !(*this == unit);}
    bool operator!=(const Value value) const {return !(*this == value);}

    Unit& operator=(const Value value) {
        internal_value_ = value;
        return *this;
    }

    template <typename T> T value() const;
    static const std::array<Value, 3> grib_selected_units;
    static const std::array<Value, 15> complete_unit_order_;

    static std::vector<Unit> list_supported_units() {
        std::vector<Unit> result;
        result.reserve(32);
        for (const auto& val : complete_unit_order_) {
            if (val == Value::MISSING)
                continue;
            result.push_back(Unit(val));
        }

        return result;
    }

private:
    // Trivially destructible lookup table — no heap allocations, so no
    // data-race during static destruction when OpenMP threads are still alive.
    class Map {
    public:
        struct Entry {
            Value unit_value;
            const char* unit_name;
            uint64_t duration;
        };

        static constexpr std::array<Entry, 15> tab_ = {{
            {Value::MISSING   , "MISSING" , 0},
            {Value::SECOND    , "s"       , 1},
            {Value::MINUTE    , "m"       , 60},
            {Value::MINUTES15 , "15m"     , 900},
            {Value::MINUTES30 , "30m"     , 1800},
            {Value::HOUR      , "h"       , 3600},
            {Value::HOURS3    , "3h"      , 10800},
            {Value::HOURS6    , "6h"      , 21600},
            {Value::HOURS12   , "12h"     , 43200},
            {Value::DAY       , "D"       , 86400},
            {Value::MONTH     , "M"       , 2592000},
            {Value::YEAR      , "Y"       , 31536000},
            {Value::YEARS10   , "10Y"     , 315360000},
            {Value::YEARS30   , "30Y"     , 946080000},
            {Value::CENTURY   , "C"       , 3153600000},
        }};

        std::string unit_to_name(const Value& v) const {
            for (const auto& e : tab_) if (e.unit_value == v) return e.unit_name;
            throw std::out_of_range("unit_to_name: unknown unit");
        }
        Value name_to_unit(const std::string& name) const {
            for (const auto& e : tab_) if (name == e.unit_name) return e.unit_value;
            throw std::out_of_range("name_to_unit: unknown name");
        }

        uint64_t unit_to_duration(const Value& v) const {
            for (const auto& e : tab_) if (e.unit_value == v) return e.duration;
            throw std::out_of_range("unit_to_duration: unknown unit");
        }
        Value duration_to_unit(long duration) const {
            for (const auto& e : tab_) if (e.duration == static_cast<uint64_t>(duration)) return e.unit_value;
            throw std::out_of_range("duration_to_unit: unknown duration");
        }

        long unit_to_long(const Value& v) const {
            for (const auto& e : tab_) if (e.unit_value == v) return static_cast<long>(e.unit_value);
            throw std::out_of_range("unit_to_long: unknown unit");
        }
        Value long_to_unit(long wmo_code) const {
            for (const auto& e : tab_) if (static_cast<long>(e.unit_value) == wmo_code) return e.unit_value;
            throw std::out_of_range("long_to_unit: unknown code");
        }
    };


    Value internal_value_;
public:
    static Map& get_converter() {
        static Map map_;
        return map_;
    }
};


template <typename T>
Seconds<T> to_seconds(long value, const Unit& unit) {
    Seconds<T> seconds;
    switch (unit.value<Unit::Value>()) {
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
            std::string msg = "Unknown unit: " + unit.value<std::string>();
            throw std::runtime_error(msg);
    }
    return seconds;
}


template <typename T>
T from_seconds(Seconds<T> seconds, const Unit& unit) {
    T value;
    switch (unit.value<Unit::Value>()) {
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
            std::string msg = "Unknown unit: " + unit.value<std::string>();
            throw std::runtime_error(msg);
    }
    return value;
}

} // namespace eccodes
