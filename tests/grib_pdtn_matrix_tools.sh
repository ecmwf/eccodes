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

label="grib_pdtn_matrix_tools"
out_trace="${label}.trace.out"
out_diff="${label}.diff.out"
out_preflight="${label}.preflight.out"
out_wizard="${label}.wizard.out"
out_wizard_json="${label}.wizard.json"
out_wizard_wave_json="${label}.wizard.wave.json"
out_wizard_aerosol_json="${label}.wizard.aerosol.json"
out_wizard_chemical_optical_json="${label}.wizard.chemical_optical.json"
out_wizard_chemical_srcsink_json="${label}.wizard.chemical_srcsink.json"
out_wizard_satellite_json="${label}.wizard.satellite.json"

script_dir="${proj_dir}/tools"
trace_py="${script_dir}/codes_pdt_trace.py"
diff_py="${script_dir}/codes_pdtn_diff.py"
preflight_py="${script_dir}/codes_set_preflight.py"
wizard_py="${script_dir}/codes_encode_wizard.py"

if ! command -v python3 >/dev/null 2>&1; then
    echo "Skipping ${label}: python3 not available"
    exit 0
fi

if ! python3 -c 'import pandas' >/dev/null 2>&1; then
    echo "Skipping ${label}: pandas not available"
    exit 0
fi

sample_grib2="${ECCODES_SAMPLES_PATH}/GRIB2.tmpl"

# 1) trace: canonical deterministic -> ensemble -> ensemble-statistical path
python3 "$trace_py" -s 0 -k perturbationNumber typeOfStatisticalProcessing --show-candidates 2 > "$out_trace"
grep -q "Start PDTN: 0" "$out_trace"
grep -q "Reason: superset-min-extra" "$out_trace"
grep -q "Final PDTN: 11" "$out_trace"

# 2) diff: compare known transition 1 -> 11 and require a known interval key to appear
python3 "$diff_py" --from-pdtn 1 --to-pdtn 11 --only added > "$out_diff"
grep -q "PDTN diff: 1 -> 11" "$out_diff"
grep -q "Added keys" "$out_diff"
grep -q "lengthOfTimeRange" "$out_diff"

# 3) preflight: parse grib_set-style expression and predict final PDTN + low risk
python3 "$preflight_py" --input "$sample_grib2" --set "perturbationNumber=2,typeOfStatisticalProcessing=1" > "$out_preflight"
grep -q "Final PDTN: 11" "$out_preflight"
grep -q "Risk summary: LOW" "$out_preflight"

# 4) wizard: scenario recommendation in text and JSON
python3 "$wizard_py" --scenario ensemble-statistical --startpdtn 0 > "$out_wizard"
grep -q "Scenario: ensemble-statistical" "$out_wizard"
grep -q "perturbationNumber" "$out_wizard"
grep -q "typeOfStatisticalProcessing" "$out_wizard"

python3 "$wizard_py" --scenario radar --json > "$out_wizard_json"
python3 - "$out_wizard_json" <<'PY'
import json
import sys

with open(sys.argv[1], encoding="utf-8") as handle:
    doc = json.load(handle)

assert doc["scenario"] == "radar"
assert "numberOfRadarSitesUsed" in doc["recommended_keys"]
assert "commands" in doc and "guard" in doc["commands"]
PY

python3 "$wizard_py" --scenario satellite --json > "$out_wizard_satellite_json"
python3 - "$out_wizard_satellite_json" <<'PY'
import json
import sys

with open(sys.argv[1], encoding="utf-8") as handle:
    doc = json.load(handle)

assert doc["scenario"] == "satellite"
assert "satelliteNumber" in doc["recommended_keys"]
PY

python3 "$wizard_py" --scenario wave-spectra --json > "$out_wizard_wave_json"
python3 - "$out_wizard_wave_json" <<'PY'
import json
import sys

with open(sys.argv[1], encoding="utf-8") as handle:
    doc = json.load(handle)

assert doc["scenario"] == "wave-spectra"
assert (
    "numberOfWaveDirections" in doc["recommended_keys"] or
    "numberOfWaveFrequencies" in doc["recommended_keys"]
)
PY

python3 "$wizard_py" --scenario aerosol --json > "$out_wizard_aerosol_json"
python3 - "$out_wizard_aerosol_json" <<'PY'
import json
import sys

with open(sys.argv[1], encoding="utf-8") as handle:
    doc = json.load(handle)

assert doc["scenario"] == "aerosol"
assert "constituentType" in doc["recommended_keys"]
assert "typeOfSizeInterval" in doc["recommended_keys"]
PY

python3 "$wizard_py" --scenario chemical_optical --json > "$out_wizard_chemical_optical_json"
python3 - "$out_wizard_chemical_optical_json" <<'PY'
import json
import sys

with open(sys.argv[1], encoding="utf-8") as handle:
    doc = json.load(handle)

assert doc["scenario"] == "chemical_optical"
assert "constituentType" in doc["recommended_keys"]
assert "typeOfWavelengthInterval" in doc["recommended_keys"]
PY

python3 "$wizard_py" --scenario chemical_srcSink --json > "$out_wizard_chemical_srcsink_json"
python3 - "$out_wizard_chemical_srcsink_json" <<'PY'
import json
import sys

with open(sys.argv[1], encoding="utf-8") as handle:
    doc = json.load(handle)

assert doc["scenario"] == "chemical_srcSink"
assert "constituentType" in doc["recommended_keys"]
assert "sourceSinkChemicalPhysicalProcess" in doc["recommended_keys"]
PY

rm -f "$out_trace" "$out_diff" "$out_preflight" "$out_wizard" "$out_wizard_json" "$out_wizard_wave_json" "$out_wizard_aerosol_json" "$out_wizard_chemical_optical_json" "$out_wizard_chemical_srcsink_json" "$out_wizard_satellite_json"
exit 0
