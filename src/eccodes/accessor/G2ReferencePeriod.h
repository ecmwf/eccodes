#pragma once

#include "Unsigned.h"

namespace eccodes::accessor
{

class G2ReferencePeriod : public Unsigned
{
public:
    const AccessorType& accessor_type() const override { return accessor_type_; }
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int value_count(long* count) override;
    void init(const long length, grib_arguments* args) override;

private:
    const char* productDefinitionTemplateNumber_ = nullptr;

public:
    static inline const AccessorType accessor_type_{"g2_referenceperiod"};
};

}
