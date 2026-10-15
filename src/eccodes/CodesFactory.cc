/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "CodesFactory.h"
#include "grib_api_internal.h"

#include <algorithm>
#include <stdexcept>

namespace eccodes {

void throwNoCreator(const char* type, const std::vector<const char*>& knownTypes)
{
    const grib_context* context = grib_context_get_default();
    grib_context_log(context, GRIB_LOG_ERROR, "No creator for type %s", type);

    // Suggest the 5 most similar known types, using the Levenshtein distance
    std::vector<std::pair<size_t, const char*>> candidates;
    for (const char* known : knownTypes) {
        candidates.push_back({levenshteinDistance(type, known), known});
    }
    const size_t n = std::min<size_t>(5, candidates.size());
    std::partial_sort(candidates.begin(), candidates.begin() + n, candidates.end(),
                      [](auto const& a, auto const& b) { return a.first < b.first; });
    if (n > 0) {
        grib_context_log(context, GRIB_LOG_ERROR, "Did you mean:");
        for (size_t i = 0; i < n; ++i) {
            grib_context_log(context, GRIB_LOG_ERROR, "  %s", candidates[i].second);
        }
    }
    throw std::runtime_error(std::string("No creator for type ") + type);
}

} // namespace eccodes
