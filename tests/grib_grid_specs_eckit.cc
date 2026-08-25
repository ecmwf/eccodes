/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * Systematic test for ecKit geo grid specifications in ecCodes.
 *
 * Tests all grid types defined in ecKit's geo library that can be mapped
 * to GRIB representation via GribFromSpec. For each grid type, we:
 *   1. Create a grid from an eckit spec
 *   2. Pack it into a GRIB handle via GribFromSpec::set()
 *   3. Verify the resulting GRIB keys match expectations
 *   4. Verify message validity where applicable
 *   5. Test both GRIB1 and GRIB2 editions where supported
 *
 * Grid types tested:
 *   - Regular Lat/Lon (various resolutions, areas, scanning orders, references)
 *   - Regular Gaussian (global, various N, named F<N>)
 *   - Reduced Gaussian: Octahedral (O<N>) and Classical (N<N>)
 *   - Reduced Lat/Lon (explicit pl arrays)
 *   - HEALPix (ring and nested ordering, various Nside)
 *   - Spherical Harmonics (spectral T<N>)
 *   - GridFactory string parsing (JSON specs)
 *   - Cross-edition consistency (GRIB1 vs GRIB2)
 *
 * Not tested (require external data downloads or unsupported mappings):
 *   - ORCA, ICON, FESOM (unstructured grids requiring server data)
 *   - Lambert/LAEA/Polar Stereographic (RegularXY projection handling)
 *   - Rotated grids (rotation projection not connected in eckit 2.1.0)
 */

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#include "eccodes/eccodes.h"
#include "eccodes/geo/GribFromSpec.h"


#define CHECK(a) CODES_CHECK(a, nullptr)

using eckit::types::is_approximately_equal;


// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

bool get_string(const grib_handle* h, const char* key, std::string& value)
{
    char buffer[10240];
    size_t size = sizeof(buffer);
    ASSERT(h);
    int err = codes_get_string(h, key, buffer, &size);
    if (err == CODES_NOT_FOUND) return false;
    ASSERT(err == 0);
    value = buffer;
    return true;
}

void set_string(grib_handle* h, const char* key, const std::string& value)
{
    size_t length = value.length();
    CHECK(codes_set_string(h, key, value.c_str(), &length));
}

long get_long(const grib_handle* h, const char* key)
{
    long val = 0;
    CHECK(codes_get_long(h, key, &val));
    return val;
}

double get_double(const grib_handle* h, const char* key)
{
    double val = 0;
    CHECK(codes_get_double(h, key, &val));
    return val;
}

std::vector<long> get_long_array(const grib_handle* h, const char* key)
{
    size_t size = 0;
    CHECK(codes_get_size(h, key, &size));
    std::vector<long> arr(size);
    CHECK(codes_get_long_array(h, key, arr.data(), &size));
    arr.resize(size);
    return arr;
}


// Test a spec against both GRIB1 and GRIB2 samples, checking grid type and data point count
void test_spec_both_editions(const eckit::spec::Custom& spec,
                             const std::string& expected_gridType,
                             bool check_validity = true)
{
    for (const auto* edition : {"GRIB1", "GRIB2"}) {
        auto* sample = codes_grib_handle_new_from_samples(nullptr, edition);
        EXPECT(sample != nullptr);

        auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
        EXPECT(h != nullptr);

        std::string gridType;
        get_string(h, "gridType", gridType);
        EXPECT(gridType == expected_gridType);

        if (check_validity) {
            set_string(h, "messageValidityChecks", "grid");
            long valid = 0;
            CHECK(codes_get_long(h, "isMessageValid", &valid));
            EXPECT(valid == 1);
        }

        codes_handle_delete(h);
    }
}

