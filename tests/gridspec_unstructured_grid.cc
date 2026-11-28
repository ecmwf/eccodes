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


// gridSpec of gridType=unstructured_grid, GRIB2 only (ORCA, ICON, and others by uid)


#include <string>
#include <vector>

#include "gridspec.h"


using gridspec::encode_t;


CASE("gridSpec to GRIB and back")
{
    EXPECT(gridspec::check_all<encode_t>({
        // ORCA: by type and arrangement (and uid)
        { "GRIB2", R"({"grid":"ORCA2_T"})", "unstructured_grid", { { "unstructuredGridType", "ORCA2" }, { "unstructuredGridSubtype", "T" }, { "uuidOfHGrid", "d5bde4f52ff3a9bea5629cd9ac514410" }, { "numberOfDataPoints", 149 * 182 } } },
        { "GRIB2", R"({"grid":"eORCA1_U"})", "unstructured_grid", { { "unstructuredGridType", "eORCA1" }, { "unstructuredGridSubtype", "U" }, { "uuidOfHGrid", "4eb1054957dcae914e219faf9a4068e3" }, { "numberOfDataPoints", 120184 } } },

        // by uid, of a grid known by name (the name and the uid of these points)
        { "GRIB2", R"({"uid":"16076978a048410747dd7c9876677b28"})", "unstructured_grid", { { "unstructuredGridType", "eORCA1" }, { "unstructuredGridSubtype", "T" }, { "uuidOfHGrid", "16076978a048410747dd7c9876677b28" } }, R"({"grid":"eORCA1_T","uid":"16076978a048410747dd7c9876677b28"})" },

        // ICON: by uid and number of grid (known by name, and by uid)
        { "GRIB2", R"({"grid":"icon-grid-0012-r02b04-g"})", "unstructured_grid", { { "uuidOfHGrid", "968657501dce11b29969d72c2fa229cf" }, { "numberOfGridUsed", 12 }, { "numberOfDataPoints", 20480 } } },
        { "GRIB2", R"({"uid":"968657501dce11b29969d72c2fa229cf"})", "unstructured_grid", { { "uuidOfHGrid", "968657501dce11b29969d72c2fa229cf" } }, R"({"grid":"icon-grid-0012-r02b04-g"})" },
    }));
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=unstructured_grid,icon.grib",
                                   { R"({"grid":"icon-grid-0012-r02b04-g"})" }));

    EXPECT(gridspec::check_decoded("gridspec/gridType=unstructured_grid,orca.grib",
                                   {
                                       R"({"grid":"eORCA1_T","uid":"16076978a048410747dd7c9876677b28"})",
                                       R"({"grid":"eORCA1_T"})",
                                       R"({"grid":"eORCA1_U"})",
                                       R"({"grid":"eORCA1_U","uid":"7378487847e050559b82d0792374a705"})",
                                       R"({"grid":"eORCA1_V","uid":"d9622b55f3120eafb3dbaf5c742bc56c"})",
                                       R"({"grid":"eORCA1_V"})",
                                   }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
