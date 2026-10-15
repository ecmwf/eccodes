#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

if [ $HAVE_GEOGRAPHY -ne 1 ] || [ $HAVE_ECKIT_GEO -ne 1 ]; then
    echo "$0: This test is disabled when HAVE_GEOGRAPHY=OFF or HAVE_ECKIT_GEO=OFF"
    exit 0
fi

label="grib_eckit_geo_projections_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempText=temp.$label.txt

# Check a key, by ECCODES_ECKIT_GEO level ("-" if not available)
check() {
    result=$(ECCODES_ECKIT_GEO=$1 ${tools_dir}/grib_get -p $2 $tempGrib 2>/dev/null) || result="-"
    if [ "$result" != "$3" ]; then
        echo "ERROR: $2 (ECCODES_ECKIT_GEO=$1): '$result', expected '$3'" >&2
        exit 1
    fi
}

# The projection spec, of the grid by eckit::geo from gridSpec
projection() {
    ECCODES_ECKIT_GEO=2 ${test_dir}/grib_eckit_geo_projection $tempGrib
}

# The first, second and last grid points by eckit::geo, as the reference
points() {
    ECCODES_ECKIT_GEO=2 ${tools_dir}/grib_get_data -L "%.9f %.9f" $tempGrib | sed -n '2p;3p;$p' > $tempText
    paste $tempText.ref $tempText | awk -v n=$(awk '{ print NF; exit }' $tempText.ref) '{
        if ($(n + 1) == "nan") { if ($1 != 0 || $2 != 0) { print "ERROR: " $0; exit 1 } next }
        dlat = $1 - $(n + 1);
        dlon = $2 - $(n + 2); dlon -= 360 * int(dlon / 360 + (dlon < 0 ? -0.5 : 0.5));
        if (dlat * dlat > 1e-12 || dlon * dlon > 1e-12) { print "ERROR: " $0; exit 1 } }'
}

# Grids by grib_filter (keys set in order) from a sample, or a test data message (count=N, then the keys).
# Columns:
# - gridSpec ("-" if not supported); projection spec of the gridSpec grid
# - projString
# - projSourceString
# - first, second, last grid points by eckit::geo: as the eccodes iterator ("legacy"), as given, or not compared ("-")
#   - in microdegrees, longitudes modulo 360
#   - not visible (space_view): "0 0" (eccodes iterator), "nan nan" (eckit::geo)
# Iterator implementation differences:
# - regular Gaussian: not unrotated; unrotated points rounded (a few microdegrees)
# - Lambert: latitudes assumed increasing from the first grid point (GRIB-405)
#   given by PROJ and by the rotation as the eccodes iterator, without rounding
# - space_view, scanning not respected: listed W-E, N-S regardless of placement
#   given as the eccodes iterator points, in the scanning order
# Projected grids: on the projection plane; false easting/northing puts the first grid point at (0, 0), to millimetres.
# Example grids (smaller):
# - lambert: lambert_lam.grib2 (Belgium, 2.5 km); PFFPOS000+0000_guyane.grb (French Guiana, 8 km);
#   INCA (GRIB1, oblate, two standard parallels); oblate.belgium.grib2; AROME (LaD != Latin1/2);
#   CARRA (polar); WRF (GRIB1, southern); t_arome (GRIB1, scanning j-); NOAA NDFD (alternativeRowScanning)
# - albers: oblate; points as PROJ aea (no eccodes iterator)
# - mercator: mercator.grib2
# - polar_stereographic: CMC_hrdps_east
# - lambert_azimuthal_equal_area: opera.grib2
# - space_view: MSG; h03 (oblate); Himawari (GRIB1, oblate);
#   test data messages as windows on the Earth disc (full disc corners not visible):
#   cnmc, EUMETSAT (scanning i-j+)
while IFS='|' read -r input keys gridSpec projString1 projString2 points; do
    case $keys in
        count=*)
            ${tools_dir}/grib_copy -w ${keys%%,*} ${data_dir}/$input $tempGrib.in
            keys=$(echo "$keys" | sed -n 's/^[^,]*,//p')
            ;;
        *)
            cp $ECCODES_SAMPLES_PATH/$input $tempGrib.in
            ;;
    esac
    echo "$keys" | tr ',' '\n' | sed '/^$/d; s/^\(.*\)$/set \1;/' > $tempFilt
    echo 'write;' >> $tempFilt
    ${tools_dir}/grib_filter -o $tempGrib $tempFilt $tempGrib.in

    check 2 gridSpec "$gridSpec"
    check 0 projString "$projString1"
    check 1 projString "$projString2"
    check 2 projString "$projString2"
    if [ "$projString2" != "-" ]; then
        check 2 projSourceString "$(echo "$projString2" | awk '{
            s = "+proj=longlat"; for (i = 1; i <= NF; i++) if ($i ~ /^\+(R|a|b|ellps)=/) s = s " " $i; print s }')"
    fi

    if [ "$gridSpec" != "-" ]; then
        projection=$(projection)
        case $gridSpec in
            *"\"projection\":$projection"*) ;;
            *) echo "ERROR: projection '$projection' not as gridSpec" >&2; exit 1 ;;
        esac
    fi

    if [ "$points" != "-" ]; then
        if [ "$points" = "legacy" ]; then
            ECCODES_ECKIT_GEO=0 ${tools_dir}/grib_get_data -L "%.9f %.9f" $tempGrib | sed -n '2p;3p;$p' > $tempText.ref
        else
            echo "$points" | tr ';' '\n' > $tempText.ref
        fi
        points
    fi
    rm -f $tempGrib $tempGrib.in $tempText.ref
