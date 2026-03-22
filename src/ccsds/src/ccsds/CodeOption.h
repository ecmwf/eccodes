#pragma once

#include <cstdint>
#include <stdexcept>
#include <cassert>

enum class CodeOptionName{
    ZeroBlock,
    SecondExtension,
    NoCompression,
    SplitSample
};

enum class CodeOptionSet {
    Basic,
    Restricted
};

using IdType = uint16_t;

class CodeOptionBase {

    public:

    CodeOptionBase(uint8_t bpv, CodeOptionSet optionSet) {
        assert(bpv >= 1 && bpv <= 32 && "BPV must be between 1 and 32");

        maxIdLength = [&]{
            if (optionSet == CodeOptionSet::Basic) {
                if (bpv > 16 && bpv <= 32)
                    return 6;
                else if (bpv > 8 && bpv <= 16)
                    return 5;
                else
                    return 4;
            }
            else {
                if (bpv > 16 && bpv <= 32)
                    return 6;
                else if (bpv > 8 && bpv <= 16)
                    return 5;
                else {
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

    uint8_t maxIdLength;
};


class ZeroBlockCodeOption
{
    public:
        ZeroBlockCodeOption(uint8_t bpv, CodeOptionSet cos) {
            length = [&]{
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
        SecondExtensionCodeOption(uint8_t bpv, CodeOptionSet cos) {
            length = [&]{
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


class SplitSampleLimits {
    public:
        SplitSampleLimits(uint8_t bpv, CodeOptionSet cos) {

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
                        else if (bpv == 1 || bpv == 2)
                            throw std::invalid_argument("Split option not supported for bpv 1 or 2 in restricted option set");
                    }
                }
                throw std::invalid_argument("Unsupported bpv or option set");
            }();
        }

    // static_assert(kMin_ <= kMax_, "Invalid K range calculation");

    public:
    std::pair<uint8_t, uint8_t> getKRange() {
        return {kMin_, kMax_};
    }

    uint8_t kMin_;
    uint8_t kMax_;
};

class SplitSampleIdLength
{
    // static_assert(K >= SplitSampleLimits<BPV, COS>::kMin_ && K <= SplitSampleLimits<BPV, COS>::kMax_, "K value out of range for given BPV and option set");

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
                            throw std::invalid_argument("Split option not supported for bpv 1 or 2 in restricted option set");
                    }
                }
                throw std::invalid_argument("Unsupported bpv or option set");
            }();
        };

        uint8_t length;
};

template<typename ValueType>
class SplitSampleCodeOption : public SplitSampleIdLength
{
    public:
        SplitSampleCodeOption(uint8_t bpv, uint8_t k, CodeOptionSet cos) : SplitSampleIdLength(bpv, cos), value(k + 1) {
            valueShifted = value << (sizeof(IdType) * 8 - length);
            mask = (ValueType(1) << k) - 1;
        }
        IdType value;
        IdType valueShifted;
        ValueType mask;
};

class NoCompressionCodeOption
{
    public:
        NoCompressionCodeOption(uint8_t bpv, CodeOptionSet cos)
        {
            length = [&]{
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

            value = [&]() {
                if (bpv > 16 && bpv <= 32) {
                    return 0b11111;
                }
                else if (bpv > 8 && bpv <= 16) {
                    return 0b1111;
                }
                else {
                    if (cos == CodeOptionSet::Basic) {
                        if (bpv >= 1 && bpv <= 8)
                            return 0b111;
                    }
                    else if (cos == CodeOptionSet::Restricted) {
                        if (bpv > 4 && bpv <= 8)
                            return 0b111;
                        else if (bpv == 3 || bpv == 4)
                            return 0b11;
                        else if (bpv == 1 || bpv == 2)
                            return 0b1;
                    }
                }
                throw std::invalid_argument("Unsupported bpv or option set");
            }();

            valueShifted = value << (sizeof(IdType) * 8 - length);
        }

        uint8_t length;
        IdType value;
        IdType valueShifted;
};
