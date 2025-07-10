/*
 * (C) Copyright 2024- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eccodes/geo/GribToSpec.h"

#include <algorithm>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <sstream>
#include <vector>

#include "eckit/config/Resource.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/JSON.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::util
{
const std::vector<double>& gaussian_latitudes(size_t N, bool increasing);
}


namespace eccodes::geo
{


namespace
{


using eckit::Log;


struct Condition
{
    Condition() = default;

    Condition(const Condition&)            = delete;
    Condition(Condition&&)                 = delete;
    Condition& operator=(const Condition&) = delete;
    Condition& operator=(Condition&&)      = delete;

    virtual ~Condition()                   = default;
    virtual bool eval(codes_handle*) const = 0;
};


template <class T>
struct ConditionT : Condition
{
    const char* key_;
    T value_;
    bool eval(codes_handle* /*unused*/) const override;

public:
    ConditionT(const char* key, const T& value) :
        key_(key), value_(value) {}
};


template <>
bool ConditionT<long>::eval(codes_handle* h) const
{
    ASSERT(h != nullptr);

    long value = 0;
    int err    = codes_get_long(h, key_, &value);

    if (err == CODES_NOT_FOUND) {
        return false;
    }

    CHECK_ERROR(err, key_);
    return value_ == value;
}


template <>
bool ConditionT<double>::eval(codes_handle* h) const
{
    ASSERT(h != nullptr);

    double value = 0;
    int err      = codes_get_double(h, key_, &value);

    if (err == CODES_NOT_FOUND) {
        return false;
    }

    CHECK_ERROR(err, key_);
    return value_ == value;  // Want an epsilon?
}


template <>
bool ConditionT<std::string>::eval(codes_handle* h) const
{
    ASSERT(h != nullptr);

    char buffer[10240];
    size_t size = sizeof(buffer);
    int err     = codes_get_string(h, key_, buffer, &size);

    if (err == CODES_NOT_FOUND) {
        return false;
    }

    CHECK_ERROR(err, key_);
    return value_ == buffer;
}


struct ConditionOR : Condition
{
    const std::unique_ptr<const Condition> left_;
    const std::unique_ptr<const Condition> right_;
    bool eval(codes_handle* h) const override { return left_->eval(h) || right_->eval(h); }

    ConditionOR(const Condition* left, const Condition* right) :
        left_(left), right_(right) {}
};


struct ConditionAND : Condition
{
    const std::unique_ptr<const Condition> left_;
    const std::unique_ptr<const Condition> right_;
    bool eval(codes_handle* h) const override { return left_->eval(h) && right_->eval(h); }

    ConditionAND(const Condition* left, const Condition* right) :
        left_(left), right_(right) {}
};


/*
class ConditionNOT : public Condition {
    const Condition* c_;
    bool eval(codes_handle* h) const  override { return !c_->eval(h); }
    ~ConditionNOT() override {
        delete c_;
    }

public:
    ConditionNOT(const Condition* c) : c_(c) {}
};
*/


void wrongly_encoded_grib(const std::string& msg)
{
    static bool abortIfWronglyEncodedGRIB = eckit::Resource<bool>("$MIR_ABORT_IF_WRONGLY_ENCODED_GRIB", false);

    if (abortIfWronglyEncodedGRIB) {
        Log::error() << msg << std::endl;
        throw eckit::UserError(msg);
    }

    Log::warning() << msg << std::endl;
}


template <class T>
Condition* is(const char* key, const T& value)
{
    return new ConditionT<T>(key, value);
}


Condition* is(const char* key, const char* value)
{
    return new ConditionT<std::string>(key, value);
}


Condition* _and(const Condition* left, const Condition* right)
{
    return new ConditionAND(left, right);
}


Condition* _or(const Condition* left, const Condition* right)
{
    return new ConditionOR(left, right);
}


/*
 Condition *_not(const Condition *c) {
    return new ConditionNOT(c);
}
*/


