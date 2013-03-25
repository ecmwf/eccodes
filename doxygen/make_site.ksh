#!/bin/ksh

cd ../html_web

for i in *.html
do

#p4 edit $i
#sed 's/index.html/grib_api.html/g' $i > $i.tmp
#mv $i.tmp $i

sed 's/href=\"grib\_/href=\"\/publications\/manuals\/grib\_api\/grib\_/g' $i > $i.tmp
mv $i.tmp $i
sed 's/<link rel=\"stylesheet\" href=\"css\/packages\.css\" type=\"text\/css\">//g' $i > $i.tmp
mv $i.tmp $i


done

#mv index.html grib_api.html
