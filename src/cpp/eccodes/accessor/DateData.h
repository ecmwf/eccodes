#ifndef _DATE_DATA_H_20230818_
#define _DATE_DATA_H_20230818_

#include "LongData.h"

namespace eccodes::accessor {

class DateData : public LongData
{
public:
    DateData(AccessorInitData const& initData);
    ~DateData() = default;

private:
    bool pack(std::vector<char> const& values) override;
    bool unpack(std::vector<char> &values) const override;
};

}

#endif // _DATE_DATA_H_20230818_
