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

# ---------------------------------------------------------------------------
# Spectral complex packing of zero-padded fields
#
# A field truncated to T<N> and then zero-padded to a higher truncation
# (e.g. by a spectral interpolation tool) has non-zero coefficients only for
# total wavenumbers n <= N. When N is just above the unpacked subset (JS),
# the least-squares fit of the laplacianOperator in calculate_pfactor() has
# only one genuine point and is driven by the artificial zero rows.
# This yields a huge operator, the scaled coefficients overflow the IEEE float
# range and encoding aborts with:
#   grib_ieee_to_long: Number is too large: x=... > xmax=3.40282346638528859812e+38
#   ecCodes assertion failed: `0' in .../grib_ieeefloat.cc:59
# ---------------------------------------------------------------------------

label="grib_sh_zero_padded_test"
tempFilt=temp.$label.filt
tempGrib=temp.$label.grib
tempText=temp.$label.txt
tempExpected=temp.$label.expected.txt
tempDecoded=temp.$label.decoded.txt

sample=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl

# Create a grib_filter rule file which keeps the coefficients of $input with
# total wavenumber n <= $keep and zero-pads the field to truncation $jout.
# The unpacked subset (JS,KS,MS) is set to $sub.
# The expected values are written to $tempExpected (one per line).
make_zero_padded_filter()
{
    input=$1
    keep=$2
    jout=$3
    sub=$4

    jin=`${tools_dir}/grib_get -p J $input`

    echo 'print "[values%.17g!1]";' > $tempFilt
    ${tools_dir}/grib_filter $tempFilt $input > $tempText

    awk -v jin=$jin -v jout=$jout -v keep=$keep -v jsub=$sub -v expected=$tempExpected '
        NF { v[nv++] = $1 }
        END {
            # Start of each zonal wavenumber m in the input (re,im interleaved)
            pos = 0
            for (m = 0; m <= jin; m++) { start[m] = pos; pos += 2 * (jin - m + 1) }
            if (pos != nv) { print "make_zero_padded_filter: got " nv " values, expected " pos > "/dev/stderr"; exit 1 }

            printf "set J=%d; set K=%d; set M=%d;\n", jout, jout, jout
            printf "set JS=%d; set KS=%d; set MS=%d;\n", jsub, jsub, jsub
            printf "set values = {"
            first = 1
            for (m = 0; m <= jout; m++)
                for (n = m; n <= jout; n++)
                    for (c = 0; c < 2; c++) {
                        val = (n <= keep && n <= jin) ? v[start[m] + 2 * (n - m) + c] : 0
                        printf "%s%s", (first ? "" : ","), val
                        print val > expected
                        first = 0
                    }
            print "};"
            print "write;"
        }' $tempText > $tempFilt
}

# Encode the zero-padded field and check that the decoded values match
# the expected ones within the packing error
check_zero_padded()
{
    keep=$1
    jout=$2
    sub=$3

    make_zero_padded_filter $sample $keep $jout $sub

    # The laplacianOperator is computed from the data on encoding
    ${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample
    grib_check_key_equals $tempGrib J,JS,numberOfValues "$jout $sub $(( (jout+1)*(jout+2) ))"

    # Compare decoded and expected values
    tolerance=`${tools_dir}/grib_get -p packingError $tempGrib`
    echo 'print "[values%.17g!1]";' > $tempFilt
    ${tools_dir}/grib_filter $tempFilt $tempGrib > $tempDecoded
    awk 'NF' $tempDecoded | paste - $tempExpected | awk -v tol=$tolerance '
        {
            d = $1 - $2; if (d < 0) d = -d
            if (d > tol) { print "Value " NR ": decoded=" $1 " expected=" $2 " diff=" d " > tolerance=" tol; bad++ }
        }
        END { if (NR == 0 || bad) exit 1 }'
}

# Case 1: T63 sample, keep n <= 21, same truncation (zeros above T21)
check_zero_padded 21 63 20

# Case 2: as done by spectral interpolation (spint): T21 padded to T42, JS=20
check_zero_padded 21 42 20

# Clean up
rm -f $tempFilt $tempGrib $tempText $tempExpected $tempDecoded
