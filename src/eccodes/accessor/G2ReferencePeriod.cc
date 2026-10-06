#include "G2ReferencePeriod.h"

eccodes::AccessorBuilder<eccodes::accessor::G2ReferencePeriod> _grib_accessor_g2_referenceperiod_builder{};

namespace eccodes::accessor
{

void G2ReferencePeriod::init(const long length, grib_arguments* args)
{
    Unsigned::init(length, args);
    productDefinitionTemplateNumber_ = args->get_name(get_enclosing_handle(), 0);
}

int G2ReferencePeriod::unpack_long(long* val, size_t* len)
{
    *val = grib_is_defined(get_enclosing_handle(), "typeOfRelationToReferenceDataset");
    return GRIB_SUCCESS;
}

int G2ReferencePeriod::pack_long(const long* val, size_t* len)
{
    grib_handle* handle = get_enclosing_handle();
    long current_template = 0;
    int err = grib_get_long(handle, productDefinitionTemplateNumber_, &current_template);
    if (err) return err;
    const bool is_interval = grib_is_defined(handle, "numberOfTimeRanges");
    const bool is_ensemble = grib_is_defined(handle, "perturbationNumber");
    const bool is_probability = grib_is_defined(handle, "probabilityType");
    long template_number = is_interval ? (is_ensemble ? 106 : 105) : (is_ensemble ? 129 : 128);
    if (is_interval && grib_is_defined(handle, "derivedForecast")) {
        template_number = is_interval ? 107 : 130;
    }
    if (is_probability) {
        template_number = is_interval ? 112 : 131;
    }
    if (current_template == template_number) return GRIB_SUCCESS;
    return grib_set_long(handle, productDefinitionTemplateNumber_, template_number);
}

int G2ReferencePeriod::value_count(long* count)
{
    *count = 1;
    return GRIB_SUCCESS;
}

}