Condition* is_gaussian()
{
    const auto* key = "gridType";
    return _or(is(key, "reduced_gg"),
               _or(is(key, "regular_gg"),
                   _or(is(key, "reduced_rotated_gg"),
                       _or(is(key, "regular_rotated_gg"),
                           _or(is(key, "rotated_gg"),
                               _or(is(key, "reduced_stretched_gg"),
                                   _or(is(key, "regular_stretched_gg"),
                                       _or(is(key, "reduced_stretched_rotated_gg"),
                                           _or(is(key, "regular_stretched_rotated_gg"),
                                               _or(is(key, "stretched_gg"), is(key, "stretched_rotated_gg")))))))))));
}


const char* get_key(const std::string& name, codes_handle* h)
{
    struct P
    {
        const std::string name;
        const char* key;
        const std::unique_ptr<const Condition> condition;
        P(const std::string& _name, const char* _key, const Condition* _condition = nullptr) :
            name(_name), key(_key), condition(_condition) {}
    };

    static const std::initializer_list<P> mappings{
        { "type", "gridType" },

        { "west_east_increment", "iDirectionIncrementInDegrees_fix_for_periodic_regular_grids",
          is("gridType", "regular_ll") },
        { "west_east_increment", "iDirectionIncrementInDegrees" },
        { "south_north_increment", "jDirectionIncrementInDegrees" },

        { "west", "longitudeOfFirstGridPointInDegrees" },
        { "east", "longitudeOfLastGridPointInDegrees_fix_for_global_reduced_grids", is("gridType", "reduced_gg") },
        { "east", "longitudeOfLastGridPointInDegrees" },

        { "north", "latitudeOfFirstGridPointInDegrees_fix_for_gaussian_grids",
          _and(is("scanningMode", 0L), is_gaussian()) },
        { "south", "latitudeOfLastGridPointInDegrees_fix_for_gaussian_grids",
          _and(is("scanningMode", 0L), is_gaussian()) },
        { "north", "latitudeOfLastGridPointInDegrees_fix_for_gaussian_grids",
          _and(is("scanningMode", 1L), is_gaussian()) },
        { "south", "latitudeOfFirstGridPointInDegrees_fix_for_gaussian_grids",
          _and(is("scanningMode", 1L), is_gaussian()) },

        { "north", "latitudeOfLastGridPointInDegrees", is("jScansPositively", 1L) },
        { "south", "latitudeOfFirstGridPointInDegrees", is("jScansPositively", 1L) },
        { "north", "latitudeOfFirstGridPointInDegrees", is("scanningMode", 0L) },
        { "south", "latitudeOfLastGridPointInDegrees", is("scanningMode", 0L) },

        { "north", "latitudeOfLastGridPointInDegrees", is("jScansPositively", 1L) },
        { "south", "latitudeOfFirstGridPointInDegrees", is("jScansPositively", 1L) },
        { "north", "latitudeOfFirstGridPointInDegrees" },
        { "south", "latitudeOfLastGridPointInDegrees" },

        { "truncation", "pentagonalResolutionParameterJ" },  // Assumes triangular truncation
        { "accuracy", "bitsPerValue" },

        { "south_pole_latitude", "latitudeOfSouthernPoleInDegrees" },
        { "south_pole_longitude", "longitudeOfSouthernPoleInDegrees" },
        { "south_pole_rotation_angle", "angleOfRotationInDegrees" },

        { "proj", "projTargetString" },
        { "projSource", "projSourceString" },

        { "first_lat",
          "latitudeOfFirstGridPointInDegrees" },
        { "first_lon",
          "longitudeOfFirstGridPointInDegrees" },

        { "radius", "radiusInMetres" },
        { "semi_major_axis", "earthMajorAxisInMetres" },
        { "semi_minor_axis", "earthMinorAxisInMetres" },

        { "lat_0",
          "LaDInDegrees",
          _or(is("gridType", "lambert"), is("gridType", "lambert_lam")) },
        { "lon_0",
          "LoVInDegrees",
          _or(is("gridType", "lambert"), is("gridType", "lambert_lam")) },
        { "lat_1",
          "Latin1InDegrees",
          _or(is("gridType", "albers"), _or(is("gridType", "lambert"), is("gridType", "lambert_lam"))) },
        { "lat_2",
          "Latin2InDegrees",
          _or(is("gridType", "albers"), _or(is("gridType", "lambert"), is("gridType", "lambert_lam"))) },

        // This will be just called for has()
        {
            "gridded",
            "Nx",
            _or(_or(_or(is("gridType", "polar_stereographic"), is("gridType", "lambert_azimuthal_equal_area")),
                    is("gridType", "lambert")),
                is("gridType", "space_view")),
        },
        {
            "gridded",
            "Ni",
            is("gridType", "triangular_grid"),
        },
        {
            "gridded",
            "numberOfGridInReference" /*just a dummy*/,
            is("gridType", "unstructured_grid"),
        },
        { "gridded", "numberOfPointsAlongAMeridian" },  // Is that always true?
        { "gridded_regular_ll", "Ni", _or(is("gridType", "regular_ll"), is("gridType", "rotated_ll")) },
        { "gridded_named", "gridName" },

        { "grid", "gridName",
          _or(_or(_or(_or(is("gridType", "regular_gg"), is("gridType", "reduced_gg")), is("gridType", "rotated_gg")),
                  is("gridType", "reduced_rotated_gg")),
              is("gridType", "unstructured_grid")) },

        { "spectral", "pentagonalResolutionParameterJ" },

        { "uid", "uuidOfHGrid", is("gridType", "unstructured_grid") },
        { "ordering", "orderingConvention", is("gridType", "healpix") },

        /// FIXME: Find something that does no clash
        { "reduced", "numberOfParallelsBetweenAPoleAndTheEquator", is("isOctahedral", 0L) },
        { "regular", "N", is("gridType", "regular_gg") },
        { "octahedral", "numberOfParallelsBetweenAPoleAndTheEquator", is("isOctahedral", 1L) },

        /// TODO: is that a good idea?
        { "param", "paramId" },
        { "statistics", "" },  // (avoid ecCodes error "statistics: Function not yet implemented")
    };

    for (const auto& m : mappings) {
        if (name == m.name) {
            if (!m.condition || m.condition->eval(h)) {
                return m.key;
            }
        }
    }

    const auto* key = name.c_str();
    return key;
}


