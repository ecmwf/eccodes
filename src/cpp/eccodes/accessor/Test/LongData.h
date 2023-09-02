#pragma once

#include "AccessorData/AccessorData.h"

namespace eccodes::accessor {

class LongData : public AccessorData
{
public:
    LongData(AccessorInitData const& initData);
    ~LongData() = default;

private:
    GribType nativeType() const override;

    GribStatus pack(std::vector<long> const& values) override;
    GribStatus unpack(std::vector<long> &values) const override;
};

}
