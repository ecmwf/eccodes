:
set -eu

pid=$1

sample2=samples/GRIB2.tmpl
temp=$TMPDIR/temp.create_legacy_def.grib
defs=definitions/grib2/localConcepts/ecmf

output_def()
{
    _name=$1
    _val=$2
    _d=$3
    _c=$4
    _n=$5

    if [ $_c -eq 0 ]; then
        _c=128
    fi
    if [ "$_val" != "unknown" ]; then
        echo "#$_name"
        echo "'$_val' = {"
        echo " discipline = $_d ;"
        echo " parameterCategory = $_c ;"
        echo " parameterNumber = $_n ;"
        echo "}"
    fi
}

grib_set -s paramId=$pid $sample2 $temp
name=$(grib_get -p name $temp)
shortName=$(grib_get -p shortName $temp)
units=$(grib_get -p units $temp)
cfName=$(grib_get -p cfName $temp)
cfVarName=$(grib_get -p cfVarName $temp)

dis=192
cat=$((pid/1000))
num=$((pid - cat*1000))

output_def "$name" "$pid"       $dis $cat $num >> $defs/paramId.legacy.def
output_def "$name" "$name"      $dis $cat $num >> $defs/name.legacy.def
output_def "$name" "$shortName" $dis $cat $num >> $defs/shortName.legacy.def
output_def "$name" "$units"     $dis $cat $num >> $defs/units.legacy.def
output_def "$name" "$cfVarName" $dis $cat $num >> $defs/cfVarName.legacy.def
output_def "$name" "$cfName"    $dis $cat $num >> $defs/cfName.legacy.def

echo "Files updated"
rm -f $temp