done <<'GRIDS'
GRIB2.tmpl|gridType="lambert",Nx=49,Ny=69,values={1},numberOfDataPoints=3381,numberOfValues=3381,shapeOfTheEarth=6,latitudeOfFirstGridPointInDegrees=51.199433,longitudeOfFirstGridPointInDegrees=4.038959,LaDInDegrees=51.967,LoVInDegrees=4.9,Latin1InDegrees=51.967,Latin2InDegrees=51.967,DxInMetres=2500,DyInMetres=2500,jScansPositively=1|{"false":[60000.021,85000.055],"order":"i+j+","projection":{"lat_0":51.967,"lat_1":51.967,"lon_0":4.9,"reverse":true,"type":"lcc"},"x":[0,120000,2500],"y":[0,170000,2500]}|+proj=lcc +lon_0=4.900000 +lat_0=51.967000 +lat_1=51.967000 +lat_2=51.967000 +R=6371229.000000|+proj=lcc +lat_0=51.967 +lat_1=51.967 +lon_0=4.9 +R=6371229|legacy
GRIB2.tmpl|gridType="lambert_lam",Nx=49,Ny=69,values={1},numberOfDataPoints=3381,numberOfValues=3381,shapeOfTheEarth=6,latitudeOfFirstGridPointInDegrees=51.199433,longitudeOfFirstGridPointInDegrees=4.038959,LaDInDegrees=51.967,LoVInDegrees=4.9,Latin1InDegrees=51.967,Latin2InDegrees=51.967,DxInMetres=2500,DyInMetres=2500,jScansPositively=1|{"false":[60000.021,85000.055],"order":"i+j+","projection":{"lat_0":51.967,"lat_1":51.967,"lon_0":4.9,"reverse":true,"type":"lcc"},"x":[0,120000,2500],"y":[0,170000,2500]}|-|+proj=lcc +lat_0=51.967 +lat_1=51.967 +lon_0=4.9 +R=6371229|legacy
GRIB2.tmpl|gridType="lambert",Nx=181,Ny=139,values={1},numberOfDataPoints=25159,numberOfValues=25159,shapeOfTheEarth=6,latitudeOfFirstGridPointInDegrees=10.044953,longitudeOfFirstGridPointInDegrees=302.098546,LaDInDegrees=5.12,LoVInDegrees=308.65,Latin1InDegrees=5.12,Latin2InDegrees=5.12,DxInMetres=8000,DyInMetres=8000|{"false":[720000.044,-552000.009],"projection":{"lat_0":5.12,"lat_1":5.12,"lon_0":308.65,"reverse":true,"type":"lcc"},"x":[0,1440000,8000],"y":[0,-1104000,-8000]}|+proj=lcc +lon_0=308.650000 +lat_0=5.120000 +lat_1=5.120000 +lat_2=5.120000 +R=6371229.000000|+proj=lcc +lat_0=5.12 +lat_1=5.12 +lon_0=308.65 +R=6371229|10.044953002 302.098546001;10.045680339 302.171334960;0.129704986 315.100527634
GRIB1.tmpl|gridType="lambert",Nx=31,Ny=29,values={1},earthIsOblate=1,latitudeOfFirstGridPointInDegrees=47.896,longitudeOfFirstGridPointInDegrees=0.492,LoVInDegrees=4.359,Latin1InDegrees=49.833,Latin2InDegrees=51.167,DxInMetres=1000,DyInMetres=1000,jScansPositively=1|{"false":[289299.535,207955.983],"order":"i+j+","projection":{"figure":"iau1965","lat_0":49.833,"lat_1":49.833,"lat_2":51.167,"lon_0":4.359,"reverse":true,"type":"lcc"},"x":[0,30000,1000],"y":[0,28000,1000]}|+proj=lcc +lon_0=4.359000 +lat_0=49.833000 +lat_1=49.833000 +lat_2=51.167000 +a=6378160.000000 +b=6356775.000000|+proj=lcc +lat_0=49.833 +lat_1=49.833 +lat_2=51.167 +lon_0=4.359 +a=6378160 +b=6356775|legacy
GRIB2.tmpl|gridType="lambert",Nx=31,Ny=29,values={1},numberOfDataPoints=899,numberOfValues=899,shapeOfTheEarth=2,latitudeOfFirstGridPointInDegrees=47.896,longitudeOfFirstGridPointInDegrees=0.492,LaDInDegrees=49.833,LoVInDegrees=4.359,Latin1InDegrees=49.833,Latin2InDegrees=51.167,DxInMetres=1000,DyInMetres=1000,jScansPositively=1|{"false":[289299.535,207955.983],"order":"i+j+","projection":{"figure":"iau1965","lat_0":49.833,"lat_1":49.833,"lat_2":51.167,"lon_0":4.359,"reverse":true,"type":"lcc"},"x":[0,30000,1000],"y":[0,28000,1000]}|+proj=lcc +lon_0=4.359000 +lat_0=49.833000 +lat_1=49.833000 +lat_2=51.167000 +a=6378160.000000 +b=6356775.000000|+proj=lcc +lat_0=49.833 +lat_1=49.833 +lat_2=51.167 +lon_0=4.359 +a=6378160 +b=6356775|legacy
GRIB2.tmpl|gridType="lambert",Nx=31,Ny=29,values={1},numberOfDataPoints=899,numberOfValues=899,shapeOfTheEarth=6,latitudeOfFirstGridPointInDegrees=40.442,longitudeOfFirstGridPointInDegrees=353.559,LaDInDegrees=60,LoVInDegrees=2.2,Latin1InDegrees=46.401,Latin2InDegrees=46.401,DxInMetres=2499,DyInMetres=2499|{"false":[733660.317,2151382.887],"projection":{"lat_0":60,"lat_1":46.401,"lon_0":2.2,"reverse":true,"type":"lcc"},"x":[0,74970,2499],"y":[0,-69972,-2499]}|+proj=lcc +lon_0=2.200000 +lat_0=60.000000 +lat_1=46.401000 +lat_2=46.401000 +R=6371229.000000|+proj=lcc +lat_0=60 +lat_1=46.401 +lon_0=2.2 +R=6371229|40.442000000 -6.441000000;40.444432697 -6.411799273;39.888697201 -5.484135844
GRIB2.tmpl|gridType="lambert",Nx=31,Ny=29,values={1},numberOfDataPoints=899,numberOfValues=899,shapeOfTheEarth=6,latitudeOfFirstGridPointInDegrees=70.135,longitudeOfFirstGridPointInDegrees=340.592,LaDInDegrees=80,LoVInDegrees=326,Latin1InDegrees=80,Latin2InDegrees=80,DxInMetres=2500,DyInMetres=2500,jScansPositively=1|{"false":[-552217.004,1031959.078],"order":"i+j+","projection":{"lat_0":80,"lat_1":80,"lon_0":326,"reverse":true,"type":"lcc"},"x":[0,75000,2500],"y":[0,70000,2500]}|+proj=lcc +lon_0=326.000000 +lat_0=80.000000 +lat_1=80.000000 +lat_2=80.000000 +R=6371229.000000|+proj=lcc +lat_0=80 +lat_1=80 +lon_0=326 +R=6371229|legacy
GRIB1.tmpl|gridType="lambert",Nx=31,Ny=29,values={1},latitudeOfFirstGridPointInDegrees=-54.387,longitudeOfFirstGridPointInDegrees=265.669,LoVInDegrees=295,Latin1InDegrees=-35,Latin2InDegrees=-35,DxInMetres=4000,DyInMetres=4000,projectionCentreFlag=128,jScansPositively=1|{"false":[1995214.954,2495030.718],"order":"i+j+","projection":{"lat_0":-35,"lat_1":-35,"lon_0":295,"reverse":true,"type":"lcc"},"x":[0,120000,4000],"y":[0,112000,4000]}|+proj=lcc +lon_0=295.000000 +lat_0=-35.000000 +lat_1=-35.000000 +lat_2=-35.000000 +R=6367470.000000|+proj=lcc +lat_0=-35 +lat_1=-35 +lon_0=295 +R=6367470|legacy
GRIB1.tmpl|gridType="lambert",Nx=31,Ny=29,values={1},latitudeOfFirstGridPointInDegrees=37.333,longitudeOfFirstGridPointInDegrees=-8.354,LoVInDegrees=2,Latin1InDegrees=45.8,Latin2InDegrees=45.8,DxInMetres=2500,DyInMetres=2500,jScansPositively=0|{"false":[921962.126,884482.74],"projection":{"lat_0":45.8,"lat_1":45.8,"lon_0":2,"reverse":true,"type":"lcc"},"x":[0,75000,2500],"y":[0,-70000,-2500]}|+proj=lcc +lon_0=2.000000 +lat_0=45.800000 +lat_1=45.800000 +lat_2=45.800000 +R=6367470.000000|+proj=lcc +lat_0=45.8 +lat_1=45.8 +lon_0=2 +R=6367470|37.333000000 -8.354000000;37.335872180 -8.326235857;36.797231872 -7.427630540
GRIB2.tmpl|gridType="lambert",Nx=31,Ny=29,values={1},numberOfDataPoints=899,numberOfValues=899,shapeOfTheEarth=1,scaleFactorOfRadiusOfSphericalEarth=0,scaledValueOfRadiusOfSphericalEarth=6371200,latitudeOfFirstGridPointInDegrees=20.192,longitudeOfFirstGridPointInDegrees=238.446,LaDInDegrees=25,LoVInDegrees=265,Latin1InDegrees=25,Latin2InDegrees=25,DxInMetres=2539.703,DyInMetres=2539.703,jScansPositively=1,alternativeRowScanning=1|{"false":[2763204.375,263789.38],"order":"i+-j+","projection":{"figure":"wgs84_sphere","lat_0":25,"lat_1":25,"lon_0":265,"reverse":true,"type":"lcc"},"x":[0,76191.09,2539.703],"y":[0,71111.684,2539.703]}|+proj=lcc +lon_0=265.000000 +lat_0=25.000000 +lat_1=25.000000 +lat_2=25.000000 +R=6371200.000000|+proj=lcc +lat_0=25 +lat_1=25 +lon_0=265 +R=6371200|legacy
GRIB2.tmpl|gridType="mercator",Ni=56,Nj=44,values={1},numberOfDataPoints=2464,numberOfValues=2464,shapeOfTheEarth=6,latitudeOfFirstGridPointInDegrees=14.736453,longitudeOfFirstGridPointInDegrees=262.036499,latitudeOfLastGridPointInDegrees=31.173066,longitudeOfLastGridPointInDegrees=284.975289,LaDInDegrees=14,DiInMetres=45000,DjInMetres=45000,jScansPositively=1|{"false":[10569854.248,-1607827.789],"order":"i+j+","projection":{"lat_ts":14,"reverse":true,"type":"merc"},"x":[0,2475000,45000],"y":[0,1935000,45000]}|+proj=merc +lat_ts=14.000000 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +R=6371229.000000|+proj=merc +lat_ts=14 +R=6371229|legacy
GRIB2.tmpl|gridType="albers",Nx=31,Ny=29,values={1},numberOfDataPoints=899,numberOfValues=899,shapeOfTheEarth=4,latitudeOfFirstGridPointInDegrees=23,longitudeOfFirstGridPointInDegrees=264,LaDInDegrees=23,LoVInDegrees=264,Latin1InDegrees=29.5,Latin2InDegrees=45.5,DxInMetres=50000,DyInMetres=50000,jScansPositively=1|{"order":"i+j+","projection":{"figure":"grs80","lat_0":23,"lat_1":29.5,"lat_2":45.5,"lon_0":264,"reverse":true,"type":"aea"},"x":[0,1500000,50000],"y":[0,1400000,50000]}|+proj=aea +lon_0=264.000000 +lat_0=23.000000 +lat_1=29.500000 +lat_2=45.500000 +a=6378137.000000 +b=6356752.314000|+proj=aea +lat_0=23 +lat_1=29.5 +lat_2=45.5 +lon_0=264 +ellps=GRS80|23.000000000 264.000000000;22.998841507 264.478562795;34.484152764 280.544471606
GRIB2.tmpl|gridType="polar_stereographic",Nx=40,Ny=30,values={1},numberOfDataPoints=1200,numberOfValues=1200,shapeOfTheEarth=6,latitudeOfFirstGridPointInDegrees=38.7017,longitudeOfFirstGridPointInDegrees=268.656832,LaDInDegrees=60,orientationOfTheGridInDegrees=280,DxInMetres=50000,DyInMetres=50000,jScansPositively=1|{"false":[1122822.896,5597229.258],"order":"i+j+","projection":{"lat_0":90,"lat_ts":60,"lon_0":280,"reverse":true,"type":"stere"},"x":[0,1950000,50000],"y":[0,1450000,50000]}|+proj=stere +lat_ts=60.000000 +lat_0=90 +lon_0=280.000000 +k_0=1 +x_0=0 +y_0=0 +R=6371229.000000|+proj=stere +lat_0=90 +lat_ts=60 +lon_0=280 +R=6371229|legacy
GRIB2.tmpl|gridType="polar_stereographic",Nx=20,Ny=20,values={1},numberOfDataPoints=400,numberOfValues=400,shapeOfTheEarth=6,projectionCentreFlag=128,latitudeOfFirstGridPointInDegrees=-60,longitudeOfFirstGridPointInDegrees=315,LaDInDegrees=-71,orientationOfTheGridInDegrees=0,DxInMetres=100000,DyInMetres=100000,jScansPositively=1|{"false":[2348529.672,-2348529.672],"order":"i+j+","projection":{"lat_0":-90,"lat_ts":-71,"reverse":true,"type":"stere"},"x":[0,1900000,100000],"y":[0,1900000,100000]}|+proj=stere +lat_ts=-71.000000 +lat_0=-90 +lon_0=0.000000 +k_0=1 +x_0=0 +y_0=0 +R=6371229.000000|+proj=stere +lat_0=-90 +lat_ts=-71 +R=6371229|legacy
GRIB2.tmpl|gridType="lambert_azimuthal_equal_area",Nx=20,Ny=19,values={1},numberOfDataPoints=380,numberOfValues=380,shapeOfTheEarth=4,latitudeOfFirstGridPointInDegrees=66.982143,longitudeOfFirstGridPointInDegrees=324.965976,standardParallelInDegrees=52,centralLongitudeInDegrees=10,xDirectionGridLengthInMetres=50000,yDirectionGridLengthInMetres=50000,jScansPositively=0|{"false":[1818500.021,-2287499.955],"projection":{"figure":"grs80","lat_0":52,"lon_0":10,"reverse":true,"type":"laea"},"x":[0,950000,50000],"y":[0,-900000,-50000]}|+proj=laea +lon_0=10.000000 +lat_0=52.000000 +a=6378137.000000 +b=6356752.314000|+proj=laea +lat_0=52 +lon_0=10 +ellps=GRS80|legacy
GRIB2.tmpl|gridType="space_view",Nx=16,Ny=16,values={1},numberOfDataPoints=256,numberOfValues=256,shapeOfTheEarth=6,dx=3622,dy=3622,Xp=1856000,Yp=1856000,Xo=1500,Yo=1700,Nr=6610700|{"false":[1067074.48,-467594.435],"projection":{"h":35747054.5503,"reverse":true,"type":"space_view"},"x":[0,44961.0033558134,2997.40022372089],"y":[0,-44961.0033558134,-2997.40022372089]}|-|-|legacy
GRIB2.tmpl|gridType="space_view",Nx=16,Ny=12,values={1},numberOfDataPoints=192,numberOfValues=192,shapeOfTheEarth=5,longitudeOfSubSatellitePoint=-75000000,dx=3622,dy=3610,Xp=1856000,Yp=1856000,Xo=2400,Yo=1200,Nr=6610700|{"false":[-1632353.683,-1968348.046],"projection":{"figure":"wgs84","h":35785813.2659,"lon_0":-75,"reverse":true,"type":"space_view"},"x":[0,45009.7522881123,3000.65015254082],"y":[0,-33005.8361442917,-3000.53055857197]}|-|-|legacy
GRIB2.tmpl|gridType="space_view",Nx=16,Ny=16,values={1},numberOfDataPoints=256,numberOfValues=256,shapeOfTheEarth=3,scaleFactorOfEarthMajorAxis=4,scaledValueOfEarthMajorAxis=63781400,scaleFactorOfEarthMinorAxis=4,scaledValueOfEarthMinorAxis=63567550,dx=3622,dy=3610,Xp=1856000,Yp=1856000,Xo=1500,Yo=1700,Nr=6610700,iScansNegatively=1,jScansPositively=1|{"false":[-1068231.957,468082.965],"order":"i-j+","projection":{"figure":{"a":6378140,"b":6356755},"h":35785830.098,"reverse":true,"type":"space_view"},"x":[0,-45009.7734587546,-3000.65156391697],"y":[0,45007.9773946342,3000.53182630895]}|-|-|-4.251977500 9.700685413;-4.251905444 9.673016062;-3.841154157 9.280465701
GRIB1.tmpl|gridType="space_view",Nx=16,Ny=16,values={1},earthIsOblate=1,longitudeOfSubSatellitePoint=140700,dx=5434,dy=5434,XpInGridLengths=2750,YpInGridLengths=2750,Xo=2600,Yo=2700,Nr=6610710|{"false":[300010.955,-99668.354],"projection":{"figure":"iau1965","h":35786006.0936,"lon_0":140.7,"reverse":true,"type":"space_view"},"x":[0,30001.0954560476,2000.07303040317],"y":[0,-29900.5063478522,-1993.36708985681]}|-|-|legacy
GRIB2.tmpl|gridType="rotated_ll",Ni=16,Nj=31,latitudeOfFirstGridPointInDegrees=60,longitudeOfFirstGridPointInDegrees=0,latitudeOfLastGridPointInDegrees=0,longitudeOfLastGridPointInDegrees=30,iDirectionIncrementInDegrees=2,jDirectionIncrementInDegrees=2,latitudeOfSouthernPoleInDegrees=-40,longitudeOfSouthernPoleInDegrees=10|{"area":[60,0,0,30],"grid":[2,2],"projection":{"south_pole":[10,-40],"type":"rotation"}}|-|+proj=ob_tran +lon_0=10 +o_lat_p=40 +o_proj=longlat +R=6371229|70.000000000 190.000000000;69.960949271 187.080989800;41.560762570 51.930105190
regular_gg_sfc_grib2.tmpl|gridType="rotated_gg",latitudeOfSouthernPoleInDegrees=-30,longitudeOfSouthernPoleInDegrees=-20|{"grid":"F32","projection":{"south_pole":[340,-30],"type":"rotation"}}|-|+proj=ob_tran +lon_0=340 +o_lat_p=30 +o_proj=longlat +R=6371229|32.136201161 160.000000000;32.133570198 159.876248309;-27.866318938 339.881459945
reduced_gg_pl_32_grib2.tmpl|gridType="reduced_rotated_gg",latitudeOfSouthernPoleInDegrees=-30,longitudeOfSouthernPoleInDegrees=-20|{"grid":"N32","projection":{"south_pole":[340,-30],"type":"rotation"}}|-|+proj=ob_tran +lon_0=340 +o_lat_p=30 +o_proj=longlat +R=6371229|32.136201161 160.000000000;32.029359820 159.221504855;-27.966243726 339.252751033
gridspec/gridType=lambert.grib|count=11|{"false":[704556.223,584635.182],"order":"i+j+","projection":{"lat_0":40,"lat_1":40,"lon_0":357.5,"reverse":true,"type":"lcc"},"x":[0,1410000,2500],"y":[0,1170000,2500]}|+proj=lcc +lon_0=357.500000 +lat_0=40.000000 +lat_1=40.000000 +lat_2=40.000000 +R=6367470.000000|+proj=lcc +lat_0=40 +lat_1=40 +lon_0=357.5 +R=6367470|legacy
gridspec/gridType=lambert_azimuthal_equal_area.grib|count=1|{"false":[5387500.044,-5387500.044],"projection":{"figure":"wgs84","lat_0":90,"reverse":true,"type":"laea"},"x":[0,10775000,25000],"y":[0,-10775000,-25000]}|+proj=laea +lon_0=0.000000 +lat_0=90.000000 +a=6378137.000000 +b=6356752.314000|+proj=laea +lat_0=90 +ellps=WGS84|legacy
gridspec/gridType=mercator.grib|count=1|{"false":[7108383.475,-1800563.621],"order":"i+-j+","projection":{"figure":"wgs84_sphere","lat_ts":20,"reverse":true,"type":"merc"},"x":[0,422500,1250],"y":[0,280000,1250]}|+proj=merc +lat_ts=20.000000 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +R=6371200.000000|+proj=merc +lat_ts=20 +R=6371200|legacy
gridspec/gridType=polar_stereographic.grib|count=1|{"false":[1901416.78,7613145.491],"order":"i+j+","projection":{"lat_0":90,"lat_ts":60,"lon_0":-105,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]}|+proj=stere +lat_ts=60.000000 +lat_0=90 +lon_0=-105.000000 +k_0=1 +x_0=0 +y_0=0 +R=6367470.000000|+proj=stere +lat_0=90 +lat_ts=60 +lon_0=-105 +R=6367470|-
gridspec/gridType=space_view.grib|count=1|{"false":[2292497.795,-5322943.46],"projection":{"figure":{"a":6378140,"b":6356755},"h":35785830.098,"reverse":true,"type":"space_view"},"x":[0,5698237.31987833,3000.65156391697],"y":[0,-2697478.11185174,-3000.53182630895]}|-|-|legacy
gridspec/gridType=space_view.grib|count=1,Nx=16,Ny=12,Xo=600,Yo=500,values={1},numberOfDataPoints=192,numberOfValues=192|{"false":[492106.856,-3822677.547],"projection":{"figure":{"a":6378140,"b":6356755},"h":35785830.098,"reverse":true,"type":"space_view"},"x":[0,45009.7734587546,3000.65156391697],"y":[0,-33005.8500893984,-3000.53182630895]}|-|-|legacy
gridspec/gridType=space_view.grib|count=6,Nx=16,Ny=12,Xo=2400,Yo=1200,values={1},numberOfDataPoints=192,numberOfValues=192|{"false":[1632354.451,1968348.878],"order":"i-j+","projection":{"figure":{"a":6378140,"b":6356755},"h":35785830.098,"reverse":true,"type":"space_view"},"x":[0,-45009.7734587546,-3000.65156391697],"y":[0,33005.8500893984,3000.53182630895]}|-|-|-18.425012580 344.094434648;-18.425559470 344.063919391;-18.106823587 343.672864796
gridspec/gridType=rotated_ll.grib|count=4|{"area":[69,-60,21,60],"grid":[3,3],"projection":{"south_pole":[30,30],"type":"rotation"}}|-|+proj=ob_tran +lon_0=30 +o_lat_p=-30 +o_proj=longlat +R=6371229|-
gridspec/gridType=rotated_gg.grib|count=1|{"grid":"F48","projection":{"south_pole":[30,30],"type":"rotation"}}|-|+proj=ob_tran +lon_0=30 +o_lat_p=-30 +o_proj=longlat +R=6371229|-
gridspec/gridType=reduced_rotated_gg.grib|count=5|{"area":[27.9625200060835,0,-13.9110027017923,40],"grid":"N320","projection":{"south_pole":[-40,-22],"type":"rotation"}}|-|+proj=ob_tran +lon_0=-40 +o_lat_p=22 +o_proj=longlat +R=6371229|-
GRIDS


# Clean up
rm -f $tempGrib $tempGrib.in $tempFilt $tempText
