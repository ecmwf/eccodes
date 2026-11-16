#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

# Policy: in the ECMWF-owned GRIB tables (master + localConcepts/ecmf, current
# and .legacy files) a shortName must identify one paramId.  Codex MARS-008
# allows two paramIds to share a shortName ONLY for a units change
# (tp = 228 [m] / 228228 [kg m**-2]); any other shared shortName is a collision.
#
# What the script checks: it lists every shortName that maps to more than one
# paramId and compares that with the list below.  The test fails when a shared
# shortName appears, disappears or changes its paramIds.  Every shared
# shortName is reported, whatever its units and keys: a new paramId that
# reuses an existing shortName always makes the test fail, even when it has
# the same unit (that is not a units change either).  The four classes in the
# list are documentation only: they are not checked.
#
# How the classes were decided (offline, once, when the list was written):
#   - Only keys coded in the message count (discipline, parameterCategory,
#     parameterNumber, localTablesVersion, indicatorOfParameter, table2Version,
#     levels, statistics, ...).  A pair "differs only in the unit" when all of
#     those are the same on both sides and only the unit differs.
#   - shortName, paramId, units and name are concept tables, not message
#     fields.  'units' is what MARS-008 compares; 'name' is just an annotation:
#     a '(!)' marks a pair whose names differ too.  It never changes the class,
#     but usually means two different quantities sharing a shortName by mistake.
#   - An absent 'typeOfSecondFixedSurface' was taken as 255 ('missing'), as
#     ecCodes does; otherwise even the MARS-008 example 'grib2 tp' would look
#     like a real difference.  (This rule is not applied by the code below.)

# ADR
# https://github.com/ecmwf/codex/blob/main/MARS%20language/MARS-008-WMO-Units.md

. ./include.ctest.sh

# ---------------------------------------------------------------------------
# Skip where the definition files cannot be read directly
# ---------------------------------------------------------------------------
if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi
if [ ! -d "$ECCODES_DEFINITION_PATH/grib2" ]; then
    echo "$0: definitions not on the filesystem ($ECCODES_DEFINITION_PATH) -- test skipped"
    exit 0
fi

label="grib_check_shortname_duplicates_test"
tempList=temp.$label.list    # the list below, as written (with comments)
tempRef=temp.$label.ref      # expected: the list, normalised and sorted
tempOut=temp.$label.txt      # actual: shared shortNames found in the tables
tempPairs=temp.$label.pairs  # actual: "edition shortName paramId" per entry
tempAwk=temp.$label.awk      # awk program that produces $tempPairs

# ---------------------------------------------------------------------------
# The list of shared shortNames that are currently accepted
# ---------------------------------------------------------------------------
# Entry:   <edition> <shortName in lower case> <paramIds, sorted as text>
# Comment: everything from '#' to the end of the line is ignored, and so are
#          empty lines.  The comment after an entry gives the unit of each
#          paramId, in the order listed; a '#' line under it annotates it.
# Order does not matter: the list is sorted before it is compared.
# To update: add, change or delete entries here in place; the error message
# of a failing run shows the lines to change ('>' new, '<' gone).

