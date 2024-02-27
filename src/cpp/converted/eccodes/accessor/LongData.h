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

#include "AccessorData/AccessorData.h"


namespace eccodes::accessor
{

// Forward declarations

class LongData : public AccessorData
{
public:
    LongData(const AccessorInitData& initData);
    ~LongData() = default;

protected:
    // Virtual member functions
    using AccessorData::pack;
    using AccessorData::unpack;
    GribType nativeType() const override;
    void dump() const override;
    GribStatus unpack(std::string& stringValue) const override;
    GribStatus packMissing() override;
    GribStatus unpack(std::vector<double>& doubleValues) const override;
    bool compare(const AccessorData& rhs) const override;
    GribStatus pack(const std::string& stringValue) override;

    // Data members


private:
    // Member functions
};

}  // namespace eccodes::accessor