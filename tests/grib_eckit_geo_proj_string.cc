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


#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/Projection.h"
#include "eckit/testing/Test.h"

#include "eccodes/geo/GribToSpec.h"
#include "eccodes/eccodes.h"
#include "eccodes/geo/eckit.h"


namespace eccodes::tests
{


std::string get_string(codes_handle* h, const char* key)
{
    char value[1024] = { 0 };
    size_t length = sizeof(value);
    return codes_get_string(h, key, value, &length) == CODES_SUCCESS ? value : "";
}


/// PROJ string parameters, comparable: without defaults, the figure as semi-major/minor axes
std::map<std::string, std::string> proj_parameters(const std::string& str)
{
    std::map<std::string, std::string> p;

    std::istringstream in(str);
    for (std::string token; in >> token;) {
        ASSERT(token.size() > 1 && token.front() == '+');
        const auto eq = token.find('=');
        p[token.substr(1, eq - 1)] = eq == std::string::npos ? "" : token.substr(eq + 1);
    }

    auto erase_if = [&p](const char* key, double value) {
        if (auto it = p.find(key); it != p.end() && std::abs(std::stod(it->second)) == value) {
            p.erase(it);
        }
    };

    // defaults, polar stereographic true scale at the pole
    for (const auto* key : { "lat_0", "lon_0", "x_0", "y_0" }) {
        erase_if(key, 0.);
    }
    erase_if("k_0", 1.);
    erase_if("lat_ts", 90.);
    p.erase("no_defs");
    p.erase("type");

    // a tangent cone
    if (p.count("lat_2") != 0 && p.count("lat_1") != 0 && std::stod(p["lat_1"]) == std::stod(p["lat_2"])) {
        p.erase("lat_2");
    }

    // the figure as semi-major/minor axes
    static const std::map<std::string, std::pair<std::string, std::string>> ELLPS{
        { "WGS84", { "6378137", "6356752.314245" } },
        { "GRS80", { "6378137", "6356752.314140" } },
    };

    if (auto R = p.find("R"); R != p.end()) {
        p["a"] = p["b"] = R->second;
        p.erase("R");
    }

    for (const auto* key : { "ellps", "datum" }) {
        if (auto e = p.find(key); e != p.end()) {
            const auto& [a, b] = ELLPS.at(e->second);
            p["a"] = a;
            p["b"] = b;
            p.erase(key);
        }
    }

    return p;
}


bool proj_equivalent(const std::string& a, const std::string& b)
{
    auto pa = proj_parameters(a);
    auto pb = proj_parameters(b);

    // unprojected grids: eccodes projString always assumes WGS84
    if (pa["proj"] == "longlat" && pb["proj"] == "longlat") {
        return true;
    }

    return pa.size() == pb.size() && std::all_of(pa.begin(), pa.end(), [&pb](const auto& kv) {
               const auto& [key, value] = kv;
               const auto other = pb.find(key);
               if (other == pb.end() || key == "proj") {
                   return other != pb.end() && value == other->second;
               }

               // values to the precision of eccodes projString (axes, to millimetres; angles, modulo 360)
               auto d = std::stod(value) - std::stod(other->second);
               if (key == "lon_0") {
                   d = eckit::geo::PointLonLat::normalise_angle_to_minimum(d, -180.);
               }

               return std::abs(d) <= (key == "a" || key == "b" ? 1e-3 : 1e-6);
           });
}


CASE("projString(eccodes) == proj_str(eckit::geo::Grid projection)")
{
    for (int i = 1; i < eckit::Main::instance().argc(); ++i) {
        const auto& path = eckit::Main::instance().argv(i);

        auto* in = std::fopen(path.c_str(), "rb");
        ASSERT(in != nullptr);

        int err = 0;
        for (grib_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err));) {
            ASSERT(err == CODES_SUCCESS);
            std::unique_ptr<codes_handle, decltype(&codes_handle_delete)> handle(h, &codes_handle_delete);

            const auto label = path + " (gridType=" + get_string(h, "gridType") + ")";

            std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(geo::GribToSpec(h)));
            ASSERT(grid);

            // eccodes projString (if supported, ECCODES_ECKIT_GEO=0)
            const auto projString = get_string(h, "projString");
            const auto& projection = grid->projection();

            std::string proj_str;
            try {
                proj_str = projection.proj_str();
            }
            catch (const eckit::geo::exception::ProjectionError&) {
                std::printf("%s: eckit::geo projection %s without PROJ equivalent\n", label.c_str(),
                            projection.spec_str().c_str());
                EXPECT(projString.empty());
                continue;
            }
            EXPECT(!proj_str.empty());

            // the projection from its spec (default figures are not in specs)
            std::unique_ptr<const eckit::geo::Projection> same(eckit::geo::ProjectionFactory::build(projection.spec()));
            EXPECT(same->spec_str() == projection.spec_str());
            EXPECT(projection.figure().is_default() || same->proj_str() == proj_str);

            if (projString.empty()) {
                std::printf("%s: eccodes projString not supported, not compared\n", label.c_str());
                continue;
            }

            if (!proj_equivalent(projString, proj_str)) {
                std::printf("%s: eccodes '%s' and eckit::geo '%s' differ\n", label.c_str(), projString.c_str(),
                            proj_str.c_str());
            }
            EXPECT(proj_equivalent(projString, proj_str));
        }

        std::fclose(in);
    }
}


}  // namespace eccodes::tests


int main(int argc, char* argv[])
{
    return eckit::testing::run_tests(argc, argv);
}