# (1) DIFFER ONLY IN THE UNIT -- the MARS-008 case: all keys coded in the
#     message are identical, only the unit differs.
cat > $tempList <<'EOF'
grib1 al 174 260509           # (0 - 1) | %
# (!) 'Albedo (climatological)' vs 'Forecast albedo'
grib1 apt 151181 210120       # deg C | m
# (!) 'Analysed potential temperature' vs 'Plume top height above surface'
grib1 bld 145 3123            # J m**-2 | W m**-2
# (!) 'Time-integrated boundary layer dissipation' vs 'Boundary layer dissipation'
grib1 cap 190170 228170       # (0 - 1) | kg m**-3
grib1 ccc 185 3072            # (0 - 1) | %
grib1 dhcc 130216 216         # K s**-1 | K
grib1 dhlc 130217 217         # K s**-1 | K
# (!) 'Diabatic heating by large-scale condensation' vs 'Diabatic heating large-scale condensation'
grib1 dhr 130214 214          # K s**-1 | K
grib1 dhvd 130215 215         # K s**-1 | K
grib1 ewgd 130220 220         # m**2 s**-3 | m s**-1
# (!) 'East-West gravity wave drag' vs 'East-West gravity wave drag tendency'
grib1 hcc 188 3075            # (0 - 1) | %
grib1 htcc 130225 225         # kg kg**-1 s**-1 | kg kg**-1
grib1 htlc 130226 226         # kg kg**-1 s**-1 | kg kg**-1
grib1 lcc 186 3073            # (0 - 1) | %
grib1 lsp 142 3062            # m | kg m**-2
grib1 mcc 187 3074            # (0 - 1) | %
grib1 mdww 140235 3101        # degrees | Degree true
grib1 nsgd 130221 221         # m**2 s**-3 | m s**-1
# (!) 'North-South gravity wave drag' vs 'North-South gravity wave drag tendency'
grib1 par 217009 58           # kg kg**-1 | J m**-2
# (!) 'Paraffins' vs 'Photosynthetically active radiation at the surface'
grib1 pta 151211 171003       # deg C | K
# (!) 'Bias in temperature(applied)' vs 'Potential temperature anomaly'
grib1 ra 171157 210181        # % | kg kg**-1
# (!) 'Relative humidity anomaly' vs 'Radon'
grib1 sd 141 228141           # m of water equivalent | kg m**-2
# (!) 'Snow depth' vs 'Snow depth water equivalent'
grib1 sf 144 228144           # m of water equivalent | kg m**-2
# (!) 'Snowfall' vs 'Snowfall water equivalent'
grib1 shf 151160 3122         # J m**-2 | W m**-2
# (!) 'Specified surface heat flux' vs 'Sensible heat flux'
grib1 sr 173 190173           # m | (0 - 1)
# (!) 'Surface roughness (climatological)' vs 'Roughness length'
grib1 sro 174008 8            # kg m**-2 | m
grib1 sst 151159 34           # deg C | K
# (!) 'Specified sea surface temperature' vs 'Sea surface temperature'
grib1 swl2 170171 171         # m | m of water equivalent
grib1 tcc 164 228164          # (0 - 1) | %
grib1 tmax 140217 3015        # s | K
# (!) 'Period corresponding to maximum individual wave height' vs 'Maximum temperature'
grib1 tprate 172228 260048    # m s**-1 | kg m**-2 s**-1
# (!) 'Time-mean total precipitation rate' vs 'Total precipitation rate'
grib1 uu 150142 151143        # m s**-2 | m**2 s**-2
# (!) 'U*U product' vs 'UU product'
grib1 uv 150139 151140        # m s**-2 | m**2 s**-2
# (!) 'U*V product' vs 'UV product'
grib1 vdh 130224 224          # kg kg**-1 s**-1 | kg kg**-1
grib1 vdmw 130219 219         # m**2 s**-3 | m s**-1
grib1 vdzw 130218 218         # m**2 s**-3 | m s**-1
grib1 vp 2 3055               # m**2 s**-1 | Pa
# (!) 'Velocity potential' vs 'Vapour pressure'
grib1 vv 150143 151144        # m s**-2 | m**2 s**-2
# (!) 'V*V product' vs 'VV product'
grib2 apt 151181 210120       # deg C | m
# (!) 'Analysed potential temperature' vs 'Plume top height above surface'
grib2 asn 228032 32           # % | (0 - 1)
grib2 cp 143 228143           # m | kg m**-2
grib2 dhcc 130216 216         # K s**-1 | K
grib2 dhlc 130217 217         # K s**-1 | K
# (!) 'Diabatic heating by large-scale condensation' vs 'Diabatic heating large-scale condensation'
grib2 dhr 130214 214          # K s**-1 | K
grib2 dhvd 130215 215         # K s**-1 | K
grib2 ewgd 130220 220         # m**2 s**-3 | m s**-1
# (!) 'East-West gravity wave drag' vs 'East-West gravity wave drag tendency'
grib2 hcc 188 3075            # (0 - 1) | %
grib2 htcc 130225 225         # kg kg**-1 s**-1 | kg kg**-1
grib2 htlc 130226 226         # kg kg**-1 s**-1 | kg kg**-1
grib2 lcc 186 3073            # (0 - 1) | %
grib2 lsp 142 3062            # m | kg m**-2
grib2 mcc 187 3074            # (0 - 1) | %
grib2 nsgd 130221 221         # m**2 s**-3 | m s**-1
# (!) 'North-South gravity wave drag' vs 'North-South gravity wave drag tendency'
grib2 ra 171157 210181        # % | kg kg**-1
# (!) 'Relative humidity anomaly' vs 'Radon'
grib2 sd 141 228141           # m of water equivalent | kg m**-2
# (!) 'Snow depth' vs 'Snow depth water equivalent'
grib2 sf 144 228144           # m of water equivalent | kg m**-2
# (!) 'Snowfall' vs 'Snowfall water equivalent'
grib2 swl2 170171 171         # m | m of water equivalent
grib2 tcc 164 228164          # (0 - 1) | %
grib2 tmax 140217 3015        # s | K
# (!) 'Period corresponding to maximum individual wave height' vs 'Maximum temperature'
grib2 tp 228 228228           # m | kg m**-2
grib2 uu 150142 151143        # m s**-2 | m**2 s**-2
# (!) 'U*U product' vs 'UU product'
grib2 uv 150139 151140        # m s**-2 | m**2 s**-2
# (!) 'U*V product' vs 'UV product'
grib2 uvi 151187 260094       # m s**-1 per time step | Numeric
# (!) 'Zonal Velocity increment (from balance operator)' vs 'UV index'
grib2 vdh 130224 224          # kg kg**-1 s**-1 | kg kg**-1
grib2 vdmw 130219 219         # m**2 s**-3 | m s**-1
grib2 vdzw 130218 218         # m**2 s**-3 | m s**-1
grib2 veg 199 260180          # (0 - 1) | %
# (!) 'Vegetation fraction' vs 'Vegetation'
grib2 vv 150143 151144        # m s**-2 | m**2 s**-2
# (!) 'V*V product' vs 'VV product'
EOF

