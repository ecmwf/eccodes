#!/bin/ksh
set -ea

if [[ "$@" = "-k" ]]
then
	set +e
	rm -f fail.t pass.t
fi
rm -f fail.grib || true

set -u

SAMPLES=/vol/dataserv/grib_samples/brute_force


find $SAMPLES -type f \( -name '*.g???' -o -name '*.GB' \) -print > all_gribs
touch pass.t skip.t
/path/to/filediff all_gribs pass.t > tmp
/path/to/filediff tmp skip.t > todo

while true
do
	read file || break

	echo
	echo $file
	echo
	
	ok=1
	for t in  `cat tests| grep -v "#" `
	do
		echo $t
		ln -sf $file data
		./$t $file
		if [[ $? -ne 0 ]]
		then
			ok=0
		fi
	done
	if [[ $ok -eq 0 ]]
	then
		echo $file >> fail.t
		ln -s $file fail.grib
		exit
	else
		echo $file >> pass.t
	fi
	

done < todo