std::string get_string(codes_handle* h, const char* key)
{
    if (codes_is_defined(h, key) != 0) {
        char buffer[64] = {
            0,
        };
        size_t size = sizeof(buffer);

        CHECK_CALL(codes_get_string(h, key, buffer, &size));
        ASSERT(size < sizeof(buffer) - 1);

        if (std::strcmp(buffer, "MISSING") != 0) {
            return buffer;
        }
    }

    return "";
}


bool get_bool(codes_handle* h, const char* key)
{
    long l = 0;
    CHECK_CALL(codes_get_long(h, key, &l));

    return l != 0;
}


template <typename T>
struct ProcessingT
{
    using fun_t = std::function<bool(codes_handle*, T&)>;
    fun_t fun_;
    explicit ProcessingT(fun_t&& fun) :
        fun_(fun) {}
    ~ProcessingT()                     = default;
    ProcessingT(const ProcessingT&)    = delete;
    ProcessingT(ProcessingT&&)         = delete;
    void operator=(const ProcessingT&) = delete;
    void operator=(ProcessingT&&)      = delete;
    bool eval(codes_handle* h, T& v) const { return fun_(h, v); }
};


ProcessingT<double>* angular_precision()
{
    return new ProcessingT<double>([](codes_handle* h, double& value) {
        auto well_defined = [](codes_handle* h, const char* key) -> bool {
            long dummy = 0;
            int err    = 0;
            return (codes_is_defined(h, key) != 0) && (codes_is_missing(h, key, &err) == 0) && (err == CODES_SUCCESS) &&
                   (codes_get_long(h, key, &dummy) == CODES_SUCCESS) && (dummy != 0);
        };

        if (well_defined(h, "basicAngleOfTheInitialProductionDomain") && well_defined(h, "subdivisionsOfBasicAngle")) {
            value = 0.;
            return true;
        }

        long angleSubdivisions = 0;
        CHECK_CALL(codes_get_long(h, "angleSubdivisions", &angleSubdivisions));

        value = angleSubdivisions > 0 ? 1. / static_cast<double>(angleSubdivisions) : 0.;
        return true;
    });
}


