#!/bin/sh
set -e

echo "-# To copy only the SYNOP messages from a file\\n"
echo "\verbatim"
echo "> bufr_copy -w dataCategory=0 in.bufr out.bufr"
echo "\endverbatim\\n"

echo "-# To copy only the non-SYNOP messages from a file\\n"
echo "\verbatim"
echo "> bufr_copy -w dataCategory!=0 in.bufr out.bufr"
echo "\endverbatim\\n"

echo "-# Use the square brackets to insert the value of a key in the name of the output file. \\n"
echo "\verbatim"
echo ">bufr_copy in.bufr out_[dataCategory].bufr "
echo "\endverbatim\\n"
