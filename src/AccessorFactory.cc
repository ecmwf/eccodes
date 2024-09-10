#include "AccessorFactory.h"
#include "AccessorUtils/AccessorException.h"
#include "grib_api_internal.h"

#include <map>

//namespace eccodes::accessor {

AccessorFactory::~AccessorFactory() {
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    if(!builders_.empty())
    {
        for(auto it = builders_.begin(); it != builders_.end();)
        {
            grib_context* context = grib_context_get_default();
            grib_context_log(context, GRIB_LOG_DEBUG, "AccessorFactory::~AccessorFactory - Erasing %s", it->first.get().c_str());
            it = builders_.erase(it);
        }
    }
}

AccessorFactory& AccessorFactory::instance() {
    static AccessorFactory theOne;
    return theOne;
}

void AccessorFactory::add(AccessorType const& type, AccessorBuilderBase* builder)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    if (has(type)) {
        throw AccessorException("AccessorFactory::add - duplicate entry: " + type.get());
    }
    builders_[type] = builder;
}

void AccessorFactory::remove(AccessorType const& type)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    builders_.erase(type);
}

bool AccessorFactory::has(AccessorType const& type)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    return builders_.find(type) != builders_.end();
}

void AccessorFactory::list(std::ostream& out)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    grib_context* context = grib_context_get_default();
    const char* sep = "";
    for (auto const& entry : builders_) {
        grib_context_log(context, GRIB_LOG_DEBUG, "%s, ", entry.first.get().c_str());
    }
}

AccessorPtr AccessorFactory::build(AccessorType const& type, AccessorName const& name, AccessorNameSpace const& nameSpace)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);

    if (auto builder_ = builders_.find(type); builder_ == builders_.end()) {
        grib_context* context = grib_context_get_default();
        grib_context_log(context, GRIB_LOG_ERROR, "No AccessorBuilder called %s", type.get().c_str());
        grib_context_log(context, GRIB_LOG_ERROR, "AccessorBuilders are:");
        for (auto const& entry : builders_) {
            grib_context_log(context, GRIB_LOG_ERROR, "No AccessorBuilder called %s", entry.first.get().c_str());
        }
        throw AccessorException(std::string("No AccessorBuilder called ") + type.get());
    }
    else {
        AccessorPtr ptr = builder_->second->make(name, nameSpace);
        // TOOD(maee): Re-enable this when AccessorStore is used
        //AccessorStore::instance().addAccessor(ptr);
        return ptr;
    }  
}

AccessorBuilderBase::AccessorBuilderBase(AccessorType const& type) :
    type_(type) {
    AccessorFactory::instance().add(type_, this);
}

AccessorBuilderBase::~AccessorBuilderBase() {
    AccessorFactory::instance().remove(type_);
}

//}