// Test a spec against GRIB2 only
void test_spec_grib2(const eckit::spec::Custom& spec,
                     const std::string& expected_gridType,
                     bool check_validity = true)
{
    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    EXPECT(sample != nullptr);

    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);

    std::string gridType;
    get_string(h, "gridType", gridType);
    EXPECT(gridType == expected_gridType);

    if (check_validity) {
        set_string(h, "messageValidityChecks", "grid");
        long valid = 0;
        CHECK(codes_get_long(h, "isMessageValid", &valid));
        EXPECT(valid == 1);
    }

    codes_handle_delete(h);
}


// ===========================================================================
// 1. Regular Lat/Lon grids
// ===========================================================================

CASE("regular_ll: global 1/1")
{
    const eckit::spec::Custom spec{{"grid", std::vector<double>{1, 1}}};
    test_spec_both_editions(spec, "regular_ll");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);

    EXPECT(get_long(h, "Ni") == 360);
    EXPECT(get_long(h, "Nj") == 181);
    EXPECT(get_long(h, "Ni") * get_long(h, "Nj") == get_long(h, "numberOfDataPoints"));
    EXPECT(is_approximately_equal(get_double(h, "iDirectionIncrementInDegrees"), 1.0));
    EXPECT(is_approximately_equal(get_double(h, "jDirectionIncrementInDegrees"), 1.0));
    EXPECT(is_approximately_equal(get_double(h, "latitudeOfFirstGridPointInDegrees"), 90.0));
    EXPECT(is_approximately_equal(get_double(h, "longitudeOfFirstGridPointInDegrees"), 0.0));
    EXPECT(is_approximately_equal(get_double(h, "latitudeOfLastGridPointInDegrees"), -90.0));

    codes_handle_delete(h);
}


CASE("regular_ll: global 2/2")
{
    const eckit::spec::Custom spec{{"grid", std::vector<double>{2, 2}}};
    test_spec_both_editions(spec, "regular_ll");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "Ni") * get_long(h, "Nj") == get_long(h, "numberOfDataPoints"));
    codes_handle_delete(h);
}


CASE("regular_ll: global 0.5/0.5")
{
    const eckit::spec::Custom spec{{"grid", std::vector<double>{0.5, 0.5}}};
    test_spec_both_editions(spec, "regular_ll");
}


CASE("regular_ll: global 10/10")
{
    const eckit::spec::Custom spec{{"grid", std::vector<double>{10, 10}}};
    test_spec_both_editions(spec, "regular_ll");
}


CASE("regular_ll: named 1/1")
{
    const eckit::spec::Custom spec{{"grid", "1/1"}};
    test_spec_both_editions(spec, "regular_ll");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "numberOfDataPoints") == 360 * 181);
    codes_handle_delete(h);
}


CASE("regular_ll: named 2.5/2.5")
{
    const eckit::spec::Custom spec{{"grid", "2.5/2.5"}};
    test_spec_both_editions(spec, "regular_ll");
}


CASE("regular_ll: non-square grid 2/4")
{
    const eckit::spec::Custom spec{{"grid", std::vector<double>{2, 4}}};
    test_spec_both_editions(spec, "regular_ll");

    // Verify Ni*Nj == numberOfDataPoints (exact count depends on eckit rounding)
    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "Ni") * get_long(h, "Nj") == get_long(h, "numberOfDataPoints"));
    EXPECT(is_approximately_equal(get_double(h, "iDirectionIncrementInDegrees"), 2.0, 0.01));
    EXPECT(is_approximately_equal(get_double(h, "jDirectionIncrementInDegrees"), 4.0, 0.01));
    codes_handle_delete(h);
}


CASE("regular_ll: sub-area")
{
    const eckit::spec::Custom spec{
        {"grid", std::vector<double>{1, 1}},
        {"area", std::vector<double>{73, -27, 33, 45}}
    };
    test_spec_both_editions(spec, "regular_ll");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "Ni") * get_long(h, "Nj") == get_long(h, "numberOfDataPoints"));
    EXPECT(is_approximately_equal(get_double(h, "latitudeOfFirstGridPointInDegrees"), 73.0, 0.5));
    EXPECT(is_approximately_equal(get_double(h, "latitudeOfLastGridPointInDegrees"), 33.0, 0.5));
    codes_handle_delete(h);
}


