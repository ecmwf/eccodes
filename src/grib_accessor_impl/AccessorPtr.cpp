#include "AccessorPtr.h"
#include "Accessor.h"
#include <algorithm>

namespace eccodes {

constexpr Accessor* AccessorPtr::release() noexcept {
    Accessor* tmp = get();
    reset();
    return tmp;
}

constexpr void AccessorPtr::reset(Accessor* accessor /* = nullptr */) noexcept {
    ptr_ = accessor;
}

void AccessorPtr::swap(AccessorPtr& other) noexcept {
    std::swap(ptr_, other.ptr_);
}

}
