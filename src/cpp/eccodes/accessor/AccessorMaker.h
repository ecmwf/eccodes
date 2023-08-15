#ifndef _ACCESSOR_MAKER_H_20230714_
#define _ACCESSOR_MAKER_H_20230714_

#include "Accessor.h"

#include <memory>
#include <string>
#include <memory>

struct grib_section;
struct grib_action;
struct grib_arguments;
struct grib_accessor;

namespace eccodes::accessor {
class AccessorMaker;
void registerMaker(std::string const& name, AccessorMaker const* maker);
AccessorMaker const* getMaker(std::string const& name);

using AccessorPtr = std::shared_ptr<Accessor>;

// Helper for template type deduction
template<typename ACCESSOR_DATA_TYPE>
struct AccessorDataType{};

class AccessorMaker {
    struct MakerConcept {
        virtual ~MakerConcept() = default;
        virtual AccessorPtr makeAccessorImpl(grib_section* p, grib_action* creator, const long len, grib_arguments* arg) const = 0;
    };

    template<typename ACCESSOR_DATA_TYPE>
    struct MakerModel : public MakerConcept {
        AccessorPtr makeAccessorImpl(grib_section* p, grib_action* creator, const long len, grib_arguments* arg) const final {
            std::unique_ptr<AccessorData> data = std::make_unique<ACCESSOR_DATA_TYPE>(len, arg);
            return std::make_shared<Accessor>(std::move(data), p, creator);
        }
    };

    friend AccessorPtr makeAccessor(grib_section* p, grib_action* creator, const long len, grib_arguments* arg);

    std::unique_ptr<MakerConcept> pimpl;

public:
    template<typename ACCESSOR_DATA_TYPE>
    AccessorMaker(std::string const& name, AccessorDataType<ACCESSOR_DATA_TYPE>) : 
        pimpl{ std::make_unique<MakerModel<ACCESSOR_DATA_TYPE>>()} {
            registerMaker(name, this);
        }
};

}

#endif // _ACCESSOR_MAKER_H_20230714_