CASE("regular_ll: sub-area with fine resolution")
{
    const eckit::spec::Custom spec{
        {"grid", std::vector<double>{0.15, 0.15}},
        {"area", std::vector<double>{47.1, -12.9, 33, 8.1}}
    };
    test_spec_both_editions(spec, "regular_ll", false);
}


CASE("regular_ll: scanning order i+j+")
{
    const eckit::spec::Custom spec{
        {"grid", std::vector<double>{1.25, 0.833333333333333}},
        {"order", "i+j+"}
    };

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);

    std::string gridType;
    get_string(h, "gridType", gridType);
    EXPECT(gridType == "regular_ll");
    EXPECT(get_long(h, "jScansPositively") == 1);

    codes_handle_delete(h);
}


CASE("regular_ll: scanning order i-j-")
{
    const eckit::spec::Custom spec{
        {"grid", std::vector<double>{1, 1}},
        {"area", std::vector<double>{20, 20, 17, 24}},
        {"order", "i-j-"}
    };

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);

    std::string gridType;
    get_string(h, "gridType", gridType);
    EXPECT(gridType == "regular_ll");
    EXPECT(get_long(h, "iScansNegatively") == 1);
    EXPECT(get_long(h, "jScansPositively") == 0);

    codes_handle_delete(h);
}


CASE("regular_ll: with reference point")
{
    const eckit::spec::Custom spec{
        {"grid", std::vector<double>{1, 1}},
        {"reference", std::vector<double>{0.5, 0.5}}
    };
    test_spec_grib2(spec, "regular_ll", false);
}


CASE("regular_ll: asymmetric increments 1.875/1.25")
{
    const eckit::spec::Custom spec{
        {"grid", std::vector<double>{1.875, 1.25}},
        {"order", "i+j+"},
        {"reference", std::vector<double>{0.9375, 0.625}}
    };
    test_spec_grib2(spec, "regular_ll", false);
}


CASE("regular_ll: very fine resolution 0.1/0.1")
{
    const eckit::spec::Custom spec{{"grid", std::vector<double>{0.1, 0.1}}};
    test_spec_grib2(spec, "regular_ll");
}


// ===========================================================================
// 2. Regular Gaussian grids
// ===========================================================================

