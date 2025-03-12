#!/usr/bin/ksh
set -e

bin=/usr/local/lib/metaps/bin

gribex_count=$bin/gribex_count
grib_api_count=$bin/grib_api_count

typeset -i ngribex
typeset -i ngrib_api
typeset -i diff

for file in $1
do

	# to skip the errors due to edition 2
	ngribex=`$gribex_count -i $file | sed '/D2ORDR: 21st octet of section 4 not zero:/d' | sed '/interpreted as extra bits for group number/d' | sed '/GRIBEX: Grib Edition not catered for./d' | sed '/GRIBEX: Return code =  413/d'`

	#grib_api tool is counint only grib edition 1
	ngrib_api=`$grib_api_count 1 $file`

	ngrib_api_all=`$grib_api_count $file`

	if [ $ngribex != $ngrib_api ]
	then
		echo gribex count != grib_api count:  $ngribex != $ngrib_api 
		echo $file - ERR -
#	exit 1
	fi

	(( diff=ngrib_api_all - ngrib_api ))
	if [ $diff != 0 ]
	then
		echo $file == $diff GRIB 2 messages == 
	else
		echo $file - OK -
	fi

done