# (2) NOT a units change: the same unit spelled differently, or one side has
#     no usable unit ('~' means absent; '(Code table N)' is an encoded value,
#     not a unit).  Annotation: the unit of each paramId.
cat >> $tempList <<'EOF'
# --- same unit, spelled differently (units are compared as text)
grib1 ut 150140 151141        # m s**-1 deg C | m s**-1 degC
grib2 ut 150140 151141        # m s**-1 deg C | m s**-1 degC

# --- no usable unit on one side
grib1 lspa 171142 171152      # m | ~
grib1 nsf 150171 151156       # ~ | J m**-2
grib2 cdct 260104 260108      # (Code table 4.203) | m
grib2 lspa 171142 171152      # m | ~
grib2 nsf 150171 151156       # ~ | J m**-2
EOF

# (3) NOT a units change: a key coded in the message differs as well -- one
#     entry adds a condition the other omits (level, fixed surfaces,
#     statistical processing, reference period).  Grouped by the extra keys;
#     '...' means more keys differ than the ones named.
#     In GRIB1 the MARS-008 example 'tp' is here: 228228 also pins level 0.
cat >> $tempList <<'EOF'
# --- indicatorOfTypeOfLevel, level
grib1 tp 228 228228           # m | kg m**-2
# --- typeOfStatisticalProcessing
grib2 ro 205 228205           # m | kg m**-2
grib2 tprate 172228 260048    # m s**-1 | kg m**-2 s**-1
# --- typeOfGeneratingProcess
grib2 al 174 260509           # (0 - 1) | %
# --- typeOfFirstFixedSurface
grib2 sst 151159 34           # deg C | K
# --- typeOfFirstFixedSurface, typeOfGeneratingProcess
grib2 sr 173 190173           # m | (0 - 1)
# --- typeOfFirstFixedSurface, typeOfSecondFixedSurface
grib2 sigmat 151138 262502    # kg m**-3 | kg m**-3
grib2 so 151130 262500        # psu | g kg**-1
grib2 thetao 151129 262501    # deg C | K
# --- typeOfFirstFixedSurface, typeOfStatisticalProcessing
grib2 par 217009 58           # kg kg**-1 | J m**-2
grib2 sro 174008 8            # kg m**-2 | m
grib2 ssr 176 180176          # J m**-2 | J m**-2
grib2 str 177 180177          # J m**-2 | J m**-2
grib2 tsr 178 180178          # J m**-2 | J m**-2
grib2 ttr 170179 179 180179   # J m**-2 | J m**-2 | J m**-2
# --- probabilityType, scaleFactorOfFirstFixedSurface, ...
grib2 10spg10 131068 133021   # % | %
grib2 10spg15 131069 133022   # % | %
# --- scaleFactorOfFirstFixedSurface, scaleFactorOfSecondFixedSurface, ...
grib2 cap 190170 228170       # (0 - 1) | kg m**-3
grib2 stfbarot 151147 262107  # m**3 s**-1 | m**3 s**-1
grib2 t20d 151163 262104      # m | m
# --- scaleFactorOfSecondFixedSurface, scaledValueOfSecondFixedSurface, ...
grib2 wo 151133 262507        # m s**-1 | m s**-1
# --- numberOfAdditionalParametersForReferencePeriod,
#     typeOfRelationToReferenceDataset
grib2 fala 171243 171265      # (0 - 1) | %
grib2 hcca 171188 171258      # (0 - 1) | %
grib2 lcca 171186 171256      # (0 - 1) | %
grib2 mcca 171187 171257      # (0 - 1) | %
grib2 o3s 217024 234203       # kg kg**-1 | kg kg**-1
grib2 pta 151211 171003       # deg C | K
grib2 sda 171141 171263       # m of water equivalent | kg m**-2
grib2 sia 171212 171264       # J m**-2 | m s**-1
# --- numberOfAdditionalParametersForReferencePeriod,
#     typeOfRelationToReferenceDataset, ...
grib2 roara 173205 173258     # m s**-1 | kg m**-2 s**-1
# --- numberOfAdditionalParametersForReferencePeriod,
#     typeOfFirstFixedSurface, ...
grib2 evara 173182 173262     # m of water equivalent s**-1 | kg m**-2 s**-1
grib2 lspara 173142 173259    # m s**-1 | kg m**-2 s**-1
grib2 mcpra 173143 173260     # m s**-1 | kg m**-2 s**-1
grib2 sfara 173144 173261     # m of water equivalent s**-1 | kg m**-2 s**-1
grib2 tcca 171164 171259      # (0 - 1) | %
grib2 tpara 173228 173263     # m s**-1 | kg m**-2 s**-1
EOF