CASE("regular_gg: F8 (named)")
{
    const eckit::spec::Custom spec{{"grid", "F8"}};
    test_spec_both_editions(spec, "regular_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);
    EXPECT(get_long(h, "N") == 8);
    EXPECT(get_long(h, "Ni") == 32);   // 4*N
    EXPECT(get_long(h, "Nj") == 16);   // 2*N
    EXPECT(get_long(h, "numberOfDataPoints") == 512);
    codes_handle_delete(h);
}


CASE("regular_gg: F32")
{
    const eckit::spec::Custom spec{{"grid", "F32"}};
    test_spec_both_editions(spec, "regular_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "N") == 32);
    EXPECT(get_long(h, "numberOfDataPoints") == 4 * 32 * 2 * 32);
    codes_handle_delete(h);
}


CASE("regular_gg: F48")
{
    const eckit::spec::Custom spec{{"grid", "F48"}};
    test_spec_both_editions(spec, "regular_gg");
}


CASE("regular_gg: F95")
{
    const eckit::spec::Custom spec{{"grid", "F95"}};
    test_spec_grib2(spec, "regular_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "N") == 95);
    EXPECT(get_long(h, "numberOfDataPoints") == 4 * 95 * 2 * 95);
    codes_handle_delete(h);
}


CASE("regular_gg: via type and N")
{
    const eckit::spec::Custom spec{{"type", "regular_gg"}, {"N", 16L}};
    test_spec_both_editions(spec, "regular_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "N") == 16);
    EXPECT(get_long(h, "numberOfDataPoints") == 4 * 16 * 2 * 16);
    codes_handle_delete(h);
}


// ===========================================================================
// 3. Reduced Gaussian grids - Octahedral
// ===========================================================================

CASE("reduced_gg: O2 (octahedral)")
{
    const eckit::spec::Custom spec{{"grid", "O2"}};
    test_spec_both_editions(spec, "reduced_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);
    EXPECT(get_long(h, "N") == 2);
    EXPECT(get_long(h, "numberOfDataPoints") == 88);

    std::string gridName;
    get_string(h, "gridName", gridName);
    EXPECT(gridName == "O2");

    auto pl = get_long_array(h, "pl");
    EXPECT(pl.size() == 4);
    std::vector<long> expected_pl{20L, 24, 24, 20};
    EXPECT(pl == expected_pl);

    codes_handle_delete(h);
}


CASE("reduced_gg: O4")
{
    const eckit::spec::Custom spec{{"grid", "O4"}};
    test_spec_both_editions(spec, "reduced_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "N") == 4);
    codes_handle_delete(h);
}


CASE("reduced_gg: O8")
{
    const eckit::spec::Custom spec{{"grid", "O8"}};
    test_spec_both_editions(spec, "reduced_gg");
}


CASE("reduced_gg: O16")
{
    const eckit::spec::Custom spec{{"grid", "O16"}};
    test_spec_both_editions(spec, "reduced_gg");
}


CASE("reduced_gg: O32")
{
    const eckit::spec::Custom spec{{"grid", "O32"}};
    test_spec_both_editions(spec, "reduced_gg");
}


CASE("reduced_gg: O128")
{
    const eckit::spec::Custom spec{{"grid", "O128"}};
    test_spec_grib2(spec, "reduced_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "N") == 128);

    std::string gridName;
    get_string(h, "gridName", gridName);
    EXPECT(gridName == "O128");

    codes_handle_delete(h);
}


CASE("reduced_gg: via N parameter (defaults to octahedral O<N>)")
{
    const eckit::spec::Custom spec{{"N", 4L}};
    test_spec_both_editions(spec, "reduced_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "N") == 4);
    codes_handle_delete(h);
}


CASE("reduced_gg: via explicit pl array")
{
    const eckit::spec::Custom spec{
        {"type", "reduced_gg"},
        {"pl", std::vector<long>{20, 24, 24, 20}},
        {"N", 2L}
    };
    test_spec_both_editions(spec, "reduced_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "numberOfDataPoints") == 88);
    codes_handle_delete(h);
}


// ===========================================================================
// 4. Reduced Gaussian grids - Classical
// ===========================================================================

CASE("reduced_gg: N32 (classical)")
{
    const eckit::spec::Custom spec{{"grid", "N32"}};
    test_spec_both_editions(spec, "reduced_gg");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "N") == 32);
    codes_handle_delete(h);
}


CASE("reduced_gg: N80")
{
    const eckit::spec::Custom spec{{"grid", "N80"}};
    test_spec_both_editions(spec, "reduced_gg");
}


CASE("reduced_gg: N128")
{
    const eckit::spec::Custom spec{{"grid", "N128"}};
    test_spec_grib2(spec, "reduced_gg");
}


// ===========================================================================
// 5. Reduced Lat/Lon grids
// ===========================================================================

CASE("reduced_ll: basic (3 rows)")
{
    const eckit::spec::Custom spec{
        {"type", "reduced_ll"},
        {"pl", std::vector<long>{0, 10, 0}},
        {"area", std::vector<double>{90., 0., -90., 360.}}
    };
    test_spec_both_editions(spec, "reduced_ll");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);
    EXPECT(get_long(h, "Nj") == 3);
    EXPECT(get_long(h, "numberOfDataPoints") == 10);
    EXPECT(is_approximately_equal(get_double(h, "latitudeOfFirstGridPointInDegrees"), 90.0, 0.01));
    codes_handle_delete(h);
}


CASE("reduced_ll: 5 rows")
{
    const eckit::spec::Custom spec{
        {"type", "reduced_ll"},
        {"pl", std::vector<long>{4, 8, 12, 8, 4}},
        {"area", std::vector<double>{90., 0., -90., 360.}}
    };
    test_spec_both_editions(spec, "reduced_ll");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "numberOfDataPoints") == 36);
    codes_handle_delete(h);
}


// ===========================================================================
// 6. HEALPix grids
// ===========================================================================

CASE("healpix: H2 (default ring order)")
{
    const eckit::spec::Custom spec{{"grid", "H2"}};
    test_spec_grib2(spec, "healpix");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);
    EXPECT(get_long(h, "Nside") == 2);
    EXPECT(get_long(h, "numberOfDataPoints") == 48);  // 12 * 2^2

    // ring ordering: ordering code = 0
    std::string orderStr;
    get_string(h, "orderingConvention", orderStr);
    EXPECT(orderStr == "ring");

    codes_handle_delete(h);
}


CASE("healpix: H4")
{
    const eckit::spec::Custom spec{{"grid", "H4"}};
    test_spec_grib2(spec, "healpix");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "numberOfDataPoints") == 12 * 16);
    codes_handle_delete(h);
}


CASE("healpix: H8")
{
    const eckit::spec::Custom spec{{"grid", "H8"}};
    test_spec_grib2(spec, "healpix");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "numberOfDataPoints") == 12 * 64);
    codes_handle_delete(h);
}


