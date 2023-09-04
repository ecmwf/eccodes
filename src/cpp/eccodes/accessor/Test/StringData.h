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

    GribStatus pack(std::string const& values) override;
    GribStatus unpack(std::string &values) const override;
};

}
