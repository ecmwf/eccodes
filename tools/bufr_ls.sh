#!/bin/sh

#Default options

echo "-# Without options a <b>default list of keys</b> is printed. \\n"
echo "The default list can be different depending on the type of BUFR message. \\n"
echo "\\verbatim "
echo "> bufr_ls ../data/bufr/syno_multi.bufr \\n"
echo "\\endverbatim\\n "

echo "The result is: \\n"

echo "\\verbatim "

./bufr_ls ../data/bufr/syno_multi.bufr

echo "\\endverbatim\\n "

#Wildcards

echo "-# It is allowed to use <b>wildcards</b> in filenames.  \\n"
echo "\\verbatim "
echo "> bufr_ls ../data/bufr/syno_*.bufr \\n"
echo "\\endverbatim\\n "

#echo "The result is: \\n"

#echo "\\verbatim "

#./bufr_ls ../data/bufr/syno_*.bufr

#echo "\\endverbatim\\n "

#Custom keys

echo "-# To list only a specific set of keys use the <b>-p</b> option. \\n"
echo "\\verbatim "
echo "> bufr_ls -p totalLength,bufrHeaderCentre,bufrHeaderSubCentre ../data/bufr/syno_multi.bufr \\n"
echo "\\endverbatim\\n "

#echo "The result is: \\n"

#echo "\\verbatim "

#./bufr_ls -p totalLength,bufrHeaderCentre,bufrHeaderSubCentre ../data/bufr/syno_multi.bufr

#echo "\\endverbatim\\n "

#Where

echo "-# To list only a subset of messages use the <b>-w</b> (where option).\\n "
echo "Only the 12 UTC messages are listed with the following line.\\n "
echo "\\verbatim "
echo "> bufr_ls -w typicalTime=\"120000\" ../data/bufr/syno_*.bufr"

#./bufr_ls -w typicalTime="120000" ../data/bufr/syno_*.bufr

echo "\\endverbatim\\n "

#Where

echo "-# All the non-12 UTC messages are listed as follows:\\n "
echo "\\verbatim "
echo ">  bufr_ls -w typicalTime!=\"120000\" ../data/bufr/syno_*.bufr"
echo "\\endverbatim\\n "

#echo "The result is: \\n"

#echo "\\verbatim "

#./bufr_ls -w typicalTime!="120000" ../data/bufr/syno_*.bufr 

#echo "\\endverbatim\\n "

echo "-# To list only the second message from a BUFR file:\\n "
echo "\\verbatim "
echo "> bufr_ls -w count=2 ../data/bufr/syno_multi.bufr"

#./bufr_ls -w count=2  ../data/bufr/syno_multi.bufr

echo "\\endverbatim\\n "