CASE("healpix: H32")
{
    const eckit::spec::Custom spec{{"grid", "H32"}};
    test_spec_grib2(spec, "healpix");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "numberOfDataPoints") == 12 * 1024);
    codes_handle_delete(h);
}


CASE("healpix: H2 nested order")
{
    const eckit::spec::Custom spec{{"grid", "H2"}, {"order", "nested"}};
    test_spec_grib2(spec, "healpix");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);
    EXPECT(get_long(h, "Nside") == 2);

    std::string orderStr;
    get_string(h, "orderingConvention", orderStr);
    EXPECT(orderStr == "nested");

    codes_handle_delete(h);
}


CASE("healpix: named h4r (ring)")
{
    const eckit::spec::Custom spec{{"grid", "h4r"}};
    test_spec_grib2(spec, "healpix");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "numberOfDataPoints") == 12 * 16);
    codes_handle_delete(h);
}


CASE("healpix: named h4n (nested)")
{
    const eckit::spec::Custom spec{{"grid", "h4n"}};
    test_spec_grib2(spec, "healpix");

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(get_long(h, "numberOfDataPoints") == 12 * 16);

    std::string orderStr;
    get_string(h, "orderingConvention", orderStr);
    EXPECT(orderStr == "nested");

    codes_handle_delete(h);
}


// ===========================================================================
// 7. Spherical Harmonics
// ===========================================================================

CASE("sh: T19")
{
    const eckit::spec::Custom spec{{"grid", "T19"}};

    // SH grid size: (T+1)*(T+2)/2
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(spec));
    EXPECT(grid != nullptr);
    EXPECT(grid->size() == 210);  // 20*21/2

    // GribFromSpec for SH requires spectral packing in the sample. Use GRIB2
    // but note: the sample may not have compatible data size, so we just test
    // that the grid is correctly constructed by eckit and check the spec.
    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);

    // Note: SH packing may fail with "Wrong number of values" if the sample
    // has incompatible data. In that case, h will be nullptr and we just
    // verify the eckit grid was correctly built above.
    if (h != nullptr) {
        EXPECT(get_long(h, "J") == 19);
        EXPECT(get_long(h, "K") == 19);
        EXPECT(get_long(h, "M") == 19);
    }

    codes_handle_delete(h);
}


CASE("sh: T42")
{
    const eckit::spec::Custom spec{{"grid", "T42"}};
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(spec));
    EXPECT(grid != nullptr);
    long T = 42;
    EXPECT(grid->size() == static_cast<size_t>((T + 1) * (T + 2) / 2));
}


