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
#include <cstdint>
#include <cstring>

namespace eccodes {

class Accessor;

// Per-handle open-addressing hash map for accessor name lookups.
//
// Design:
// - Embedded fixed-size array (no heap allocation, zero-init by calloc).
// - 64-bit FNV-1a hash; matches by hash only (collision probability ~ 10^-15).
// - hash == 0 means "empty slot" (FNV-1a is OR'ed with 1 to ensure non-zero).
// - Thread-safe in the sense that distinct handles can be used from distinct
//   threads; the same handle must not be shared between threads (matches the
//   contract of grib_handle in develop branch).
class AccessorStore {
public:
    static constexpr std::size_t CAPACITY = 1024;  // power of 2
    static constexpr std::size_t MASK     = CAPACITY - 1;

    struct Slot {
        std::uint64_t hash;
        Accessor*     value;
    };

    static_assert((CAPACITY & MASK) == 0, "CAPACITY must be a power of 2");

    __attribute__((always_inline))
    static std::uint64_t compute_hash(const char* s) {
        std::uint64_t h = 14695981039346656037ULL;
        for (; *s; ++s) {
            h ^= static_cast<unsigned char>(*s);
            h *= 1099511628211ULL;
        }
        return h | 1;  // ensure non-zero (0 = empty sentinel)
    }

    __attribute__((always_inline))
    Accessor* get(const char* name) const {
        const std::uint64_t h = compute_hash(name);
        std::size_t idx = h & MASK;
        while (slots_[idx].hash) {
            if (slots_[idx].hash == h)
                return slots_[idx].value;
            idx = (idx + 1) & MASK;
        }
        return nullptr;
    }

    __attribute__((always_inline))
    void add(const char* name, Accessor* accessor) {
        exchange(name, accessor);
    }

    // Insert-or-replace; returns the previous value (nullptr if new).
    __attribute__((always_inline))
    Accessor* exchange(const char* name, Accessor* new_value) {
        const std::uint64_t h = compute_hash(name);
        std::size_t idx = h & MASK;
        while (slots_[idx].hash) {
            if (slots_[idx].hash == h) {
                Accessor* old = slots_[idx].value;
                slots_[idx].value = new_value;
                return old;
            }
            idx = (idx + 1) & MASK;
        }
        slots_[idx].hash  = h;
        slots_[idx].value = new_value;
        ++size_;
        return nullptr;
    }

    void remove(const char* name) {
        const std::uint64_t h = compute_hash(name);
        std::size_t idx = h & MASK;
        while (slots_[idx].hash) {
            if (slots_[idx].hash == h) {
                // Backward-shift deletion — preserves probe chains.
                std::size_t j = idx;
                for (;;) {
                    j = (j + 1) & MASK;
                    if (!slots_[j].hash) break;
                    const std::size_t k = slots_[j].hash & MASK;
                    if ((idx <= j) ? (k <= idx || k > j) : (k <= idx && k > j)) {
                        slots_[idx] = slots_[j];
                        idx = j;
                    }
                }
                slots_[idx] = {0, nullptr};
                --size_;
                return;
            }
            idx = (idx + 1) & MASK;
        }
    }

    void clear() {
        std::memset(slots_, 0, sizeof(slots_));
        size_ = 0;
    }

    template <typename F>
    void for_each(F&& func) const {
        for (std::size_t i = 0; i < CAPACITY; ++i) {
            if (slots_[i].hash)
                func(slots_[i].value);
        }
    }

    std::size_t size() const { return size_; }

private:
    Slot        slots_[CAPACITY];  // zero-init by calloc = empty table
    std::size_t size_ = 0;
};

}  // namespace eccodes
