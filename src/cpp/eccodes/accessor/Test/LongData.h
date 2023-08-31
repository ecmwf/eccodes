#pragma once

#include "../AccessorData.h"

namespace eccodes::accessor {

class LongData : public AccessorData
{
public:
    LongData(AccessorInitData const& initData);
    ~LongData() = default;

private:
    int nativeType() const override;

    bool pack(std::vector<long> const& values) override;
    bool unpack(std::vector<long> &values) const override;
};

}