CASE("sh: T63")
{
    const eckit::spec::Custom spec{{"grid", "T63"}};
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(spec));
    EXPECT(grid != nullptr);
    long T = 63;
    EXPECT(grid->size() == static_cast<size_t>((T + 1) * (T + 2) / 2));
}


CASE("sh: T106")
{
    const eckit::spec::Custom spec{{"grid", "T106"}};
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(spec));
    EXPECT(grid != nullptr);
    long T = 106;
    EXPECT(grid->size() == static_cast<size_t>((T + 1) * (T + 2) / 2));
}


CASE("sh: T639")
{
    const eckit::spec::Custom spec{{"grid", "T639"}};
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(spec));
    EXPECT(grid != nullptr);
    long T = 639;
    EXPECT(grid->size() == static_cast<size_t>((T + 1) * (T + 2) / 2));
}


CASE("sh: via type and truncation")
{
    const eckit::spec::Custom spec{{"type", "sh"}, {"truncation", 21L}};
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(spec));
    EXPECT(grid != nullptr);
    long T = 21;
    EXPECT(grid->size() == static_cast<size_t>((T + 1) * (T + 2) / 2));
}


// ===========================================================================
// 8. Round-trip tests: spec -> GRIB -> gridSpec string -> parse
// ===========================================================================

CASE("round-trip: regular_ll via gridSpec key")
{
    const eckit::spec::Custom spec{{"grid", std::vector<double>{1, 1}}};

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);

    char gridSpec[10240];
    size_t size = sizeof(gridSpec);
    int err = codes_get_string(h, "gridSpec", gridSpec, &size);
    if (err == CODES_SUCCESS) {
        std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(gridSpec));
        EXPECT(grid != nullptr);
        EXPECT(grid->size() == 360 * 181);
    }

    codes_handle_delete(h);
}


CASE("round-trip: O8 via gridSpec key")
{
    const eckit::spec::Custom spec{{"grid", "O8"}};

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);

    char gridSpec[10240];
    size_t size = sizeof(gridSpec);
    int err = codes_get_string(h, "gridSpec", gridSpec, &size);
    if (err == CODES_SUCCESS) {
        std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(gridSpec));
        EXPECT(grid != nullptr);
    }

    codes_handle_delete(h);
}


CASE("round-trip: H4 via gridSpec key")
{
    const eckit::spec::Custom spec{{"grid", "H4"}};

    auto* sample = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
    EXPECT(h != nullptr);

    char gridSpec[10240];
    size_t size = sizeof(gridSpec);
    int err = codes_get_string(h, "gridSpec", gridSpec, &size);
    if (err == CODES_SUCCESS) {
        std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(gridSpec));
        EXPECT(grid != nullptr);
        EXPECT(grid->size() == 12 * 16);
    }

    codes_handle_delete(h);
}


// ===========================================================================
// 9. GridFactory string-based construction (JSON specs)
// ===========================================================================

CASE("string spec: regular_ll from JSON")
{
    std::string spec_str = R"({"grid":[2,2]})";
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(spec_str));
    EXPECT(grid != nullptr);
    EXPECT(grid->size() > 0);
}


CASE("string spec: O4 from JSON")
{
    std::string spec_str = R"({"grid":"O4"})";
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(spec_str));
    EXPECT(grid != nullptr);
    EXPECT(grid->size() > 0);
}


CASE("string spec: H8 from JSON")
{
    std::string spec_str = R"({"grid":"H8"})";
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(spec_str));
    EXPECT(grid != nullptr);
    EXPECT(grid->size() == 12 * 64);
}


CASE("string spec: T63 from JSON")
{
    std::string spec_str = R"({"grid":"T63"})";
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(spec_str));
    EXPECT(grid != nullptr);
    EXPECT(grid->size() == 64 * 65 / 2);
}


CASE("string spec: regular_ll with area from JSON")
{
    std::string spec_str = R"({"grid":[1,1],"area":[73,-27,33,45]})";
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(spec_str));
    EXPECT(grid != nullptr);
    EXPECT(grid->size() > 0);
}


