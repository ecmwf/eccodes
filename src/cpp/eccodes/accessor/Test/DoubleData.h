#pragma once

#include "../AccessorData.h"

namespace eccodes::accessor {

class DoubleData : public AccessorData
{
public:
    DoubleData(AccessorInitData const& initData);
    ~DoubleData() = default;

private:
    int nativeType() const override;

    bool pack(std::vector<double> const& values) override;
    bool unpack(std::vector<double> &values) const override;
};

}
