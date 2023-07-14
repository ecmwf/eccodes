#ifndef _ACCESSOR_MAKER_H_20230714_
#define _ACCESSOR_MAKER_H_20230714_

#include "Accessor.h"
#include "AccessorPtr.h"

#include <memory>
#include <string>
#include <memory>

struct grib_section;
struct grib_action;
struct grib_arguments;
struct grib_accessor;

namespace eccodes {
class AccessorMaker;
void registerMaker(std::string const& name, AccessorMaker const* maker);
AccessorMaker const* getMaker(std::string const& name);

class AccessorMaker {
    struct MakerConcept {
        virtual ~MakerConcept() = default;
        virtual Accessor makeAccessor(grib_section* p, grib_action* creator, const long len, grib_arguments* arg) const = 0;
    };

    template<typename ACCESSOR_TYPE>
    struct MakerModel : public MakerConcept {
        Accessor makeAccessor(grib_section* p, grib_action* creator, const long len, grib_arguments* arg) const final {
            return Accessor(AccessorType<ACCESSOR_TYPE>{}, p, creator, len, arg);
        }
    };

    friend AccessorPtr makeAccessor(grib_section* p, grib_action* creator, const long len, grib_arguments* arg);

    std::unique_ptr<MakerConcept> pimpl;

public:
    template<typename ACCESSOR_TYPE>
    AccessorMaker(std::string const& name, AccessorType<ACCESSOR_TYPE>) : 
        pimpl{ std::make_unique<MakerModel<ACCESSOR_TYPE>>()} {
            registerMaker(name, this);
        }
};

}

#endif // _ACCESSOR_MAKER_H_20230714_
