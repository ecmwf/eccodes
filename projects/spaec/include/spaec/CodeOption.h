/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdint>
#include <stdexcept>
#include <cassert>

namespace spaec
{

enum class CodeOptionSet
{
    Basic,
    Restricted
};

using IdType = uint16_t;

class CodeOptionBase
{
public:
    CodeOptionBase(uint8_t bpv, CodeOptionSet optionSet)
    {
        // An exception like the other code options (an assert aborted in debug builds, and the
        // basic set accepted any bits per value in release builds)
        if (bpv < 1 || bpv > 32) {
            throw std::invalid_argument("Unsupported BPV or option set");
        }

        // bpv is 1..32 here
        maxIdLength = [&] {
            if (bpv > 16)
                return 6;
            if (bpv > 8)
                return 5;
            if (optionSet == CodeOptionSet::Basic || bpv > 4)
                return 4;
            return bpv >= 3 ? 3 : 2;  // restricted set
        }();
    }

    uint8_t maxIdLength;
};


class ZeroBlockCodeOption
{
public:
    ZeroBlockCodeOption(uint8_t bpv, CodeOptionSet cos)
    {
        length = [&] {
            if (bpv > 16 && bpv <= 32)
                return 6;
            else if (bpv > 8 && bpv <= 16)
                return 5;
            else {
                if (cos == CodeOptionSet::Basic) {
                    if (bpv >= 1 && bpv <= 8)
                        return 4;
                }
                else if (cos == CodeOptionSet::Restricted) {
                    if (bpv > 4 && bpv <= 8)
                        return 4;
                    else if (bpv == 3 || bpv == 4)
                        return 3;
                    else if (bpv == 1 || bpv == 2)
                        return 2;
                }
            }

            throw std::invalid_argument("Unsupported BPV or option set");
        }();
    }

    static constexpr IdType value = 0x00;
    uint8_t length;
    static constexpr IdType valueShifted = 0x00;
};


class SecondExtensionCodeOption
{
public:
    SecondExtensionCodeOption(uint8_t bpv, CodeOptionSet cos)
    {
        length = [&] {
            if (bpv > 16 && bpv <= 32) {
                return 6;
            }
            else if (bpv > 8 && bpv <= 16) {
                return 5;
            }
            else {
                if (cos == CodeOptionSet::Basic) {
                    if (bpv >= 1 && bpv <= 8)
                        return 4;
                }
                else if (cos == CodeOptionSet::Restricted) {
                    if (bpv > 4 && bpv <= 8)
                        return 4;
                    else if (bpv == 3 || bpv == 4)
                        return 3;
                    else if (bpv == 1 || bpv == 2)
                        return 2;
                }
            }
            throw std::invalid_argument("Unsupported bpv or option set");
        }();

        valueShifted = value << (sizeof(IdType) * 8 - length);
    }


    static constexpr IdType value = 0x01;
    uint8_t length;
    IdType valueShifted;
};


class SplitSampleLimits
{
public:
    SplitSampleLimits(uint8_t bpv, CodeOptionSet cos)
    {
        kMin_ = 0;

        kMax_ = [&]() {
            if (bpv > 16 && bpv <= 32) {
                return 29;
            }
            else if (bpv > 8 && bpv <= 16) {
                return 13;
            }
            else {
                if (cos == CodeOptionSet::Basic) {
                    if (bpv >= 1 && bpv <= 8) {
                        return 5;
                    }
                }
                else if (cos == CodeOptionSet::Restricted) {
                    if (bpv > 4 && bpv <= 8)
                        return 5;
                    else if (bpv == 3 || bpv == 4)
                        return 1;
                    else if (bpv == 1 || bpv == 2) {
                        // CCSDS 121.0-B-3, Table 5-1: no FS and no split-sample options
                        available_ = false;
                        return 0;
                    }
                }
            }
            throw std::invalid_argument("Unsupported bpv or option set");
        }();
    }

public:
    uint8_t kMin_;
    uint8_t kMax_;
    bool available_ = true;  // false: the option set has no FS and split-sample options
};

class SplitSampleIdLength
{
public:
    SplitSampleIdLength(uint8_t bpv, CodeOptionSet cos)
    {
        length = [&]() {
            if (bpv > 16 && bpv <= 32) {
                return 5;
            }
            else if (bpv > 8 && bpv <= 16) {
                return 4;
            }
            else {
                if (cos == CodeOptionSet::Basic) {
                    if (bpv >= 1 && bpv <= 8) {
                        return 3;
                    }
                }
                else if (cos == CodeOptionSet::Restricted) {
                    if (bpv > 4 && bpv <= 8)
                        return 3;
                    else if (bpv == 3 || bpv == 4)
                        return 2;
                    else if (bpv == 1 || bpv == 2)
                        return 1;  // the ID length of the set (no compression is "1"), no split-sample IDs
                }
            }
            throw std::invalid_argument("Unsupported bpv or option set");
        }();
    };

    uint8_t length;
};

class SplitSampleCodeOption : public SplitSampleIdLength
{
public:
    SplitSampleCodeOption(uint8_t bpv, uint8_t k, CodeOptionSet cos) : SplitSampleIdLength(bpv, cos), value(k + 1)
    {
        if (cos == CodeOptionSet::Restricted && bpv <= 2) {
            throw std::invalid_argument("Split option not supported for bpv 1 or 2 in restricted option set");
        }
        valueShifted = value << (sizeof(IdType) * 8 - length);
    }
    IdType value;
    IdType valueShifted;
};

class NoCompressionCodeOption
{
public:
    NoCompressionCodeOption(uint8_t bpv, CodeOptionSet cos)
    {
        length = [&] {
            if (bpv > 16 && bpv <= 32) {
                return 5;
            }
            else if (bpv > 8 && bpv <= 16) {
                return 4;
            }
            else {
                if (cos == CodeOptionSet::Basic) {
                    if (bpv >= 1 && bpv <= 8)
                        return 3;
                }
                else if (cos == CodeOptionSet::Restricted) {
                    if (bpv > 4 && bpv <= 8)
                        return 3;
                    else if (bpv == 3 || bpv == 4)
                        return 2;
                    else if (bpv == 1 || bpv == 2)
                        return 1;
                }
            }
            throw std::invalid_argument("Unsupported bpv or option set");
        }();

        // The length above has rejected unsupported bits per value: bpv is 1..32 here
        value = [&]() {
            if (bpv > 16)
                return 0b11111;
            if (bpv > 8)
                return 0b1111;
            if (cos == CodeOptionSet::Basic || bpv > 4)
                return 0b111;
            return bpv >= 3 ? 0b11 : 0b1;  // restricted set
        }();

        valueShifted = value << (sizeof(IdType) * 8 - length);
    }

    uint8_t length;
    IdType value;
    IdType valueShifted;
};

}  // namespace spaec
