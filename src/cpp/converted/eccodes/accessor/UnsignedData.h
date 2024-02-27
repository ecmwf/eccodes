/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include "LongData.h"


namespace eccodes::accessor
{

// Forward declarations

class UnsignedData : public LongData
{
public:
    UnsignedData(const AccessorInitData& initData);
    ~UnsignedData();


protected:
    // Virtual member functions
    void dump() const override;
    GribStatus unpack(std::vector<long>& longValues) const override;
    GribStatus pack(const std::vector<long>& longValues) override;
    //long byteCount() const override;
    GribStatus valueCount(long& count) const override;
    //long byteOffset() const override;
    void updateSize(std::size_t s) override;
    bool isMissing() const override;

    // Data members
    long nbytes_;
    //grib_arguments* args_;
    AccessorInitArguments args_;

private:
    // Member functions
    GribStatus packLongUnsignedHelper(const std::vector<long>& longValues, int check);
};

}  // namespace eccodes::accessor
