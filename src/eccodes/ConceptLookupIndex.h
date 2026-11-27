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

#include "grib_api_internal.h"
#include "expression/Long.h"
#include <array>
#include <cstring>
#include <map>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace eccodes
{

// Only a leading sequence of literal integer comparisons is indexable.
// All other rules retain their position and are evaluated by the original matcher.
class ConceptLookupIndex
{
public:
    using Key = std::array<long, 3>;
    struct Entry
    {
        size_t ordinal;
        grib_concept_value* rule;
    };
    using Entries = std::vector<Entry>;
    static constexpr std::array<const char*, 3> keys = {
        "discipline", "parameterCategory", "parameterNumber"
    };

    explicit ConceptLookupIndex(grib_concept_value* rules)
    {
        size_t ordinal = 0;
        for (auto* rule = rules; rule; rule = rule->next, ++ordinal) {
            Key key{};
            auto* condition = rule->conditions;
            bool eligible = true;
            for (size_t i = 0; i < keys.size(); ++i) {
                if (!condition || !condition->expression ||
                    std::strcmp(condition->name, keys[i]) != 0 ||
                    typeid(*condition->expression) != typeid(expression::Long) ||
                    condition->expression->evaluate_long(nullptr, &key[i]) != GRIB_SUCCESS) {
                    eligible = false;
                    break;
                }
                condition = condition->next;
            }
            if (eligible)
                groups_[key].push_back({ ordinal, rule });
            else
                fallback_.push_back({ ordinal, rule });
        }
    }

    bool empty() const { return groups_.empty(); }
    const Entries& fallback() const { return fallback_; }
    const Entries& candidates(const Key& key) const
    {
        auto it = groups_.find(key);
        static const Entries empty;
        return it == groups_.end() ? empty : it->second;
    }

private:
    std::map<Key, Entries> groups_;
    Entries fallback_;
};

class ConceptLookupCache
{
public:
    const ConceptLookupIndex* get(grib_concept_value* rules)
    {
        auto it = indexes_.find(rules);
        if (it == indexes_.end()) {
            auto index = std::make_unique<ConceptLookupIndex>(rules);
            if (index->empty())
                index.reset();
            it = indexes_.emplace(rules, std::move(index)).first;
        }
        return it->second.get();
    }

private:
    std::unordered_map<grib_concept_value*, std::unique_ptr<ConceptLookupIndex>> indexes_;
};

}  // namespace eccodes
