#!/bin/sh
set -e 

echo "-# To dump BUFR messages into a flat JSON format.\\n"
echo "\\verbatim"
echo  ">bufr_dump -jf ../data/bufr/aaen_55.bufr"
echo "\\endverbatim\\n"

echo "-# To dump BUFR messages into a structured JSON format. Note: This is the default if you omit the -j option\\n"
echo "\\verbatim"
echo  ">bufr_dump -js ../data/bufr/aaen_55.bufr"
echo "\\endverbatim\\n"

echo "-# To dump a Fortran program with instructions to create (<b>encode</b>) the input message.\\n"
echo "\\verbatim"
echo  ">bufr_dump -Efortran ../data/bufr/aaen_55.bufr > encode.aaen_55.f90"
echo "\\endverbatim\\n"
echo  "Now compile and run 'encode.aaen_55.f90'. This will create a new BUFR file called 'outfile.bufr'."
echo  "Check this is the the same as the input."
echo "\\verbatim"
echo  ">bufr_compare ../data/bufr/aaen_55.bufr outfile.bufr"
echo "\\endverbatim\\n"


echo "-# To dump a Python program with instructions to <b>decode</b> the input message.\\n"
echo "\\verbatim"
echo  ">bufr_dump -Dpython ../data/bufr/aaen_55.bufr > decode.aaen_55.py"
echo "\\endverbatim\\n"
echo  "Examine the generated Python script 'decode.aaen_55.py'. You will see how to access each of the BUFR keys"


echo "-# To dump in a WMO documentation style with hexadecimal octet values (-H).\\n"
echo "\\verbatim"
echo  ">bufr_dump -OH ../data/bufr/syno_1.bufr"
echo "\\endverbatim\\n"


echo "-# To add key type information (-t).\\n"
echo "\\verbatim"
echo  ">bufr_dump -OtH ../data/bufr/syno_1.bufr"
echo "\\endverbatim\\n"
