#!/bin/sh
set -e

echo The following grib_convert rules convert all the grib messages 
echo contained in the input files in grib edition 2 and if a 2 metre
echo temperature is found also the keys contained in the culy bracket are
echo changed.\\n  
echo "\code "
echo "editionNumber = 2;"
echo "if( indicatorOfParameter == 11 && indicatorOfTypeOfLevel == 105)"
echo "{"
echo "    productDefinitionTemplateNumber = 1;"
echo "    typeOfFirstFixedSurface         = 103;"
echo "    scaleFactorOfFirstFixedSurface  = 0;"
echo "    scaledValueOfFirstFixedSurface  = 2;"
echo "}"
echo "\endcode"
echo
