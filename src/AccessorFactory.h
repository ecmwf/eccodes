#pragma once

#include "AccessorDefs.h"
#include <map>
#include <mutex>

//namespace eccodes::accessor {

class AccessorBuilderBase {
    AccessorType type_;

public:
    AccessorBuilderBase(AccessorType const&);
    virtual ~AccessorBuilderBase();
    virtual AccessorPtr make(AccessorName const& name, AccessorNameSpace const& nameSpace) = 0;
};

template <class ACCESSOR_DATA_TYPE>
class AccessorBuilder : public AccessorBuilderBase {
    AccessorPtr make(AccessorName const& name, AccessorNameSpace const& nameSpace) override {
        return new ACCESSOR_DATA_TYPE();
    }


public:
    AccessorBuilder() :
        AccessorBuilderBase(ACCESSOR_DATA_TYPE::accessor_type) {}
};

class AccessorFactory {
public:
    static AccessorFactory& instance();

    ~AccessorFactory();

    void add(AccessorType const& type, AccessorBuilderBase* builder);
    void remove(AccessorType const& type);
    bool has(AccessorType const& type);
    void list(std::ostream&);

    AccessorPtr build(AccessorType const& type, AccessorName const& name, AccessorNameSpace const& nameSpace);

private:
    AccessorFactory() {}

    std::map<AccessorType, AccessorBuilderBase*> builders_;
    std::recursive_mutex mutex_;
};

//}
