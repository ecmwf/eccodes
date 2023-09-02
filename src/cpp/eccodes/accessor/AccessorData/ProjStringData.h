#pragma once

#include "AccessorData.h"
#include "AccessorDefs.h"

namespace eccodes::accessor {

class ProjStringData : public AccessorData
{
public:
    ProjStringData(AccessorInitData const& initData);
    ~ProjStringData() = default;

private:
    GribType nativeType() const override;
    GribStatus unpack(std::vector<char> &values) const override;

    AccessorName gridType_{""};
    int endpoint_;
};

}

