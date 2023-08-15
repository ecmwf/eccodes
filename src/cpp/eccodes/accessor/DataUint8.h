#ifndef _DATA_UINT8_H_20230804_
#define _DATA_UINT8_H_20230804_

#include "AccessorData.h"

namespace eccodes::accessor {

class DataUint8 : public AccessorData
{
public:
    DataUint8(const long len, grib_arguments* arg);
    ~DataUint8() = default;

private:
    int nativeTypeImpl() const override;

    int packImpl(std::vector<long> const& values) override;
    int unpackImpl(std::vector<long> &values) const override;
};

}

#endif // _DATA_UINT8_H_20230804_
