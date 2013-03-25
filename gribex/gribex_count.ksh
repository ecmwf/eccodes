#!/usr/bin/ksh

gribex_count=./gribex_count
typeset -i n
n=`$gribex_count -i $1 | sed '/GRIBEX: Grib Edition not catered for./d' | sed '/GRIBEX: Return code =  413/d'`

echo $n