# (4) NOT a units change: same unit, and the same coded keys apart from the
#     ones that number the parameter (discipline, parameterCategory,
#     parameterNumber, localTablesVersion, indicatorOfParameter,
#     table2Version, localElementNumber) -- usually one quantity defined
#     again in another (local) table under a new paramId.
cat >> $tempList <<'EOF'
grib1 10spg10 131068 133021   # % | %
# (!) '10 metre Wind speed of at least 10 m/s' vs '10 metre wind speed probability of at least 10 m/s'
grib1 10spg15 131069 133022   # % | %
# (!) '10 metre Wind speed of at least 15 m/s' vs '10 metre wind speed probability of at least 15 m/s'
grib1 mld 150154 151148 3067  # m | m | m
grib1 mpww 140236 3103        # s | s
grib1 rn 150137 151139        # ~ | ~
grib1 shww 140234 3102        # m | m
grib1 ssr 176 180176          # J m**-2 | J m**-2
# (!) 'Surface net short-wave (solar) radiation' vs 'Surface net solar radiation'
grib1 str 177 180177          # J m**-2 | J m**-2
# (!) 'Surface net long-wave (thermal) radiation' vs 'Surface net thermal radiation'
grib1 swh 140229 3100         # m | m
grib1 tsr 178 180178          # J m**-2 | J m**-2
# (!) 'Top net short-wave (solar) radiation' vs 'Top net solar radiation'
grib1 tsw 170149 180149       # m | m
# (!) 'Total soil moisture' vs 'Total soil wetness'
grib1 ttr 170179 179 180179   # J m**-2 | J m**-2 | J m**-2
# (!) 'Top net long-wave (thermal) radiation' vs 'Top net thermal radiation'
grib1 vt 150141 151142        # m s**-1 deg C | m s**-1 deg C
# (!) 'V*T product' vs 'VT product'
grib2 mld 150154 151148 3067  # m | m | m
grib2 rn 150137 151139        # ~ | ~
grib2 sithick 174098 262000   # m | m
# (!) 'Sea ice thickness' vs 'Sea-ice thickness'
grib2 tcond 260017 260113     # kg kg**-1 | kg kg**-1
# (!) 'Condensate' vs 'Total condensate'
grib2 tsec 260168 260241      # s | s
grib2 tsw 170149 180149       # m | m
# (!) 'Total soil moisture' vs 'Total soil wetness'
grib2 vt 150141 151142        # m s**-1 deg C | m s**-1 deg C
# (!) 'V*T product' vs 'VT product'
grib2 zos 151145 262124       # m | m
EOF

