#!/bin/ksh

set -eu

templates_dir=ifs_templates
date=-1
bitsPerValue=16
editionNumber=1
set -A gg_resols 48 512 400
set -A sh_resols 63 799 511


[[ -d $templates_dir ]] ||  mkdir -p $templates_dir 
cd $templates_dir

cat > ifs_templates.filter << EOF
transient nlev=0;
if ( bitsPerValue != $bitsPerValue ) { set bitsPerValue = $bitsPerValue; }

if ( levtype is "ml" ) {
 set nlev = numberOfVerticalCoordinateValues / 2 - 1;
 if ( gridType is "sh" ) {
   if ( editionNumber != $editionNumber ) { set editionNumber = $editionNumber; }
   print "[gridType]_[levtype]_T[pentagonalResolutionParameterJ]_L[nlev]_[bitsPerValue].grib[editionNumber]";
   write "[gridType]_[levtype]_T[pentagonalResolutionParameterJ]_L[nlev]_[bitsPerValue].grib[editionNumber]";
 }
 if ( gridType is "reduced_gg" ) {
   if ( editionNumber != $editionNumber ) { set editionNumber = $editionNumber; }
   print "gg_[levtype]_N[numberOfParallelsBetweenAPoleAndTheEquator]_L[nlev]_[bitsPerValue].grib[editionNumber]";
   write "gg_[levtype]_N[numberOfParallelsBetweenAPoleAndTheEquator]_L[nlev]_[bitsPerValue].grib[editionNumber]";
 }
}

if ( levtype is "pl" ) {
 if ( gridType is "sh" ) {
   if ( editionNumber != $editionNumber ) { set editionNumber = $editionNumber; }
   print "[gridType]_[levtype]_T[pentagonalResolutionParameterJ]_[bitsPerValue].grib[editionNumber]";
   write "[gridType]_[levtype]_T[pentagonalResolutionParameterJ]_[bitsPerValue].grib[editionNumber]";
 }
 if ( gridType is "reduced_gg" ) {
   if ( editionNumber != $editionNumber ) { set editionNumber = $editionNumber; }
   print "gg_[levtype]_N[numberOfParallelsBetweenAPoleAndTheEquator]_[bitsPerValue].grib[editionNumber]";
   write "gg_[levtype]_N[numberOfParallelsBetweenAPoleAndTheEquator]_[bitsPerValue].grib[editionNumber]";
 }
}

if ( levtype is "sfc" ) {
 if ( gridType is "reduced_gg" ) {
   if ( editionNumber != $editionNumber ) { set editionNumber = $editionNumber; }
   print "gg_[levtype]_N[numberOfParallelsBetweenAPoleAndTheEquator]_[bitsPerValue].grib[editionNumber]";
   write "gg_[levtype]_N[numberOfParallelsBetweenAPoleAndTheEquator]_[bitsPerValue].grib[editionNumber]";
 }
}


EOF

function do_templates
{
  target=x.grib

  for ret in ${rets[@]}
  do

    rm -f $target || true 
    mars << EOF
    $ret,
    target=$target
EOF

    grib_filter ifs_templates.filter $target 
  done

}


for resol in ${sh_resols[@]} 
do 

  base_retrieve="retrieve,resol=$resol,date=$date"

  set -A rets   \
    "${base_retrieve},param=t,levtype=pl,level=1000" \
    "${base_retrieve},param=t,levtype=ml,levelist=1" 

  do_templates

done

for resol in ${gg_resols[@]} 
do 

  base_retrieve="retrieve,gaussian=reduced,grid=$resol,date=$date"

  set -A rets   \
    "${base_retrieve},param=q,levtype=pl,level=1000" \
    "${base_retrieve},param=t,levtype=ml,levelist=1,gaussian=reduced" \
    "${base_retrieve},param=2t,levtype=sfc" 

  do_templates

done

rm -f ifs_templates.filter $target || true

ls -l 

grib_ls *
