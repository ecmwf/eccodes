/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "AccessorStore.h"
#include <cstring>

namespace eccodes {

void AccessorStore::remove(const char* name)
{
    const size_t h = compute_hash(name);
    Slot* s = slots();
    const size_t m = mask();
    size_t idx = h & m;

    while (s[idx].hash != EMPTY) {
        if (s[idx].hash == h) {
            --size_;
            // Backward shift deletion
            size_t j = idx;
            for (;;) {
                j = (j + 1) & m;
                if (s[j].hash == EMPTY) break;
                size_t k = s[j].hash & m;
                if ((idx <= j) ? (k <= idx || k > j) : (k <= idx && k > j)) {
                    s[idx] = s[j];
                    idx = j;
                }
            }
            s[idx].hash = EMPTY;
            s[idx].value = nullptr;
            return;
        }
        idx = (idx + 1) & m;
    }
}

void AccessorStore::clear()
{
    Slot* s = slots();
    std::memset(s, 0, (mask() + 1) * sizeof(Slot));
    size_ = 0;
}

void AccessorStore::destroy()
{
    if (overflow_) {
        std::free(overflow_);
        overflow_ = nullptr;
        overflow_mask_ = 0;
    }
    size_ = 0;
}

void AccessorStore::grow()
{
    const size_t old_mask = mask();
    const Slot* old_slots = slots();

    const size_t new_cap = (old_mask + 1) * 2;
    const size_t new_mask = new_cap - 1;

    Slot* new_slots = static_cast<Slot*>(std::calloc(new_cap, sizeof(Slot)));

    for (size_t i = 0; i <= old_mask; ++i) {
        if (old_slots[i].hash != EMPTY) {
            size_t idx = old_slots[i].hash & new_mask;
            while (new_slots[idx].hash != EMPTY)
                idx = (idx + 1) & new_mask;
            new_slots[idx] = old_slots[i];
        }
    }

    if (overflow_)
        std::free(overflow_);

    overflow_ = new_slots;
    overflow_mask_ = new_mask;
}

} // namespace eccodes