# Expected output: drop comments and empty lines, squeeze blanks to one space,
# sort.  LC_ALL=C on both sorts in this script, so the two orders agree.
LC_ALL=C awk 'BEGIN{OFS=" "} {sub(/#.*/, ""); if (NF) { $1 = $1; print }}' $tempList |
    LC_ALL=C sort -u > $tempRef

# ---------------------------------------------------------------------------
# Collect "edition shortName paramId" from the definition files
# ---------------------------------------------------------------------------
# Each concept file is a sequence of blocks
#     'value' = { key=value; key=value; ... }
# paramId.def and shortName.def describe the same parameters with the same
# blocks, so a shortName belongs to the paramId whose block has the same
# signature: same file pair, same keys (whitespace removed), and -- for blocks
# that repeat -- the same occurrence number.  The paramId file must be read
# before its shortName file.  shortName blocks whose value is '~' or '' are
# placeholders and are skipped.  Exit status 1 if a shortName block has no
# paramId block.
cat > $tempAwk <<'EOF'
BEGIN { OFS = "\t" }
FNR == 1 {
    # paramId.def and shortName.def (and their .legacy variants) get the
    # same "file" name, so their blocks match.
    file = FILENAME
    sub(/\/(paramId|shortName)\./, "/.", file)
    kind = FILENAME
    sub(/^.*\//, "", kind)
    sub(/\..*$/, "", kind)    # paramId or shortName
}
/^[ \t]*#/ { next }
# Block opens:  'value' = {
/^[ \t]*'.*'[ \t]*=[ \t]*[{]/ {
    value = $0
    sub(/^[ \t]*'/, "", value)
    sub(/'[ \t]*=.*$/, "", value)
    block = ""
    next
}
# Block closes:  }
/^[ \t]*[}]/ {
    key = file SUBSEP block
    key = key SUBSEP (++count[kind, key])
    if (kind == "paramId")
        pid[key] = value
    else if (value != "" && value != "~") {
        if (key in pid)
            print edition, tolower(value), pid[key]
        else {
            msg = "ERROR: no paramId for shortName " value " in " FILENAME
            print msg > "/dev/stderr"
            bad = 1
        }
    }
    next
}
# Inside a block: accumulate the keys, without blanks
{ gsub(/[ \t]/, ""); block = block $0 }
END { exit bad }
EOF

for edition in grib1 grib2; do
    files=""
    for dir in $ECCODES_DEFINITION_PATH/$edition \
               $ECCODES_DEFINITION_PATH/$edition/localConcepts/ecmf; do
        for variant in "" .legacy; do
            if [ -f $dir/paramId$variant.def ]; then
                # shortName last: the awk program relies on this order
                files="$files $dir/paramId$variant.def $dir/shortName$variant.def"
            fi
        done
    done
    awk -v edition=$edition -f $tempAwk $files
done > $tempPairs

# ---------------------------------------------------------------------------
# Compare: one line per shortName that maps to more than one paramId
# ---------------------------------------------------------------------------
# Sorted and unique, the entries of a shortName are adjacent, one per paramId.
LC_ALL=C sort -u $tempPairs | awk -F '\t' '
    function flush() { if (n > 1) print line }
    $1 " " $2 != group {
        flush()
        group = $1 " " $2; line = group; n = 0
    }
    { line = line " " $3; n++ }
    END { flush() }' > $tempOut

if ! diff $tempRef $tempOut; then
    echo "ERROR: shared shortNames changed ('>' new, '<' fixed): update the list in $0" >&2
    exit 1
fi

rm -f $tempList $tempRef $tempOut $tempPairs $tempAwk
