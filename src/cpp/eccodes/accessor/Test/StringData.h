#pragma once

#include "AccessorData/AccessorData.h"

namespace eccodes::accessor {

class StringData : public AccessorData
{
public:
    StringData(AccessorInitData const& initData);
    ~StringData() = default;

private:
    GribType nativeType() const override;

    GribStatus pack(std::vector<char> const& values) override;
    GribStatus unpack(std::vector<char> &values) const override;
};

}