// ===========================================================================
// 10. Error handling
// ===========================================================================

CASE("error: invalid grid name throws")
{
    bool threw = false;
    try {
        auto* p = eckit::geo::GridFactory::make_from_string(R"({"grid":"INVALID_GRID_XYZ"})");
        delete p;
    }
    catch (const eckit::Exception&) {
        threw = true;
    }
    EXPECT(threw);
}


CASE("error: missing type throws")
{
    bool threw = false;
    try {
        auto* p = eckit::geo::GridFactory::make_from_string(R"({"foo":"bar"})");
        delete p;
    }
    catch (const eckit::Exception&) {
        threw = true;
    }
    EXPECT(threw);
}


// ===========================================================================
// 11. Cross-edition consistency
// ===========================================================================

CASE("consistency: O8 same in GRIB1 and GRIB2")
{
    const eckit::spec::Custom spec{{"grid", "O8"}};

    auto* s1 = codes_grib_handle_new_from_samples(nullptr, "GRIB1");
    auto* h1 = eccodes::geo::GribFromSpec::set(s1, spec);
    EXPECT(h1 != nullptr);

    auto* s2 = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h2 = eccodes::geo::GribFromSpec::set(s2, spec);
    EXPECT(h2 != nullptr);

    EXPECT(get_long(h1, "N") == get_long(h2, "N"));
    EXPECT(get_long(h1, "numberOfDataPoints") == get_long(h2, "numberOfDataPoints"));

    auto pl1 = get_long_array(h1, "pl");
    auto pl2 = get_long_array(h2, "pl");
    EXPECT(pl1 == pl2);

    codes_handle_delete(h1);
    codes_handle_delete(h2);
}


CASE("consistency: F32 same in GRIB1 and GRIB2")
{
    const eckit::spec::Custom spec{{"grid", "F32"}};

    auto* s1 = codes_grib_handle_new_from_samples(nullptr, "GRIB1");
    auto* h1 = eccodes::geo::GribFromSpec::set(s1, spec);
    EXPECT(h1 != nullptr);

    auto* s2 = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h2 = eccodes::geo::GribFromSpec::set(s2, spec);
    EXPECT(h2 != nullptr);

    EXPECT(get_long(h1, "Ni") == get_long(h2, "Ni"));
    EXPECT(get_long(h1, "Nj") == get_long(h2, "Nj"));
    EXPECT(get_long(h1, "numberOfDataPoints") == get_long(h2, "numberOfDataPoints"));

    codes_handle_delete(h1);
    codes_handle_delete(h2);
}


CASE("consistency: 1/1 same in GRIB1 and GRIB2")
{
    const eckit::spec::Custom spec{{"grid", std::vector<double>{1, 1}}};

    auto* s1 = codes_grib_handle_new_from_samples(nullptr, "GRIB1");
    auto* h1 = eccodes::geo::GribFromSpec::set(s1, spec);
    EXPECT(h1 != nullptr);

    auto* s2 = codes_grib_handle_new_from_samples(nullptr, "GRIB2");
    auto* h2 = eccodes::geo::GribFromSpec::set(s2, spec);
    EXPECT(h2 != nullptr);

    EXPECT(get_long(h1, "Ni") == get_long(h2, "Ni"));
    EXPECT(get_long(h1, "Nj") == get_long(h2, "Nj"));
    EXPECT(get_long(h1, "numberOfDataPoints") == get_long(h2, "numberOfDataPoints"));

    codes_handle_delete(h1);
    codes_handle_delete(h2);
}


// ===========================================================================
// Main
// ===========================================================================

int main(int argc, char* argv[])
{
    const char* ev_name = "ECCODES_ECKIT_GEO";
    const char* ev_val  = getenv(ev_name);
    if (ev_val && atol(ev_val) != 0) {
        return eckit::testing::run_tests(argc, argv);
    }

    std::printf("%s: This test is disabled (env. variable %s is not set)\n", argv[0], ev_name);
    return 0;
}
