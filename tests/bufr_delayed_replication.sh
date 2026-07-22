#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

# This test runs many hundreds of combinations; disable command tracing
# (enabled by include.ctest.sh) to keep it fast and the log readable.
set +x

# ---------------------------------------------------------------------
# Exhaustive test of nested BUFR delayed replications
#
# Template (nesting extended / delayed / short replications):
#
#   111000, 031002,             outer, extended factor E, 11 descriptors
#       007040,                 impactParameter                  (1 per outer rep)
#       105000, 031001,         inner, delayed factor D1
#           002153,             satelliteChannelCentreFrequency  (1 per D1 rep)
#           101000, 031000,     short, factor S
#               015037,         bendingAngle                     (1 per S rep)
#           033007,             percentConfidence                (1 per D1 rep)
#       101000, 031001, 015036  delayed factor D2; atmosphericRefractivity (1 per D2 rep)
#
# For every combination of E (0..2) outer repetitions, the per-outer-rep
# delayed factors D1 and D2 (D1,D2 in 0..3 for a single outer rep; for two
# outer reps D1 in 0..2 and D2 in 0..1) and EVERY possible binary vector of
# the enclosed short (031000) factors, the test:
#   * encodes the message (must not fail),
#   * decodes it and checks that
#       - the replication factor arrays round-trip exactly,
#       - the number of decoded elements matches the value computed
#         independently from the replication factors.
# ---------------------------------------------------------------------

label="bufr_delayed_replication_test"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempOut=temp.$label.txt
tempVec=temp.$label.vec

sample_bufr4=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

ncases=0
nfail=0

# Extract the normalised (comma-separated, no spaces) contents of a BUFR array
# key from a "bufr_dump -p" output. Prints an empty string when the key is
# absent (e.g. a replication that was never entered).
extract_array()
{
    awk -v k="$1" '
        index($0, k"=") == 1 { inb = 1 }
        inb {
            buf = buf $0
            if (index($0, "}") > 0) inb = 0
        }
        END {
            sub(/.*{/, "", buf)
            sub(/}.*/, "", buf)
            gsub(/[ \t]/, "", buf)
            print buf
        }
    ' "$tempOut"
}

count_key()
{
    n=`grep -c "$1" "$tempOut"` || true
    echo "$n"
}

# run_case E "D1 per rep" "D2 per rep" "flattened short factors"
run_case()
{
    E="$1"
    d1list="$2"
    d2list="$3"
    shortlist="$4"

    # Interleaved delayed factor array (consumption order): D1_1,D2_1,D1_2,D2_2,...
    set -- $d2list
    delayed_seq=""
    sum_d1=0
    sum_d2=0
    for d1 in $d1list; do
        d2="$1"; shift
        delayed_seq="$delayed_seq $d1 $d2"
        sum_d1=$((sum_d1 + d1))
        sum_d2=$((sum_d2 + d2))
    done

    # Expected number of decoded elements
    exp_impact=$E
    exp_sat=$sum_d1
    exp_percent=$sum_d1
    exp_atmos=$sum_d2
    exp_bending=0
    for s in $shortlist; do
        exp_bending=$((exp_bending + s))
    done

    # Normalised expected arrays (empty when the key is not present)
    exp_delayed_norm=`echo $delayed_seq | sed 's/^ *//; s/ *$//; s/  */,/g'`
    exp_short_norm=`echo $shortlist     | sed 's/^ *//; s/ *$//; s/  */,/g'`
    exp_ext_norm=$E

    # Filter input arrays: never empty (unused trailing values are ignored)
    in_delayed="$delayed_seq"; [ -z "`echo $in_delayed | tr -d ' '`" ] && in_delayed="0"
    in_short="$shortlist";     [ -z "`echo $in_short   | tr -d ' '`" ] && in_short="0"
    in_delayed=`echo $in_delayed | sed 's/^ *//; s/ *$//; s/  */, /g'`
    in_short=`echo $in_short     | sed 's/^ *//; s/ *$//; s/  */, /g'`

    cat > $tempFilt <<EOF
set edition = 4;
set masterTablesVersionNumber = 39;
set numberOfSubsets = 1;
set observedData = 1;
set compressedData = 0;
set inputExtendedDelayedDescriptorReplicationFactor = {$E};
set inputDelayedDescriptorReplicationFactor = {$in_delayed};
set inputShortDelayedDescriptorReplicationFactor = {$in_short};
set unexpandedDescriptors = {
    111000, 031002, 007040, 105000, 031001, 002153,
    101000, 031000, 015037, 033007, 101000, 031001, 015036
};
set pack = 1;
write;
EOF

    ncases=$((ncases + 1))

    if ! ${tools_dir}/bufr_filter -o $tempBufr $tempFilt $sample_bufr4 > /dev/null 2>&1; then
        echo "FAIL (encode): E=$E D1={$d1list} D2={$d2list} short={$shortlist}"
        nfail=$((nfail + 1))
        return
    fi

    ${tools_dir}/bufr_dump -p $tempBufr > $tempOut 2>/dev/null

    got_delayed=`extract_array delayedDescriptorReplicationFactor`
    got_short=`extract_array shortDelayedDescriptorReplicationFactor`
    got_ext=`extract_array extendedDelayedDescriptorReplicationFactor`

    got_impact=`count_key impactParameter`
    got_sat=`count_key satelliteChannelCentreFrequency`
    got_bending=`count_key bendingAngle`
    got_percent=`count_key percentConfidence`
    got_atmos=`count_key atmosphericRefractivity`

    ok=1
    [ "$got_delayed" = "$exp_delayed_norm" ] || ok=0
    [ "$got_short"   = "$exp_short_norm"   ] || ok=0
    [ "$got_ext"     = "$exp_ext_norm"     ] || ok=0
    [ "$got_impact"  = "$exp_impact"       ] || ok=0
    [ "$got_sat"     = "$exp_sat"          ] || ok=0
    [ "$got_bending" = "$exp_bending"      ] || ok=0
    [ "$got_percent" = "$exp_percent"      ] || ok=0
    [ "$got_atmos"   = "$exp_atmos"        ] || ok=0

    if [ $ok -eq 0 ]; then
        nfail=$((nfail + 1))
        echo "FAIL: E=$E D1={$d1list} D2={$d2list} short={$shortlist}"
        echo "  delayed  got='$got_delayed' exp='$exp_delayed_norm'"
        echo "  short    got='$got_short' exp='$exp_short_norm'"
        echo "  extended got='$got_ext' exp='$exp_ext_norm'"
        echo "  counts   impact=$got_impact/$exp_impact sat=$got_sat/$exp_sat bending=$got_bending/$exp_bending percent=$got_percent/$exp_percent atmos=$got_atmos/$exp_atmos"
    fi
}

