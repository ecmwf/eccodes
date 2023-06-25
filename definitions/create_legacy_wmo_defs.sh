:
set -eu

# Usage:
#  create_legacy_def.sh $paramId $paramId...
#
# This script will insert the local ECMWF GRIB2 representation
# for each paramId into the files:
#   definitions/grib2/paramId.legacy.def
#   definitions/grib2/shortName.legacy.def
#   etc
# This is normally run for those GRIB2 parameters which had
# a WMO representation which later became deprecated.
#
# Assumptions:
#  The ecCodes tools grib_set and grib_get are available
#

if [ $# -eq 0 ]; then
    echo "Usage: $0 pid1 pid2 ..." 2>&1
    exit 1
fi

# Input list of paramId values
pids="$@"

sample2=samples/GRIB2.tmpl
temp=$TMPDIR/temp.create_legacy_def.grib
defs=definitions/grib2

create_legacy_def()
{
    _name=$1
    _dir=$2
    _file=$3

    _legacy=$(echo $_file | sed -e 's/\.def/.legacy.def/')
    #echo "Doing |$dir|  f=|$file| l=|$legacy|"
    grep -q "^#$_name$" $_dir/$_file
    if [ $? -eq 0 ]; then
        _name="#$_name"
        found=1
        echo "$_name" >> $_dir/$_legacy
        sed -e "1,/^$_name$/d" -e '/#/,$d' $_dir/$_file | tr -d '\t' >> $_dir/$_legacy
    fi
}

create_legacy_defs()
{
    name="$1"
    create_legacy_def "$name" $defs paramId.def
    create_legacy_def "$name" $defs shortName.def
    create_legacy_def "$name" $defs name.def
    create_legacy_def "$name" $defs units.def
}

# For each paramId, get its full name
count=0
for pid in $pids; do
    echo "Doing $pid ..."
    set +e
    grib_set -s paramId=$pid $sample2 $temp 2>/dev/null
    status=$?
    set -e
    if [ $status -ne 0 ]; then
        grib_set -s stepType=accum,paramId=$pid $sample2 $temp
    fi
    name=$(grib_get -p name $temp)
    count=$((count+1))
    rm -f $temp

    create_legacy_defs "$name"
done


echo "Number of legacy parameters added: $count"
if [ $count -gt 0 ]; then
    echo "Files updated. Check directory $defs"
fi
rm -f $temp
