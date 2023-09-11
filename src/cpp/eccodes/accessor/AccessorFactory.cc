#include "AccessorFactory.h"
#include "Accessor.h"
#include "AccessorUtils/AccessorLogger.h"
#include "AccessorUtils/AccessorException.h"
#include "AccessorStore.h"
#include "grib_api_internal.h"

#include <map>

namespace eccodes::accessor {

//----------------------------------------------------------------------------------------------------------------------

AccessorFactory::~AccessorFactory() {
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    if(!builders_.empty())
    {
        debugLog() << "WARNING: AccessorFactory being destroyed but it still contains builders! [Size = " << builders_.size() << "] - Cleaning up...\n";
        for(auto it = builders_.begin(); it != builders_.end();)
        {
            debugLog() << "-> Erasing " << it->first.get() << '\n';
            it = builders_.erase(it);
        }
    }
}

AccessorFactory& AccessorFactory::instance() {
    static AccessorFactory theOne;
    return theOne;
}

void AccessorFactory::add(AccessorType const& type, AccessorBuilderBase* builder) {

    std::lock_guard<std::recursive_mutex> guard(mutex_);

    debugLog() << "Adding AccessorBuilder [" << type.get() << "]\n";

    if (has(type)) {
        throw AccessorException("AccessorFactory::add - duplicate entry: " + type.get());
    }
    builders_[type] = builder;
}

void AccessorFactory::remove(AccessorType const& type) {

    std::lock_guard<std::recursive_mutex> guard(mutex_);

    debugLog() << "Removing AccessorBuilder [" << type.get() << "]\n";

    builders_.erase(type);
}

bool AccessorFactory::has(AccessorType const& type) {

    std::lock_guard<std::recursive_mutex> guard(mutex_);
    return builders_.find(type) != builders_.end();
}

void AccessorFactory::list(std::ostream& out) {
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    const char* sep = "";
    for (auto const& entry : builders_) {
        out << sep << entry.first.get();
        sep = ", ";
    }
}

AccessorPtr AccessorFactory::build(AccessorType const& type, AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorInitData const& initData) {

    std::lock_guard<std::recursive_mutex> guard(mutex_);

    debugLog() << "Looking for AccessorBuilder [" << type.get() << "]\n";

    if (auto builder_ = builders_.find(type); builder_ == builders_.end()) {
        errorLog() << "No AccessorBuilder for [" << type.get() << "]\n";
        errorLog() << "AccessorBuilders are:\n";
        for (auto const& entry : builders_) {
            errorLog() << "   " << entry.first.get() << '\n';
        }
        throw AccessorException(std::string("No AccessorBuilder called ") + type.get());
    }
    else {
        AccessorPtr ptr = builder_->second->make(name, nameSpace, initData);
        AccessorStore::instance().addAccessor(ptr);
        return ptr;
    }  
}

//----------------------------------------------------------------------------------------------------------------------

AccessorBuilderBase::AccessorBuilderBase(AccessorType const& type) :
    type_(type) {
    AccessorFactory::instance().add(type_, this);
}

AccessorBuilderBase::~AccessorBuilderBase() {
    AccessorFactory::instance().remove(type_);
}

AccessorPtr AccessorBuilderBase::make(AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorInitData const& initData) { 
    return std::make_shared<Accessor>(name, nameSpace, makeData(initData)); 
}

}
