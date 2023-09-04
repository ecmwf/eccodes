#pragma once

#include "LongData.h"

namespace eccodes::accessor {

class DateData : public LongData
{
public:
    DateData(AccessorInitData const& initData);
    ~DateData() = default;

private:
    GribStatus pack(std::string const& values) override;
    GribStatus unpack(std::string &values) const override;
};

}

