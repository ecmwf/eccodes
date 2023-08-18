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


template <typename T>
class Step {
public:
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


        explicit Unit() : value_(Value::HOUR) {}
        explicit Unit(Value unit_value) : value_(unit_value) {}

        explicit Unit(const std::string& unit_value) {
            value_ = map_.name_to_unit(unit_value);
        }

        explicit Unit(long unit_value) {
            value_ = map_.long_to_unit(unit_value);
        }

        bool operator>(const Unit& other) const {return map_.unit_to_duration(value_) > map_.unit_to_duration(other.value_);}
        bool operator==(const Value value) const {return map_.unit_to_duration(value_) == map_.unit_to_duration(value);}
        bool operator==(const Unit& unit) const {return map_.unit_to_duration(value_) == map_.unit_to_duration(unit.value_);}
        Unit& operator=(const Value value) {
            value_ = value;
            return *this;
        }

        std::string to_string() const {
            if ((value_ == Value::HOUR) && hide_hour_unit_) {
                return "";
            }
            else {
                return map_.unit_to_name(value_);
            }
        }
        long to_long() const {return map_.unit_to_long(value_);}
        Value to_value() const {return value_;}
        void hide_hour_unit() {
            hide_hour_unit_ = true;
        }

        static std::vector<Value> unitOrder;

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
            std::string unit_to_name(const Value& unit_value) const {return value_to_name_.at(unit_value);}
            Value name_to_unit(const std::string& name) const {return name_to_value_.at(name);}

            // unit_value <-> duration
            long unit_to_duration(const Value& unit_value) const {return value_to_duration_.at(unit_value);}
            Value duration_to_unit(long duration) const {return duration_to_value_.at(duration);}

            // wmo_code <-> unit_name
            long unit_to_long(const Value& unit_value) const {return value_to_long_.at(unit_value);}
            Value long_to_unit(long wmo_code) const {return long_to_value_.at(wmo_code);}

        private:
            struct Entry {
                Unit::Value unit_value;
                std::string unit_name;
                long duration;
            };

            const std::array<Entry, 15> tab_ = {
                Entry{Value::MISSING   , "MISSING" , 0},
                Entry{Value::SECOND    , "s"       , 1},
                Entry{Value::MINUTE    , "m"       , 60},
                Entry{Value::MINUTES15 , "15m"     , 900},
                Entry{Value::MINUTES30 , "30m"     , 1800},
                Entry{Value::HOUR      , "h"       , 3600},
                Entry{Value::HOURS3    , "3h"      , 10800},
                Entry{Value::HOURS6    , "6h"      , 21600},
                Entry{Value::HOURS12   , "12h"     , 43200},
                Entry{Value::DAY       , "D"       , 86400},
                Entry{Value::MONTH     , "M"       , 2592000},
                Entry{Value::YEAR      , "Y"       , 31536000},
                Entry{Value::YEARS10   , "10Y"     , 315360000},
                Entry{Value::YEARS30   , "30Y"     , 946080000},
                Entry{Value::CENTURY   , "C"       , 3153600000},
            };

            std::unordered_map<std::string, Value> name_to_value_;
            std::unordered_map<Value, std::string> value_to_name_;

            std::unordered_map<Value, long> value_to_long_;
            std::unordered_map<long, Value> long_to_value_;

            std::unordered_map<Value, long> value_to_duration_;
            std::unordered_map<long, Value> duration_to_value_;
        };


        Value value_;
        static Map map_;
    public:
        static Map& get_converter() {return map_;}
    };

    // Constructors
    Step() : value_(0), unit_(Unit::Value::SECOND) {}
    Step(T value, const Unit& unit);
    Step(T value, long unit);
    Step(T value, const std::string& unit);
    explicit Step(const std::string& str);

    // Getters
    T value() const { return value_; }
    Unit unit() const { return unit_; }

    // Setters
    Step<T>& setUnit(long new_unit);
    Step<T>& setUnit(const std::string& new_unit);
    Step<T>& setUnit(const Step<T>::Unit& new_unit);
    Step<T>& setUnit(const typename Step<T>::Unit::Value new_unit);

    // Operators
    bool operator==(const Step<T>& other) const;
    Step<T> operator+(const Step<T>& step);

    // Methods
    Step<T>& optimizeUnit();
    friend std::pair<Step<long>, Step<long>> findCommonUnits(const Step<long>& startStep, const Step<long>& endStep);
    void hide_hour_unit() {unit_.hide_hour_unit();}

private:
    T value_;
    Unit unit_;
};


template <typename T>
//typename Step<T>::Unit::Map Step<T>::Unit::map_ = Step<T>::Unit::Map();
typename Step<T>::Unit::Map Step<T>::Unit::map_{};


template <typename T>
std::vector<typename Step<T>::Unit::Value> Step<T>::Unit::unitOrder = {
            Value::SECOND,
            Value::MINUTE,
            //{StepValue::MINUTES15, 900},
            //{StepValue::MINUTES30, 1800},
            Value::HOUR,
            //{StepValue::HOURS3, 10800},
            //{StepValue::HOURS6, 21600},
            //{StepValue::HOURS12, 43200},
            //Value::DAY,
            //Value::MONTH,
            //{StepValue::YEAR, 31536000},
            //{StepValue::YEARS10, 315360000},
            //{StepValue::YEARS30, 946080000},
            //{StepValue::CENTURY, 3153600000},
        };


std::string parse_step(std::string step);

template <typename T>
bool Step<T>::operator==(const Step<T>& other) const {
    if ((value_ == other.value_) && (unit_ == other.unit_)) {
        return true;
    }
    return false;
}