ProcessingT<double>* longitudeOfLastGridPointInDegrees_fix_for_global_reduced_grids()
{
    return new ProcessingT<double>([](codes_handle* h, double& Lon2) {
        Lon2 = 0;
        CHECK_CALL(codes_get_double(h, "longitudeOfLastGridPointInDegrees", &Lon2));

        if (codes_is_defined(h, "pl") != 0) {
            double Lon1 = 0;
            CHECK_CALL(codes_get_double(h, "longitudeOfFirstGridPointInDegrees", &Lon1));

            if (eckit::types::is_approximately_equal<double>(Lon1, 0)) {
                // get pl array maximum and sum
                // if sum equals values size the grid must be global
                size_t plSize = 0;
                CHECK_CALL(codes_get_size(h, "pl", &plSize));
                ASSERT(plSize > 0);

                std::vector<long> pl(plSize, 0);
                size_t plSizeAsRead = plSize;
                CHECK_CALL(codes_get_long_array(h, "pl", pl.data(), &plSizeAsRead));
                ASSERT(plSize == plSizeAsRead);

                long plMax = 0;
                long plSum = 0;
                for (auto& p : pl) {
                    plSum += p;
                    if (plMax < p) {
                        plMax = p;
                    }
                }
                ASSERT(plMax > 0);

                size_t valuesSize = 0;
                CHECK_CALL(codes_get_size(h, "values", &valuesSize));

                if (static_cast<size_t>(plSum) == valuesSize) {
                    double eps = 0.;
                    ASSERT(std::unique_ptr<ProcessingT<double>>(angular_precision())->eval(h, eps));

                    eckit::Fraction Lon2_expected(360L * (plMax - 1L), plMax);

                    if (!eckit::types::is_approximately_greater_or_equal<double>(Lon2, Lon2_expected, eps)) {
                        std::ostringstream msgs;
                        msgs.precision(32);
                        msgs << "GribParametrisation: wrongly encoded longitudeOfLastGridPointInDegrees:"
                             << "\n"
                                "encoded:  "
                             << Lon2
                             << "\n"
                                "expected: "
                             << static_cast<double>(Lon2_expected) << " (" << Lon2_expected << " +- " << eps << ")";

                        wrongly_encoded_grib(msgs.str());
                    }

                    Lon2 = 360.;
                }
            }
        }

        return true;
    });
};


ProcessingT<double>* latitudeOfFirstGridPointInDegrees_fix_for_gaussian_grids()
{
    return new ProcessingT<double>([](codes_handle* h, double& lat) {
        double eps = 0.;
        ASSERT(std::unique_ptr<ProcessingT<double>>(angular_precision())->eval(h, eps));

        long N = 0L;
        CHECK_CALL(codes_get_long(h, "N", &N));
        ASSERT(N > 0);

        double& Lat1 = lat;
        CHECK_CALL(codes_get_double(h, "latitudeOfFirstGridPointInDegrees", &Lat1));

        double Lat2 = 0.;
        CHECK_CALL(codes_get_double(h, "latitudeOfLastGridPointInDegrees", &Lat2));

        const auto snap = eckit::geo::util::gaussian_latitudes(N, Lat1 < Lat2).front();
        if (eckit::types::is_approximately_equal(Lat1, snap, eps)) {
            lat = snap < 0. ? eckit::geo::SOUTH_POLE.lat : eckit::geo::NORTH_POLE.lat;
        }

        return true;
    });
}


ProcessingT<double>* latitudeOfLastGridPointInDegrees_fix_for_gaussian_grids()
{
    return new ProcessingT<double>([](codes_handle* h, double& lat) {
        double eps = 0.;
        ASSERT(std::unique_ptr<ProcessingT<double>>(angular_precision())->eval(h, eps));

        long N = 0L;
        CHECK_CALL(codes_get_long(h, "N", &N));
        ASSERT(N > 0);

        double Lat1 = 0.;
        CHECK_CALL(codes_get_double(h, "latitudeOfFirstGridPointInDegrees", &Lat1));

        double& Lat2 = lat;
        CHECK_CALL(codes_get_double(h, "latitudeOfLastGridPointInDegrees", &Lat2));

        const auto snap = eckit::geo::util::gaussian_latitudes(N, Lat1 < Lat2).back();
        if (eckit::types::is_approximately_equal(Lat2, snap, eps)) {
            lat = snap < 0. ? eckit::geo::SOUTH_POLE.lat : eckit::geo::NORTH_POLE.lat;
        }

        return true;
    });
}


