#ifndef _NAMED_TYPE_H_20230817_
#define _NAMED_TYPE_H_20230817_

// https://www.fluentcpp.com/2016/12/08/strong-types-for-strong-interfaces/

#include <utility>

namespace eccodes::accessor {

template <typename T, typename Parameter>
class NamedType
{
public:
    explicit NamedType(T const& value) : value_(value) {}
    explicit NamedType(T&& value) : value_(std::move(value)) {}
    T& get() { return value_; }
    T const& get() const {return value_; }

    friend bool operator<(const NamedType& l, const NamedType& r) { return l.value_ < r.value_; }

private:
    T value_;
};

}

#endif // _NAMED_TYPE_H_20230817_
