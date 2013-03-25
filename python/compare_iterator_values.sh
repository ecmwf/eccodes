#!/bin/sh

for file in ../data/*.grib?; do
    ./test_iterator.py $file > temp1
    ../examples/C/iterator $file > temp2

    diff temp1 temp2 >& /dev/null

    if [[ $? == 0 ]]; then
        echo "$file --- OK"
    else
        echo "$file --- WRONG"
        basename=`basename $file`
        cp temp1 ${basename}_iterator_slave
        cp temp2 ${basename}_iterator_master
    fi
done

rm -f temp1 temp2