ProcessingT<double>* iDirectionIncrementInDegrees_fix_for_periodic_regular_grids()
{
    return new ProcessingT<double>([](codes_handle* h, double& we) {
        long iScansPositively = 0L;
        CHECK_CALL(codes_get_long(h, "iScansPositively", &iScansPositively));
        ASSERT(iScansPositively == 1L);

        CHECK_CALL(codes_get_double(h, "iDirectionIncrementInDegrees", &we));
        ASSERT(we > 0.);

        double Lon1 = 0.;
        double Lon2 = 0.;
        long Ni     = 0;
        CHECK_CALL(codes_get_double(h, "longitudeOfFirstGridPointInDegrees", &Lon1));
        CHECK_CALL(codes_get_double(h, "longitudeOfLastGridPointInDegrees", &Lon2));
        CHECK_CALL(codes_get_long(h, "Ni", &Ni));
        ASSERT(Ni > 0);

        Lon2 = eckit::geo::PointLonLat::normalise_angle_to_minimum(Lon2, Lon1);
        ASSERT(Lon2 >= Lon1);

        // angles are within +-1/2 precision, so (Lon2 - Lon1 + we) uses factor 3*1/2
        double eps = 0.;
        ASSERT(std::unique_ptr<ProcessingT<double>>(angular_precision())->eval(h, eps));
        eps *= 1.5;

        constexpr double GLOBE = 360;

        auto Nid = static_cast<double>(Ni);
        if (eckit::types::is_approximately_equal(Lon2 - Lon1 + we, GLOBE, eps)) {
            we = GLOBE / Nid;
        }
        else if (!eckit::types::is_approximately_equal(Lon1 + (Nid - 1) * we, Lon2, eps)) {
            // TODO refactor, not really specific to "periodic regular grids", but useful
            std::ostringstream msgs;
            msgs.precision(32);
            msgs << "GribParametrisation: wrongly encoded iDirectionIncrementInDegrees:"
                    "\n"
                    "encoded: "
                 << we
                 << "\n"
                    "Ni: "
                 << Ni
                 << "\n"
                    "longitudeOfFirstGridPointInDegree: "
                 << Lon1
                 << "\n"
                    "longitudeOfLastGridPointInDegrees: "
                 << Lon2;

            wrongly_encoded_grib(msgs.str());
        }

        return true;
    });
};


ProcessingT<std::vector<double>>* vector_double(std::initializer_list<std::string> keys)
{
    const std::vector<std::string> keys_(keys);
    return new ProcessingT<std::vector<double>>([=](codes_handle* h, std::vector<double>& values) {
        ASSERT(keys.size());

        values.assign(keys_.size(), 0);
        size_t i = 0;
        for (const auto& key : keys_) {
            if (codes_is_defined(h, key.c_str()) == 0) {
                return false;
            }
            CHECK_CALL(codes_get_double(h, key.c_str(), &values[i++]));
        }
        return true;
    });
}


ProcessingT<std::string>* order()
{
    return new ProcessingT<std::string>([](codes_handle* h, std::string& value) {
        if (auto gridType = get_string(h, "gridType"); gridType == "healpix") {
            value = get_string(h, "orderingConvention");
            return true;
        }

        // scanningMode
        static const std::string P{ "+" };
        static const std::string M{ "-" };

        const auto ip = get_bool(h, "iScansPositively");
        const auto jp = get_bool(h, "jScansPositively");
        const auto a  = get_bool(h, "alternativeRowScanning");

        value = get_bool(h, "jPointsAreConsecutive") ? "j" + (jp ? P : M) + (!a ? "" : (jp ? M : P)) + "i" + (ip ? P : M)
                                                     : "i" + (ip ? P : M) + (!a ? "" : (ip ? M : P)) + "j" + (jp ? P : M);

        return true;
    });
}


ProcessingT<std::string>* packing()
{
    return new ProcessingT<std::string>([](codes_handle* h, std::string& value) {
        auto packingType = get_string(h, "packingType");
        for (const auto& prefix : std::vector<std::string>{ "grid_", "spectral_" }) {
            if (packingType.find(prefix) == 0) {
                value = packingType.substr(prefix.size());
                std::replace(value.begin(), value.end(), '_', '-');
                return true;
            }
        }

        return false;
    });
}


