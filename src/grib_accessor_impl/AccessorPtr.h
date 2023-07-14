#ifndef _ACCESSOR_PTR_H_20230712_
#define _ACCESSOR_PTR_H_20230712_

namespace eccodes {

// A non-owning pointer to an accessor
// Based on https://en.cppreference.com/w/cpp/experimental/observer_ptr

class Accessor;

class AccessorPtr{
    Accessor* ptr_{};

public:
    constexpr AccessorPtr() noexcept : ptr_{nullptr} {};
    constexpr explicit AccessorPtr(Accessor* accessor) noexcept : ptr_{accessor} {};

    constexpr Accessor* get() const noexcept { return ptr_; }
    constexpr Accessor& operator*() const { return *get(); }
    constexpr Accessor* operator->() const noexcept { return get(); }
    constexpr explicit operator bool() const noexcept {	return get() != nullptr; }
    constexpr explicit operator Accessor*() const noexcept { return get(); }

    constexpr Accessor* release() noexcept;
    constexpr void reset(Accessor* accessor = nullptr) noexcept;
    void swap(AccessorPtr& other) noexcept;
};

}

#endif // _ACCESSOR_PTR_H_20230712_
