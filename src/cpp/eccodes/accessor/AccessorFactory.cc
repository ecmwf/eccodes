#include "AccessorFactory.h"
#include "AccessorUtils.h"
#include "Accessor.h"

#include "grib_api_internal.h"

#include <map>

namespace eccodes::accessor {

//----------------------------------------------------------------------------------------------------------------------

AccessorFactory::AccessorFactory() {}

AccessorFactory& AccessorFactory::instance() {
    static AccessorFactory theOne;
    return theOne;
}

void AccessorFactory::add(AccessorType const& type, AccessorBuilderBase* builder) {

    std::lock_guard<std::recursive_mutex> guard(mutex_);

    if (has(type)) {
        throw AccessorException("AccessorFactory::add - duplicate entry: " + type.get());
    }
    builders_[type] = builder;
}

void AccessorFactory::remove(AccessorType const& type) {

    std::lock_guard<std::recursive_mutex> guard(mutex_);
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

    debugLog() << "Looking for AccessorBuilder [" << type.get() << "]" << std::endl;

    if (auto builder_ = builders_.find(type); builder_ == builders_.end()) {
        errorLog() << "No AccessorBuilder for [" << type.get() << "]" << std::endl;
        errorLog() << "AccessorBuilders are:" << std::endl;
        for (auto const& entry : builders_) {
            errorLog() << "   " << entry.first.get() << std::endl;
        }
        throw AccessorException(std::string("No AccessorBuilder called ") + type.get());
    }
    else return builder_->second->make(name, nameSpace, initData);
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