template <class T>
struct ConditionedProcessingT
{
    const std::string name;
    const std::unique_ptr<const T> processing;
    const std::unique_ptr<const Condition> condition;
    ConditionedProcessingT(const std::string& _name, const T* _processing, const Condition* _condition = nullptr) :
        name(_name), processing(_processing), condition(_condition) {}
};


template <class T>
using ProcessingList = std::initializer_list<ConditionedProcessingT<ProcessingT<T>>>;


template <typename T>
bool get_value(const std::string& name, codes_handle* h, T& value, const ProcessingList<T>& process)
{
    for (auto& p : process) {
        if (name == p.name) {
            if (!p.condition || p.condition->eval(h)) {
                ASSERT(p.processing);
                return p.processing->eval(h, value);
            }
        }
    }
    return false;
}


eckit::geo::util::recursive_mutex MUTEX;


class lock_type
{
    eckit::geo::util::lock_guard<eckit::geo::util::recursive_mutex> lock_guard_{ MUTEX };
};


}  // namespace


GribToSpec::GribToSpec(codes_handle* h) :
    handle_(h)
{
    ASSERT(handle_ != nullptr);
}


bool GribToSpec::has(const std::string& name) const
{
    lock_type lock;

    if (cache_.has(name)) {
        return true;
    }

    const auto* key = get_key(name, handle_);

    ASSERT(key != nullptr);
    if (std::strlen(key) == 0) {
        return false;
    }

    return codes_is_defined(handle_, key) != 0;
}


bool GribToSpec::get(const std::string& name, std::string& value) const
{
    lock_type lock;

    if (cache_.get(name, value)) {
        return true;
    }

    const auto* key = get_key(name, handle_);

    ASSERT(key != nullptr);
    if (std::strlen(key) == 0) {
        return false;
    }

    char buffer[10240];
    size_t size = sizeof(buffer);
    int err     = codes_get_string(handle_, key, buffer, &size);

    if (err == CODES_NOT_FOUND) {
        static const ProcessingList<std::string> process{
            { "order", order() },
            { "packing", packing() },
        };

        return get_value(key, handle_, value, process);
    }

    CHECK_ERROR(err, key);

    ASSERT(size < sizeof(buffer) - 1);

    if (std::strcmp(buffer, "MISSING") == 0) {
        return false;
    }

    cache_.set(name, value = buffer);
    return true;
}


bool GribToSpec::get(const std::string& name, bool& value) const
{
    lock_type lock;

    if (cache_.get(name, value)) {
        return true;
    }

    const auto* key = get_key(name, handle_);

    ASSERT(key != nullptr);
    if (std::strlen(key) == 0) {
        return false;
    }

    // FIXME: make sure that 'temp' is not set if CODES_MISSING_LONG
    long temp = CODES_MISSING_LONG;
    int err   = codes_get_long(handle_, key, &temp);
    CHECK_ERROR(err, key);

    cache_.set(name, value = temp != 0);
    return true;
}


bool GribToSpec::get(const std::string& name, int& value) const
{
    if (long v = 0; get(name, v)) {
        ASSERT(static_cast<long>(static_cast<int>(v)) == v);
        value = static_cast<int>(v);
        return true;
    }

    return false;
}


bool GribToSpec::get(const std::string& name, long& value) const
{
    lock_type lock;

    if (cache_.get(name, value)) {
        return true;
    }

    const std::string key = get_key(name, handle_);
    if (key.empty()) {
        return false;
    }

    // FIXME: make sure that 'value' is not set if CODES_MISSING_LONG
    int err = codes_get_long(handle_, key.c_str(), &value);
    if (err == CODES_NOT_FOUND || codes_is_missing(handle_, key.c_str(), &err) != 0) {
        return false;
    }

    CHECK_ERROR(err, key.c_str());

    cache_.set(name, value);
    return true;
}


bool GribToSpec::get(const std::string& /*name*/, long long& /*value*/) const
{
    return false;
}


bool GribToSpec::get(const std::string& /*name*/, std::size_t& /*value*/) const
{
    return false;
}


bool GribToSpec::get(const std::string& name, float& value) const
{
    if (cache_.get(name, value)) {
        return true;
    }

    if (double v = 0; get(name, v)) {
        cache_.set(name, value = static_cast<float>(v));
        return true;
    }

    return false;
}


