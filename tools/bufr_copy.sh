#!/bin/sh
set -e

echo "-# To copy only the synop messages from a file\\n"
echo "\verbatim"
echo "> bufr_copy -w dataCategory=0 in.bufr out.bufr"
echo "\endverbatim\\n"

echo "-# To copy only the non-synop messages from a file\\n"
echo "\verbatim"
echo "> bufr_copy -w dataCategory!=0 in.bufr out.bufr"
echo "\endverbatim\\n"
