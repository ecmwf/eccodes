#!/usr/bin/env python3

import yaml
import shutil
import os
import sys
import argparse
from jsonschema import validate, ValidationError

# =========================================================
# User settings
# =========================================================
INPUT_YAML = "grib2/localConcepts/ecmf/inputProcessIdentifierList.yaml"

# Output files in sub-folder
NAME_OUTPUT = "grib2/localConcepts/ecmf/inputModelNameConcept.def"
VERSION_OUTPUT = "grib2/localConcepts/ecmf/inputModelVersionConcept.def"

# Formatting toggles
ADD_BLANK_LINE_BEFORE_MODEL = True
INCLUDE_ID_RANGE_IN_COMMENT = True

# Copy settings
ENABLE_COPY = True
DEST_FOLDERS = [
    "grib2/localConcepts/hydro",
]

# =========================================================
# CLI
# =========================================================
parser = argparse.ArgumentParser(
    description=f"Generate {NAME_OUTPUT} and {VERSION_OUTPUT} from YAML"
)

parser.add_argument(
    "--dry-run",
    action="store_true",
    help="Process YAML but do not write files"
)

parser.add_argument(
    "--validate-only",
    action="store_true",
    help="Only validate YAML and exit"
)

args = parser.parse_args()

# =========================================================
# YAML Schema
# =========================================================
SCHEMA = {
    "type": "object",
    "required": ["models"],
    "properties": {
        "models": {
            "type": "array",
            "items": {
                "type": "object",
                "required": ["name", "start"],
                "additionalProperties": False,
                "properties": {
                    "name": {"type": "string"},
                    "start": {"type": "integer"},
                    "extra_keys": {
                        "type": "object",
                        "additionalProperties": {
                            "type": ["string", "number", "boolean"]
                        }
                    },
                    "versions": {
                        "type": "array",
                        "items": {
                            "oneOf": [
                                {"type": ["string", "null"]},
                                {
                                    "type": "object",
                                    "additionalProperties": False,
                                    "properties": {
                                        "version": {"type": ["string", "null"]},
                                        "skip": {"type": "boolean"},
                                        "override_id": {"type": "integer"},
                                        "extra_keys": {
                                            "type": "object",
                                            "additionalProperties": {
                                                "type": ["string", "number", "boolean"]
                                            }
                                        }
                                    }
                                }
                            ]
                        }
                    }
                }
            }
        }
    }
}

# =========================================================
# Ensure output folders exist (if needed)
# =========================================================
for path in (NAME_OUTPUT, VERSION_OUTPUT):
    d = os.path.dirname(path)
    if d:
        os.makedirs(d, exist_ok=True)

# =========================================================
# Read & validate YAML
# =========================================================
try:
    with open(INPUT_YAML, "r") as f:
        data = yaml.safe_load(f)
except Exception as e:
    sys.exit(f"ERROR reading YAML: {e}")

try:
    validate(instance=data, schema=SCHEMA)
except ValidationError as e:
    sys.exit(f"YAML SCHEMA VALIDATION ERROR:\n{e.message}")

if args.validate_only:
    print("YAML validation successful.")
    sys.exit(0)

# =========================================================
# Build blocks
# =========================================================
blocks = []
used_ids = {}  # id -> "MODEL VERSION"

for model in data["models"]:
    model_name = model["name"]
    start_id = int(model["start"])
    versions = model.get("versions", [])
    block_extra = model.get("extra_keys", {})

    entries = []
    current_id = start_id

    for v in versions:
        if isinstance(v, dict):
            version = v.get("version") or model_name
            version_extra = v.get("extra_keys", {})
            skip = v.get("skip", False)
            override_id = v.get("override_id")
        else:
            version = v if v else model_name
            version_extra = {}
            skip = False
            override_id = None

        if override_id is not None:
            id_val = override_id
        else:
            id_val = current_id

        combined_extra = {**block_extra, **version_extra}

        if not skip:
            if id_val in used_ids:
                sys.exit(
                    "ID COLLISION DETECTED:\n"
                    f"  ID {id_val} used by:\n"
                    f"    - {used_ids[id_val]}\n"
                    f"    - {model_name} {version}"
                )

            used_ids[id_val] = f"{model_name} {version}"
            entries.append((model_name, version, id_val, combined_extra))

        if override_id is None:
            current_id += 1

    blocks.append({
        "model": model_name,
        "entries": entries
    })

# =========================================================
# Sort blocks
# =========================================================
blocks.sort(key=lambda b: b["entries"][0][2] if b["entries"] else float("inf"))

# =========================================================
# Generate output
# =========================================================
name_lines = []
version_lines = []

for block in blocks:
    if not block["entries"]:
        continue

    model = block["model"]
    ids = [e[2] for e in block["entries"]]
    start_id = min(ids)
    end_id = max(ids)

    prefix = "\n" if ADD_BLANK_LINE_BEFORE_MODEL else ""
    #header = f"{prefix}# MODEL {model}"
    header = f"{prefix}# {model}"
    if INCLUDE_ID_RANGE_IN_COMMENT:
        header += f" (ID range: {start_id}-{end_id})"
    header += "\n"

    name_lines.append(header)
    version_lines.append(header)

    for model_name, version, id_val, extra in block["entries"]:
        extra_str = "".join(f"{k}={v};" for k, v in extra.items())

        name_lines.append(
            f"'{model_name}' = {{inputProcessIdentifier={id_val};{extra_str}}}\n"
        )

        if version == model_name:
            version_lines.append(
                f"'{model_name}' = {{inputProcessIdentifier={id_val};{extra_str}}}\n"
            )
        else:
            version_lines.append(
                f"'{model_name}-{version}' = {{inputProcessIdentifier={id_val};{extra_str}}}\n"
            )

# =========================================================
# Write files
# =========================================================
if args.dry_run:
    print("Dry run enabled — no files written.")
else:
    for path in (NAME_OUTPUT, VERSION_OUTPUT):
        d = os.path.dirname(path)
        if d:
            os.makedirs(d, exist_ok=True)

    with open(NAME_OUTPUT, "w") as f:
        f.writelines(name_lines)
    print(f"Created file: {NAME_OUTPUT}")

    with open(VERSION_OUTPUT, "w") as f:
        f.writelines(version_lines)
    print(f"Created file: {VERSION_OUTPUT}")

    if ENABLE_COPY:
        for folder in DEST_FOLDERS:
            os.makedirs(folder, exist_ok=True)
            shutil.copy(NAME_OUTPUT, folder)
            shutil.copy(VERSION_OUTPUT, folder)

