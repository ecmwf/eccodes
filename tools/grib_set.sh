#!/bin/sh
set -e

echo "-# To set productDefinitionTemplateNumber=2 only for the fields with productDefinitionTemplateNumber=11 \\n
\\verbatim
>grib_set -s productDefinitionTemplateNumber=2 -w productDefinitionTemplateNumber=11 ../data/tigge_pf_ecmwf.grib2 out.grib2
\\endverbatim\\n"

echo "-# To set productDefinitionTemplateNumber=2 only for the fields for which productDefinitionTemplateNumber is not equal to 11 \\n
\\verbatim
>grib_set -s productDefinitionTemplateNumber=2 -w productDefinitionTemplateNumber!=11 tigge_pf_ecmwf.grib2 out.grib2
\\endverbatim\\n"

echo "-# When a key is not used all the bits of its value should be set to 1 to indicate that it is missing. Since the length 
 (number of octet) is different from a key to another, the value that we have to code for missing keys is not unique. 
 To give an easy way to set a key to missing a string \"missing\" or \"MISSING\" is accepted by grib_set as follows:\n
\\verbatim
>grib_set -s scaleFactorOfFirstFixedSurface=missing,scaledValueOfFirstFixedSurface=MISSING ../data/regular_latlon_surface.grib2 out.grib2
\\endverbatim\\n
Since some values can not be set to missing you can get an error for those keys.\\n"

echo "-# To set scaleFactorOfSecondFixedSurface to missing only for the fields for which scaleFactorOfSecondFixedSurface is not missing: \\n
\\verbatim 
>grib_set -s scaleFactorOfSecondFixedSurface=missing -w scaleFactorOfSecondFixedSurface!=missing tigge_pf_ecmwf.grib2 out.grib2
\\endverbatim\\n"

echo "-# It's possible to produce a grib edition 2 file from a grib edition 1 just changing the edition number with grib_set.
At this stage of development all the geography parameters, level and time information is correctly translated, for the product
definition extra set calls must be done. To do this properly, \ref grib_filter is suggested. \\n
\\verbatim 
grib_set -s edition=2 ../data/reduced_gaussian_pressure_level.grib1
\\endverbatim\\n"

echo "-# With grib edition 2 is possible to compress data using the jpeg algorithm. 
To change packing algorithm from grid_simple (simple packing) to grid_jpeg (jpeg2000 packing):\\n
\\verbatim 
>grib_set -s packingType=grid_jpeg ../data/regular_gaussian_model_level.grib2 out.grib2
\\endverbatim\\n"

echo "-# It's possible to ask ecCodes to calculate the number of bits per value needed to pack a given
field with a fixed number of decimal digits of precision. For example if we want to pack a temperature
expressed in Kelvin with 1 digits of precision after the decimal point we can set changeDecimalPrecision=1 \\n
\\verbatim 
>grib_set -s changeDecimalPrecision=1 ../data/regular_latlon_surface.grib2 ../data/out.grib2
\\endverbatim\\n"

