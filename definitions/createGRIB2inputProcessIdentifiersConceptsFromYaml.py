#!/usr/bin/env python3

import yaml
import shutil
import os
import sys

# =========================================================
# User settings
# =========================================================
input_yaml = "grib2/localConcepts/ecmf/inputProcessIdentifierList.yaml"

# Output files in sub-folder
name_output = "grib2/localConcepts/ecmf/inputModelNameConcept.def"
version_output = "grib2/localConcepts/ecmf/inputModelVersionConcept.def"

# Formatting toggles
ADD_BLANK_LINE_BEFORE_MODEL = True
INCLUDE_ID_RANGE_IN_COMMENT = True

# Copy settings
ENABLE_COPY = True
DEST_FOLDERS = [
    "grib2/localConcepts/hydro",
]

# =========================================================
# Step 0: Ensure output folder exists
# =========================================================
os.makedirs(os.path.dirname(name_output), exist_ok=True)
os.makedirs(os.path.dirname(version_output), exist_ok=True)

# =========================================================
# Step 1: Read YAML
# =========================================================
try:
    with open(input_yaml, "r") as f:
        data = yaml.safe_load(f)
except Exception as e:
    sys.exit(f"Failed to read YAML file: {e}")

if not data or "models" not in data:
    sys.exit("YAML file must contain a top-level 'models' list")

models = data["models"]

# =========================================================
# Step 2: Build blocks
# =========================================================
blocks = []

for model_entry in models:
    try:
        model_name = model_entry["name"]
        start_id = int(model_entry["start"])
        versions = model_entry.get("versions", [])
        block_extra = model_entry.get("extra_keys", {})
    except KeyError as e:
        sys.exit(f"Missing required key in model entry: {e}")

    entries = []
    current_id = start_id

    for v in versions:
        # Determine version string and per-version extra_keys
        if isinstance(v, dict):
            version = v.get("version", model_name)
            version_extra = v.get("extra_keys", {})
        else:
            version = str(v).strip() if v is not None else model_name
            version_extra = {}

        # Merge block-level and version-level extra keys
        combined_extra = {**block_extra, **version_extra}

        entries.append((model_name, version, current_id, combined_extra))
        current_id += 1

    blocks.append({
        "model": model_name,
        "start_id": start_id,
        "entries": entries
    })

# =========================================================
# Step 3: Sort blocks by start ID
# =========================================================
blocks.sort(key=lambda b: b["start_id"])

# =========================================================
# Step 4: Detect ID collisions
# =========================================================
used_ranges = []

for block in blocks:
    start_id = block["start_id"]
    entries = block["entries"]
    end_id = entries[-1][2] if entries else start_id

    for used_start, used_end, used_model in used_ranges:
        if not (end_id < used_start or start_id > used_end):
            sys.exit(
                "ID collision detected:\n"
                f"  MODEL {block['model']} uses {start_id}-{end_id}\n"
                f"  MODEL {used_model} already uses {used_start}-{used_end}"
            )

    used_ranges.append((start_id, end_id, block["model"]))

# =========================================================
# Step 5: Generate output content
# =========================================================
name_lines = []
version_lines = []

for block in blocks:
    model = block["model"]
    entries = block["entries"]
    start_id = entries[0][2] if entries else 0
    end_id = entries[-1][2] if entries else start_id

    prefix = "\n" if ADD_BLANK_LINE_BEFORE_MODEL else ""
    comment = f"{prefix}# MODEL {model}"
    if INCLUDE_ID_RANGE_IN_COMMENT:
        comment += f" (ID range: {start_id}-{end_id})"
    comment += "\n"

    name_lines.append(comment)
    version_lines.append(comment)

    for model_name, version, id_val, extra_keys in entries:
        extra_str = "".join([f"{k}={v};" for k, v in extra_keys.items()]) if extra_keys else ""

        # inputModelNameConcept.def always uses just the model name
        name_lines.append(
            f"'{model_name}' = {{inputProcessIdentifier={id_val};{extra_str}}}\n"
        )

        # inputModelVersionConcept.def includes version unless same as model
        if version == model_name:
            version_lines.append(
                f"'{model_name}' = {{inputProcessIdentifier={id_val};{extra_str}}}\n"
            )
        else:
            version_lines.append(
                f"'{model_name} {version}' = {{inputProcessIdentifier={id_val};{extra_str}}}\n"
            )

# =========================================================
# Step 6: Write files
# =========================================================
with open(name_output, "w") as f:
    f.writelines(name_lines)
print(f"Created file: {name_output}")

with open(version_output, "w") as f:
    f.writelines(version_lines)
print(f"Created file: {version_output}")

# =========================================================
# Step 7: Optional copy
# =========================================================
if ENABLE_COPY:
    for folder in DEST_FOLDERS:
        os.makedirs(folder, exist_ok=True)
        shutil.copy(name_output, os.path.join(folder, os.path.basename(name_output)))
        shutil.copy(version_output, os.path.join(folder, os.path.basename(version_output)))

    print("Copied files to the following folders:")
    for folder in DEST_FOLDERS:
        print(f"  - {folder}")
else:
    print("Copying disabled.")

