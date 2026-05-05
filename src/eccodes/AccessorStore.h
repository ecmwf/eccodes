/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace eccodes {

class Accessor;

// Per-handle open-addressing hash map for accessor name lookups.
// Designed to be embedded in the handle struct (zero-initialized by calloc).
// Uses a fixed-size embedded array — no heap allocation, no pointer indirection.
// Not thread-safe — handles are not shared between threads.
class AccessorStore {
public:
    static constexpr size_t CAPACITY = 1024;
    static constexpr size_t MASK = CAPACITY - 1;
    static constexpr size_t EMPTY = 0;

    struct Slot {
        size_t hash;
        Accessor* value;
    };

    // No constructor needed — zero-initialization (calloc) produces a valid empty store.
    // No destructor needed — no heap allocations (unless grow() is called).

    __attribute__((always_inline))
    static size_t compute_hash(const char* s) {
        size_t h = 14695981039346656037ULL;
        for (; *s; ++s) {
            h ^= static_cast<unsigned char>(*s);
            h *= 1099511628211ULL;
        }
        return h | 1; // ensure non-zero (0 = empty sentinel)
    }

    __attribute__((always_inline))
    Accessor* get(const char* name) const {
        const size_t h = compute_hash(name);
        const Slot* s = slots();
        size_t idx = h & mask();
        while (s[idx].hash != EMPTY) {
            if (s[idx].hash == h)
                return s[idx].value;
            idx = (idx + 1) & mask();
        }
        return nullptr;
    }

    __attribute__((always_inline))
    void add(const char* name, Accessor* accessor) {
        if (__builtin_expect(size_ * 10 >= (mask() + 1) * 7, 0))
            grow();

        const size_t h = compute_hash(name);
        Slot* s = slots();
        size_t idx = h & mask();
        while (s[idx].hash != EMPTY) {
            if (s[idx].hash == h) {
                s[idx].value = accessor;
                return;
            }
            idx = (idx + 1) & mask();
        }
        s[idx].hash = h;
        s[idx].value = accessor;
        ++size_;
    }

    // Combined get + replace: returns old value, stores new value. One hash computation.
    __attribute__((always_inline))
    Accessor* exchange(const char* name, Accessor* new_value) {
        if (__builtin_expect(size_ * 10 >= (mask() + 1) * 7, 0))
            grow();

        const size_t h = compute_hash(name);
        Slot* s = slots();
        size_t idx = h & mask();
        while (s[idx].hash != EMPTY) {
            if (s[idx].hash == h) {
                Accessor* old = s[idx].value;
                s[idx].value = new_value;
                return old;
            }
            idx = (idx + 1) & mask();
        }
        s[idx].hash = h;
        s[idx].value = new_value;
        ++size_;
        return nullptr;
    }

    void remove(const char* name);
    void clear();
    void destroy(); // call before handle is freed (only needed if grow() was called)

    template <typename F>
    void for_each(F&& func) const {
        const Slot* s = slots();
        const size_t m = mask();
        for (size_t i = 0; i <= m; ++i) {
            if (s[i].hash != EMPTY)
                func(s[i].value);
        }
    }

private:
    __attribute__((always_inline))
    Slot* slots() { return overflow_ ? overflow_ : embedded_; }

    __attribute__((always_inline))
    const Slot* slots() const { return overflow_ ? overflow_ : embedded_; }

    __attribute__((always_inline))
    size_t mask() const { return overflow_ ? overflow_mask_ : MASK; }

    void grow();

    // Embedded storage — zero-initialized by calloc = empty table
    Slot embedded_[CAPACITY];
    Slot* overflow_ = nullptr;       // non-null only after grow()
    size_t overflow_mask_ = 0;
    size_t size_ = 0;
};

} // namespace eccodes