bool GribToSpec::get(const std::string& name, double& value) const
{
    lock_type lock;

    if (cache_.get(name, value)) {
        return true;
    }

    ASSERT(name != "grid");

    const auto* key = get_key(name, handle_);

    ASSERT(key != nullptr);
    if (std::strlen(key) == 0) {
        return false;
    }

    // FIXME: make sure that 'value' is not set if CODES_MISSING_DOUBLE
    int err = codes_get_double(handle_, key, &value);
    if (err == CODES_NOT_FOUND || codes_is_missing(handle_, key, &err) != 0) {
        static const ProcessingList<double> process{
            { "angular_precision", angular_precision() },
            { "longitudeOfLastGridPointInDegrees_fix_for_global_reduced_grids",
              longitudeOfLastGridPointInDegrees_fix_for_global_reduced_grids() },
            { "iDirectionIncrementInDegrees_fix_for_periodic_regular_grids",
              iDirectionIncrementInDegrees_fix_for_periodic_regular_grids() },
            { "latitudeOfFirstGridPointInDegrees_fix_for_gaussian_grids",
              latitudeOfFirstGridPointInDegrees_fix_for_gaussian_grids() },
            { "latitudeOfLastGridPointInDegrees_fix_for_gaussian_grids",
              latitudeOfLastGridPointInDegrees_fix_for_gaussian_grids() },
        };

        if (get_value(key, handle_, value, process)) {
            cache_.set(name, value);
            return true;
        }

        return false;
    }

    CHECK_ERROR(err, key);

    cache_.set(name, value);
    return true;
}


bool GribToSpec::get(const std::string& /*name*/, std::vector<int>& /*value*/) const
{
    return false;
}


bool GribToSpec::get(const std::string& name, std::vector<long>& value) const
{
    lock_type lock;

    if (cache_.get(name, value)) {
        return true;
    }

    const auto* key = get_key(name, handle_);

    ASSERT(key != nullptr);
    if (std::strlen(key) == 0) {
        return false;
    }

    size_t count = 0;
    int err      = codes_get_size(handle_, key, &count);
    CHECK_ERROR(err, key);

    size_t size = count;

    value.resize(count);

    CHECK_CALL(codes_get_long_array(handle_, key, value.data(), &size));
    ASSERT(count == size);

    ASSERT(!value.empty());

    if (name == "pl") {
        if (std::find(value.rbegin(), value.rend(), 0) != value.rend()) {
            wrongly_encoded_grib("GribParametrisation: pl array contains zeros");
        }
    }

    cache_.set(name, value);
    return true;
}


bool GribToSpec::get(const std::string& /*name*/, std::vector<long long>& /*value*/) const
{
    return false;
}


bool GribToSpec::get(const std::string& /*name*/, std::vector<std::size_t>& /*value*/) const
{
    return false;
}


bool GribToSpec::get(const std::string& name, std::vector<float>& value) const
{
    if (cache_.get(name, value)) {
        return true;
    }

    if (std::vector<double> v; get(name, v)) {
        value.clear();
        value.reserve(v.size());
        for (const auto& d : v) {
            value.push_back(static_cast<float>(d));
        }

        cache_.set(name, value);
        return true;
    }

    return false;
}


