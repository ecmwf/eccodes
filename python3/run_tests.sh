#!/bin/sh

input=$1

./test_general.py $input
./test_index.py $input
./test_keysiterator.py $input
./test_iterator.py $input
