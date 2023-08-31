#pragma once

#include "AccessorDefs.h"

#include <map>
#include <mutex>

namespace eccodes::accessor {

class AccessorBuilderBase {
    AccessorType type_;

public:
    AccessorBuilderBase(AccessorType const&);
    virtual ~AccessorBuilderBase();
    virtual AccessorDataPtr makeData(AccessorInitData const& initData) = 0;

    AccessorPtr make(AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorInitData const& initData);
};

template <class ACCESSOR_DATA_TYPE>
class AccessorBuilder : public AccessorBuilderBase {
    AccessorDataPtr makeData(AccessorInitData const& initData) { return std::make_unique<ACCESSOR_DATA_TYPE>(initData); }

public:
    AccessorBuilder(AccessorType const& type) :
        AccessorBuilderBase(type) {}
    ~AccessorBuilder() override = default;
};

class AccessorFactory {
public:
    static AccessorFactory& instance();

    void add(AccessorType const& type, AccessorBuilderBase* builder);
    void remove(AccessorType const& type);

    bool has(AccessorType const& type);
    void list(std::ostream&);

    AccessorPtr build(AccessorType const& type, AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorInitData const& initData);

private:
    AccessorFactory();

    std::map<AccessorType, AccessorBuilderBase*> builders_;
    std::recursive_mutex mutex_;
};

}
