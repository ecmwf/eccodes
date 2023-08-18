#ifndef _ACCESSOR_MAKER_H_20230714_
#define _ACCESSOR_MAKER_H_20230714_

#include "AccessorDefs.h"
#include "Accessor.h"
#include "AccessorMakerRegistry.h"
#include <memory>

namespace eccodes::accessor {

// Helper for template type deduction
template<typename ACCESSOR_DATA_TYPE>
struct AccessorDataType{};

class AccessorMaker {
    struct MakerConcept {
        virtual ~MakerConcept() = default;
        virtual AccessorPtr makeAccessorImpl(AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorInitData const& initData) const = 0;
    };

    template<typename ACCESSOR_DATA_TYPE>
    struct MakerModel : public MakerConcept {
        AccessorPtr makeAccessorImpl(AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorInitData const& initData) const final {
            std::unique_ptr<AccessorData> data = std::make_unique<ACCESSOR_DATA_TYPE>(initData);
            return std::make_shared<Accessor>(name, nameSpace, std::move(data));
        }
    };

    friend AccessorPtr makeAccessor(AccessorType const& type, AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorInitData const& initData);

    std::unique_ptr<MakerConcept> pimpl;

public:
    template<typename ACCESSOR_DATA_TYPE>
    AccessorMaker(AccessorType const& type, AccessorDataType<ACCESSOR_DATA_TYPE>) : 
        pimpl{ std::make_unique<MakerModel<ACCESSOR_DATA_TYPE>>()} {}
};

template<typename T>
AccessorMaker& createMaker(AccessorType const& type)
{
    static AccessorMaker maker = [&type](){ 
        AccessorMaker maker_(type, AccessorDataType<T>{});
        registerMaker(type, &maker_);
        return maker_;
    }();

    return maker;
}

}

#endif // _ACCESSOR_MAKER_H_20230714_
