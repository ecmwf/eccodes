#!/usr/bin/ksh

rm -f grib_rgauss_*
for fname in rgauss_*.h 
  do
     nfname=grib_$fname
	 cat $fname | sed -n '/REAL/!p' | sed 's/DATA QG/int grib_rg_/g' | sed 's/ \// };/g' | sed 's/\//[] = {/g' | sed 's/X//g'   > $nfname
done

all=grib_rgauss.h
rm -f $all

count=-1
integer order
for fname in grib_rgauss_*.h 
  do
	 count=$(( ${count} + 1 ))
     subarr[count]=`cat $fname | head -1 | sed 's/^[ \t]*//'  | sed 's/int//g' | sed 's/\[\] = {//g'`
	 order[count]=`echo ${subarr[count]} | sed 's/grib_rg_//g`
	 cat $fname >> $all 
done

echo 'int* grib_rg[] = {' >> $all

for (( i=1 ; ${i} < ${count}; i=$(( ${i} + 1 )) ))
   do
     echo ${subarr[i]}',' >> $all
done

echo ${subarr[count]} >> $all
echo '};' >> $all

echo '' >> $all
echo 'int grib_rg_idx[] = {' >> $all
for (( i=1 ; ${i} < ${count}; i=$(( ${i} + 1 )) ))
   do
     echo ${order[i]}',' >> $all
done

echo ${order[count]} >> $all
echo '};' >> $all

echo '' >> $all
echo '#define GRIB_MAX_RG_GRIDS  '$count >> $all
echo '' >> $all

