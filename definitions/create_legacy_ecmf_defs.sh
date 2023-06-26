:
set -eu

# Usage:
#  $0 $paramId $paramId...
#
# This script will insert the local ECMWF GRIB2 representation
# for each paramId into the files:
#   definitions/grib2/localConcepts/ecmf/paramId.legacy.def
#   definitions/grib2/localConcepts/ecmf/shortName.legacy.def
#   etc
# This is normally run for those GRIB2 parameters which had
# a local ECMWF representation which later acquired a standard
# WMO one. We want to be able to match the old encoding to the
# paramId but when we write out a field, we want to use the new
# standard WMO encoding.
#
# Assumptions:
#  The ecCodes tools grib_set and grib_get are available
#  The legacy encoding has discipline = 192
#  The parameterCategory = $paramId / 1000
#  The parameterNumber = $paramId - parameterCategory*1000
#

if [ $# -eq 0 ]; then
    echo "Usage: $0 pid1 pid2 ..." 2>&1
    exit 1
fi

# Input list of paramId values
pids="$@"

sample2=samples/GRIB2.tmpl
temp=$TMPDIR/temp.create_legacy_ecmf_def.grib
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

count=0
for pid in $pids; do
    echo "Doing $pid..."
    set +e
    grib_set -s paramId=$pid $sample2 $temp 2>/dev/null
    status=$?
    set -e
    if [ $status -ne 0 ]; then
        grib_set -s stepType=accum,paramId=$pid $sample2 $temp
    fi
    name=$(grib_get -p name $temp)
    shortName=$(grib_get -p shortName $temp)
    units=$(grib_get -p units $temp)
    cfName=$(grib_get -p cfName $temp)
    cfVarName=$(grib_get -p cfVarName $temp)
    count=$((count+1))

    dis=192
    cat=$((pid/1000))
    num=$((pid - cat*1000))

    output_def "$name" "$pid"       $dis $cat $num >> $defs/paramId.legacy.def
    output_def "$name" "$name"      $dis $cat $num >> $defs/name.legacy.def
    output_def "$name" "$shortName" $dis $cat $num >> $defs/shortName.legacy.def
    output_def "$name" "$units"     $dis $cat $num >> $defs/units.legacy.def
    output_def "$name" "$cfVarName" $dis $cat $num >> $defs/cfVarName.legacy.def
    output_def "$name" "$cfName"    $dis $cat $num >> $defs/cfName.legacy.def

done

echo "Number of legacy parameters added: $count"
if [ $count -gt 0 ]; then
    echo "Files updated. Check directory $defs"
fi
rm -f $temp
