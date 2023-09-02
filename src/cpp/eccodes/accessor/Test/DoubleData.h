#pragma once

#include "AccessorData/AccessorData.h"

namespace eccodes::accessor {

class DoubleData : public AccessorData
{
public:
    DoubleData(AccessorInitData const& initData);
    ~DoubleData() = default;

private:
    GribType nativeType() const override;

    GribStatus pack(std::vector<double> const& values) override;
    GribStatus unpack(std::vector<double> &values) const override;
};

}