template <typename T>
Step<T> Step<T>::operator+(const Step<T>& step) {
    auto [a, b] = findCommonUnits(*this, step);
    return Step(a.value_ + b.value_, a.unit_);
}

std::pair<Step<long>, Step<long>> findCommonUnits(const Step<long>& startStep, const Step<long>& endStep);


template <typename T> std::vector<Step<T>> parse_range(const std::string& range_str) {
    std::vector<Step<T>> steps;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = range_str.find("-", prev)) != std::string::npos) {
        std::string token = parse_step(range_str.substr(prev, pos - prev));
        if (token.size() > 0) {
            steps.push_back(Step<T>(token));
        }
        prev = pos + 1;
    }
    std::string token = parse_step(range_str.substr(prev));
    if (token.size() > 0) {
        steps.push_back(Step<T>(token));
    }
    return steps;
}


template <typename T>
Step<T>::Step(T value, long unit) : value_{value}, unit_{Unit{unit}} {
    static_assert(sizeof(int) == 4, "int is not 4 bytes");
    if (!(value >= 0 && value <= std::numeric_limits<int>::max())) {
        throw std::out_of_range("Step is out of range.");
    }
}

template <typename T>
Step<T>::Step(T value, const std::string& unit) : value_{value}, unit_{Unit{unit}} {
    static_assert(sizeof(int) == 4, "int is not 4 bytes");
    if (!(value >= 0 && value <= std::numeric_limits<int>::max())) {
        throw std::out_of_range("Step is out of range.");
    }
}

template <typename T>
Step<T>::Step(const std::string& str) {
    size_t pos = str.find_first_of("smhdMYC");
    if (pos == std::string::npos) {
        throw std::runtime_error("Unknown unit.");
    }
    std::string v_str = str.substr(0, pos);
    std::string u_str = str.substr(pos);
    int v = std::stoi(v_str);
    value_ = v;
    unit_ = Unit{u_str};
}

template <typename T>
Step<T>::Step(T value, const Unit& u) {
    static_assert(sizeof(int) == 4, "int is not 4 bytes");
    if (!(value >= 0 && value <= std::numeric_limits<int>::max())) {
        throw std::out_of_range("Step is out of range.");
    }
    value_ = value;
    unit_ = u;
}

template <typename T>
Step<T>& Step<T>::optimizeUnit() {
    if (value_ == 0) {
        return *this;
    }

    Seconds<T> duration(0);
    switch (unit_.to_value()) {
        case Unit::Value::SECOND:
            duration = Seconds<T>(value_);
            break;
        case Unit::Value::MINUTE:
            duration = Minutes<T>(value_);
            break;
        case Unit::Value::HOUR:
            duration = Hours<T>(value_);
            break;
        case Unit::Value::DAY:
            duration = Days<T>(value_);
            break;
        case Unit::Value::MONTH:
            duration = Months<T>(value_);
            break;
        default:
            std::string msg = "Unknown unit: " + unit_.to_string();
            throw std::runtime_error(msg);
    }

    Seconds<T> d = std::chrono::duration_cast<Seconds<T>>(duration);

    for (auto it = Unit::unitOrder.rbegin(); it != Unit::unitOrder.rend(); ++it) {
        long multiplier = Unit::get_converter().unit_to_duration(*it);
        if (d.count() % multiplier == 0) {
            value_ = duration.count() / multiplier;
            unit_ = *it;
            return *this;
        }
    }

    return *this;
}

template <typename T>
Step<T>& Step<T>::setUnit(const std::string& unit_name) {
    setUnit(Unit{unit_name}); 
    return *this;
}

template <typename T>
Step<T>& Step<T>::setUnit(long unit_code) {
    setUnit(Unit{unit_code});
    return *this;
}

template <typename T>
Step<T>& Step<T>::setUnit(const Step<T>::Unit& new_unit) {
    setUnit(new_unit.to_value());
    return *this;
}


template <typename T>
Step<T>& Step<T>::setUnit(const typename Step<T>::Unit::Value new_unit) {
    if (value_ == 0) {
        unit_ = new_unit;
        return *this;
    }
    if (unit_ == new_unit) {
        return *this;
    }
    Seconds<T> duration(0);
    switch (unit_.to_value()) {
        case Unit::Value::SECOND:
            duration = Seconds<T>(value_);
            break;
        case Unit::Value::MINUTE:
            duration = Minutes<T>(value_);
            break;
        case Unit::Value::HOUR:
            duration = Hours<T>(value_);
            break;
        case Unit::Value::DAY:
            duration = Days<T>(value_);
            break;
        case Unit::Value::MONTH:
            duration = Months<T>(value_);
            break;
        default:
            std::string msg = "Unknown unit: " + unit_.to_string();
            throw std::runtime_error(msg);
    }

    switch (new_unit) {
        case Unit::Value::SECOND:
            value_ = duration.count();
            break;
        case Unit::Value::MINUTE:
            value_ = std::chrono::duration_cast<Minutes<T>>(duration).count();
            break;
        case Unit::Value::HOUR:
            value_ = std::chrono::duration_cast<Hours<T>>(duration).count();
            break;
        case Unit::Value::DAY:
            value_ = std::chrono::duration_cast<Days<T>>(duration).count();
            break;
        case Unit::Value::MONTH:
            value_ = std::chrono::duration_cast<Months<T>>(duration).count();
            break;
        default:
            std::string msg = "Unknown unit: " + Step<T>::Unit{new_unit}.to_string();
            throw std::runtime_error(msg);
    }
    unit_ = new_unit;

    return *this;
}

