#ifndef _STRING_DATA_H_20230818_
#define _STRING_DATA_H_20230818_

#include "AccessorData.h"

namespace eccodes::accessor {

class StringData : public AccessorData
{
public:
    StringData(AccessorInitData const& initData);
    ~StringData() = default;

private:
    int nativeType() const override;

    bool pack(std::vector<char> const& values) override;
    bool unpack(std::vector<char> &values) const override;
};

}

#endif // _STRING_DATA_H_20230818_