bool GribToSpec::get(const std::string& name, std::vector<double>& value) const
{
    lock_type lock;

    if (cache_.get(name, value)) {
        return true;
    }

    const auto* key = get_key(name, handle_);

    // NOTE: MARS client sets 'grid=vector' (deprecated) which needs to be compared against GRIB gridName
    ASSERT(key != nullptr);
    if (std::strlen(key) == 0 || std::strncmp(key, "gridName", 8) == 0) {
        return false;
    }

    static const ProcessingList<std::vector<double>> process{
        { "grid", vector_double({ "iDirectionIncrementInDegrees", "jDirectionIncrementInDegrees" }),
          _or(is("gridType", "regular_ll"), is("gridType", "rotated_ll")) },
        { "grid", vector_double({ "xDirectionGridLengthInMetres", "yDirectionGridLengthInMetres" }),
          is("gridType", "lambert_azimuthal_equal_area") },
        { "grid", vector_double({ "DxInMetres", "DyInMetres" }),
          _or(is("gridType", "lambert"), is("gridType", "polar_stereographic")) },
        { "grid", vector_double({ "DiInMetres", "DjInMetres" }), is("gridType", "mercator") },
        { "grid", vector_double({ "dx", "dy" }), is("gridType", "space_view") },
        { "rotation", vector_double({ "latitudeOfSouthernPoleInDegrees", "longitudeOfSouthernPoleInDegrees" }),
          _or(_or(_or(is("gridType", "rotated_ll"), is("gridType", "rotated_gg")), is("gridType", "rotated_sh")),
              is("gridType", "reduced_rotated_gg")) },
    };

    if (get_value(key, handle_, value, process)) {
        cache_.set(name, value);
        return true;
    }

    // FIXME make logic consistent for ::get(,*)
    if (codes_is_defined(handle_, key) == 0) {
        return false;
    }

    size_t count = 0;
    int err      = codes_get_size(handle_, key, &count);
    CHECK_ERROR(err, key);

    ASSERT(count > 0);
    size_t size = count;

    value.resize(count);

    CHECK_CALL(codes_get_double_array(handle_, key, value.data(), &size));
    ASSERT(count == size);

    ASSERT(!value.empty());

    cache_.set(name, value);
    return true;
}


bool GribToSpec::get(const std::string& /*name*/, std::vector<std::string>& /*value*/) const
{
    return false;
}


void GribToSpec::json(eckit::JSON& j) const
{
    j.startObject();

    auto* kit = codes_keys_iterator_new(
        handle_, CODES_KEYS_ITERATOR_SKIP_DUPLICATES | CODES_KEYS_ITERATOR_SKIP_READ_ONLY, "geography");
    ASSERT(kit != nullptr);

    for (; codes_keys_iterator_next(kit) != 0;) {
        const auto* name = codes_keys_iterator_get_name(kit);

        int err         = 0;
        bool is_missing = 0 != codes_is_missing(handle_, name, &err);
        CHECK_ERROR(err, name);

        if (is_missing) {
            j << name << "MISSING";
            continue;
        }

        size_t size = 0;
        CHECK_CALL(codes_get_size(handle_, name, &size));

        int type = 0;
        CHECK_CALL(codes_get_native_type(handle_, name, &type));

        if (size > 1) {
            if (type == CODES_TYPE_LONG) {
                std::vector<long> array(size);

                auto size_read = size;
                CHECK_CALL(codes_get_long_array(handle_, name, array.data(), &size_read));
                ASSERT(size == size_read);

                j << name;
                j.startList();
                for (const auto& value : array) {
                    j << value;
                }
                j.endList();
                continue;
            }

            if (type == CODES_TYPE_DOUBLE) {
                std::vector<double> array(size);

                auto size_read = size;
                CHECK_CALL(codes_get_double_array(handle_, name, array.data(), &size_read));
                ASSERT(size == size_read);

                j << name;
                j.startList();
                for (const auto& value : array) {
                    j << value;
                }
                j.endList();
                continue;
            }

            (j << name).startList().endList();
            continue;
        }

        if (type == CODES_TYPE_LONG) {
            long value = 0;
            CHECK_CALL(codes_get_long(handle_, name, &value));
            j << name << value;
            continue;
        }

        if (type == CODES_TYPE_DOUBLE) {
            double value = 0;
            CHECK_CALL(codes_get_double(handle_, name, &value));
            j << name << value;
            continue;
        }

        if (type == CODES_TYPE_STRING) {
            char value[1024] = {
                0,
            };
            size_t length = sizeof(value);
            CHECK_CALL(codes_get_string(handle_, name, value, &length));
            j << name << value;
            continue;
        }

        j << name
          << (type == CODES_TYPE_UNDEFINED ? "UNDEFINED"
              : type == CODES_TYPE_BYTES   ? "BYTES"
              : type == CODES_TYPE_SECTION ? "SECTION"
              : type == CODES_TYPE_LABEL   ? "LABEL"
              : type == CODES_TYPE_MISSING ? "MISSING"
                                           : "?");
    }

    codes_keys_iterator_delete(kit);
    j.endObject();
}


#undef CHECK_ERROR
#undef CHECK_CALL


}  // namespace eccodes::geo