# Write all 2^n binary vectors of length n to $tempVec (one per line, space
# separated). For n=0 write a single empty line.
gen_short_vectors()
{
    n=$1
    : > $tempVec
    if [ "$n" -eq 0 ]; then
        echo "" >> $tempVec
        return
    fi
    total=1; j=0
    while [ $j -lt $n ]; do total=$((total * 2)); j=$((j + 1)); done
    mask=0
    while [ $mask -lt $total ]; do
        vec=""; bit=0; shift_val=1
        while [ $bit -lt $n ]; do
            b=$(( (mask / shift_val) % 2 ))
            vec="$vec $b"
            shift_val=$((shift_val * 2))
            bit=$((bit + 1))
        done
        echo "$vec" >> $tempVec
        mask=$((mask + 1))
    done
}

# ---- E = 0 : empty outer replication ----------------------------------
run_case 0 "" "" ""

# ---- E = 1 : single outer rep, D1,D2 in 0..3, ALL short vectors --------
D1=0
while [ $D1 -le 3 ]; do
    D2=0
    while [ $D2 -le 3 ]; do
        gen_short_vectors $D1
        while read sv; do
            run_case 1 "$D1" "$D2" "$sv"
        done < $tempVec
        D2=$((D2 + 1))
    done
    D1=$((D1 + 1))
done

# ---- E = 2 : two outer reps. D1a,D1b in 0..2 (these drive the enclosed
#      short factors, the heart of the bug) with ALL short vectors, and
#      D2a,D2b in 0..1. Exercises restarting the outer replication across
#      empty and non-empty inner/short replications. ------------------------
D1a=0
while [ $D1a -le 2 ]; do
    D1b=0
    while [ $D1b -le 2 ]; do
        D2a=0
        while [ $D2a -le 1 ]; do
            D2b=0
            while [ $D2b -le 1 ]; do
                L=$((D1a + D1b))
                gen_short_vectors $L
                while read sv; do
                    run_case 2 "$D1a $D1b" "$D2a $D2b" "$sv"
                done < $tempVec
                D2b=$((D2b + 1))
            done
            D2a=$((D2a + 1))
        done
        D1b=$((D1b + 1))
    done
    D1a=$((D1a + 1))
done

echo "Ran $ncases delayed-replication combinations, $nfail failures"

rm -f $tempBufr $tempFilt $tempOut $tempVec

[ "$nfail" -eq 0 ]
