#!/usr/bin/ksh

streams=`cat stream.table | awk '{print $2;}'`
#streams="enda ewda"
types=`cat type.table | awk '{print $2;}'`
types="me"

for stream in $streams
do
	for type in $types
	do
		file="grib1.$stream.$type.def"
		if [ ! -f $file ]
		then
			ln -s grib1.$type.def $file
		fi
	done
done
