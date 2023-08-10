#pragma once

#include <tuple>
#include <map>
#include <exception>
#include <stdexcept>
#include <string>
#include <chrono>
#include <vector>
#include <tuple>


enum Unit : long {
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

using Minutes = std::chrono::duration<long, std::ratio<60>>;
using Hours = std::chrono::duration<long, std::ratio<3600>>;
using Days = std::chrono::duration<long, std::ratio<86400>>;
using Months = std::chrono::duration<long, std::ratio<2592000>>;
using Years = std::chrono::duration<long, std::ratio<31536000>>;
using Years10 = std::chrono::duration<long, std::ratio<315360000>>;
using Years30 = std::chrono::duration<long, std::ratio<946080000>>;
using Centuries = std::chrono::duration<long, std::ratio<3153600000>>;
using Hours3 = std::chrono::duration<long, std::ratio<10800>>;
using Hours6 = std::chrono::duration<long, std::ratio<21600>>;
using Hours12 = std::chrono::duration<long, std::ratio<43200>>;
using Seconds = std::chrono::duration<long, std::ratio<1>>;
using Minutes15 = std::chrono::duration<long, std::ratio<900>>;
using Minutes30 = std::chrono::duration<long, std::ratio<1800>>;
using Missing = std::chrono::duration<long, std::ratio<0>>;

using MinutesDouble = std::chrono::duration<double, std::ratio<60>>;
using HoursDouble = std::chrono::duration<double, std::ratio<3600>>;
using DaysDouble = std::chrono::duration<double, std::ratio<86400>>;
using MonthsDouble = std::chrono::duration<double, std::ratio<2592000>>;
using YearsDouble = std::chrono::duration<double, std::ratio<31536000>>;
using Years10Double = std::chrono::duration<double, std::ratio<315360000>>;
using Years30Double = std::chrono::duration<double, std::ratio<946080000>>;
using CenturiesDouble = std::chrono::duration<double, std::ratio<3153600000>>;
using Hours3Double = std::chrono::duration<double, std::ratio<10800>>;
using Hours6Double = std::chrono::duration<double, std::ratio<21600>>;
using Hours12Double = std::chrono::duration<double, std::ratio<43200>>;
using SecondsDouble = std::chrono::duration<double, std::ratio<1>>;
using Minutes15Double = std::chrono::duration<double, std::ratio<900>>;
using Minutes30Double = std::chrono::duration<double, std::ratio<1800>>;
using MissingDouble = std::chrono::duration<double, std::ratio<0>>;

class StepUnitsTable{
private:
    StepUnitsTable() {
    }
public:
    static Unit to_unit(const std::string& str) {
        static std::map<std::string, Unit> map = {
            {"m", Unit::MINUTE},
            {"h", Unit::HOUR},
            {"d", Unit::DAY},
            {"M", Unit::MONTH},
            {"Y", Unit::YEAR},
            {"10Y", Unit::YEARS10},
            {"30Y", Unit::YEARS30},
            {"C", Unit::CENTURY},
            {"3h", Unit::HOURS3},
            {"6h", Unit::HOURS6},
            {"12h", Unit::HOURS12},
            {"s", Unit::SECOND},
            {"15m", Unit::MINUTES15},
            {"30m", Unit::MINUTES30},
            {"255", Unit::MISSING},
        };
        return map[str];
    }

    static long to_long(const std::string& str) {
        return to_unit(str);
    }

    static Unit to_unit(long code) {
        static std::map<long, Unit> map = {
            {Unit::MINUTE, Unit::MINUTE},
            {Unit::HOUR, Unit::HOUR},
            {Unit::DAY, Unit::DAY},
            {Unit::MONTH, Unit::MONTH},
            {Unit::YEAR, Unit::YEAR},
            {Unit::YEARS10, Unit::YEARS10},
            {Unit::YEARS30, Unit::YEARS30},
            {Unit::CENTURY, Unit::CENTURY},
            {Unit::HOURS3, Unit::HOURS3},
            {Unit::HOURS6, Unit::HOURS6},
            {Unit::HOURS12, Unit::HOURS12},
            {Unit::SECOND, Unit::SECOND},
            {Unit::MINUTES15, Unit::MINUTES15},
            {Unit::MINUTES30, Unit::MINUTES30},
            {Unit::MISSING, Unit::MISSING},
        };
        return map[code];
    }

    static std::string to_str(long code) {
        static std::map<long, std::string> map = {
            {Unit::MINUTE, "m"},
            {Unit::HOUR, "h"},
            {Unit::DAY, "d"},
            {Unit::MONTH, "M"},
            {Unit::YEAR, "Y"},
            {Unit::YEARS10, "10Y"},
            {Unit::YEARS30, "30Y"},
            {Unit::CENTURY, "C"},
            {Unit::HOURS3, "3h"},
            {Unit::HOURS6, "6h"},
            {Unit::HOURS12, "12h"},
            {Unit::SECOND, "s"},
            {Unit::MINUTES15, "15m"},
            {Unit::MINUTES30, "30m"},
            {Unit::MISSING, "255"},
        };
        return map[code];
    }

    static double unit_duration(long code) {
        static std::map<long, double> map = {
            {Unit::MINUTE, Minutes::period::num / Minutes::period::den},
            {Unit::HOUR, Hours::period::num / Hours::period::den},
            {Unit::DAY, Days::period::num / Days::period::den},
            {Unit::MONTH, Months::period::num / Months::period::den},
            {Unit::YEAR, Years::period::num / Years::period::den},
            {Unit::YEARS10, Years10::period::num / Years10::period::den},
            {Unit::YEARS30, Years30::period::num / Years30::period::den},
            {Unit::CENTURY, Centuries::period::num / Centuries::period::den},
            {Unit::HOURS3, Hours3::period::num / Hours3::period::den},
            {Unit::HOURS6, Hours6::period::num / Hours6::period::den},
            {Unit::HOURS12, Hours12::period::num / Hours12::period::den},
            {Unit::SECOND, Seconds::period::num / Seconds::period::den},
            {Unit::MINUTES15, Minutes15::period::num / Minutes15::period::den},
            {Unit::MINUTES30, Minutes30::period::num / Minutes30::period::den},
        };
        return map[code];
    }
};


class Step {
public:
    Step() : value_(0), unit_(Unit::SECOND) {}
    Step(int value, long unit);
    explicit Step(const std::string& str);

    int value() const { return value_; }
    std::string unit_as_str() const;
    long unit_as_long() const;
    void hide_hour_unit() { hide_hour_unit_ = true; }

    Step& optimizeUnit();
    Step& setUnit(std::string new_unit);
    Step& setUnit(long new_unit);
    double getDoubleValue(long unit) const;
    bool operator==(const Step& other) const;
    friend std::pair<Step, Step> findCommonUnits(Step startStep, Step endStep);
    friend Step operator+(const Step step1, const Step step2);

private:
    long unit() const { return unit_; }
    Step& setUnit(Unit new_unit);
    Step(int value, Unit unit);
    std::vector<std::pair<Unit, long>> unitMap_ = {
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
    int value_;
    Unit unit_;
    Unit default_unit_ = Unit::HOUR;
    bool hide_hour_unit_ = false;
};

std::pair<Step, Step> findCommonUnits(Step, Step);
std::string parse_step(std::string step);
std::vector<Step> parse_range(const std::string& range_str);
