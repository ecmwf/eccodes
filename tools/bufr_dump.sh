#!/bin/sh
set -e 

echo "-# To dump BUFR messages into a flat JSON format.\\n"
echo "\\verbatim"
echo  ">bufr_dump -jf ../data/bufr/aaen_55.bufr"
echo "\\endverbatim\\n"

echo "-# To dump BUFR messages into a structured JSON format.\\n"
echo "\\verbatim"
echo  ">bufr_dump -js ../data/bufr/aaen_55.bufr"
echo "\\endverbatim\\n"

echo "-# To dump in a WMO documentation style with hexadecimal octet values (-H).\\n"
echo "\\verbatim"
echo  ">bufr_dump -OH ../data/bufr/syno_1.bufr"
echo "\\endverbatim\\n"

echo "-# To add key type information (-t).\\n"
echo "\\verbatim"
echo  ">bufr_dump -OtH ../data/bufr/syno_1.bufr"
echo "\\endverbatim\\n"



