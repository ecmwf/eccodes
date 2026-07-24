#!/usr/bin/env python3

import argparse
import json
import sys

SCENARIOS = {
    "ensemble": {
        "description": "Individual ensemble forecast",
        "keys": ["perturbationNumber"],
        "notes": [
            "Use perturbationNumber to move from deterministic to ensemble PDT family.",
            "You can add totalNumber/number later if your workflow needs them.",
        ],
    },
    "statistical": {
        "description": "Time-range statistical processing",
        "keys": ["typeOfStatisticalProcessing"],
        "notes": [
            "This usually selects a time-interval PDT (for example 0 -> 8).",
            "Set step/time range fields consistently after PDT switch.",
        ],
    },
    "ensemble-statistical": {
        "description": "Ensemble with time-range statistics",
        "keys": ["perturbationNumber", "typeOfStatisticalProcessing"],
        "notes": [
            "For this recommended key set, key order does not change the target template.",
            "Common path is 0 -> 1 -> 11.",
        ],
    },
    "probability": {
        "description": "Probability forecast",
        "keys": ["probabilityType"],
        "notes": [
            "Probability templates need additional probability bounds/type fields.",
            "Check missing keys with codes_pdt_trace.py.",
        ],
    },
    "radar": {
        "description": "Radar product",
        "keys": ["numberOfRadarSitesUsed"],
        "notes": [
            "Radar transitions may require fallback from some starting PDTs.",
            "Run codes_pdt_guard.py to enforce missing-key limits.",
        ],
    },
    "chemistry": {
        "description": "Atmospheric chemistry constituent",
        "keys": ["constituentType"],
        "notes": [
            "This usually moves into chemistry PDT families (for example around 40+).",
            "Use the aerosol scenario for aerosol-specific templates.",
        ],
    },
    "aerosol": {
        "description": "Aerosol size interval product",
        "keys": ["constituentType", "typeOfSizeInterval"],
        "notes": [
            "For this recommended key set, key order does not change the target template.",
            "typeOfSizeInterval drives aerosol size interval selection.",
            "Add size bounds and related aerosol metadata after PDT switch.",
        ],
    },
    "chemical_optical": {
        "description": "Chemical optical wavelength interval product",
        "keys": ["constituentType", "typeOfWavelengthInterval"],
        "notes": [
            "For this recommended key set, key order does not change the target template.",
            "typeOfWavelengthInterval drives optical wavelength interval selection.",
            "Add wavelength bounds and related metadata after PDT switch.",
        ],
    },
    "chemical_srcSink": {
        "description": "Chemical source/sink process product",
        "keys": ["constituentType", "sourceSinkChemicalPhysicalProcess"],
        "notes": [
            "For this recommended key set, key order does not change the target template.",
            "sourceSinkChemicalPhysicalProcess drives source/sink template selection.",
            "Add process-specific metadata after PDT switch.",
        ],
    },
    "wave-spectra": {
        "description": "Wave 2D spectra with explicit frequencies and directions",
        "keys": ["numberOfWaveDirections", "numberOfWaveFrequencies"],
        "notes": [
            "For this recommended key set, key order does not change the target template.",
            "This key set targets wave 2D spectra templates (typically PDT 99).",
            "Set direction/frequency arrays consistently after template switch.",
        ],
    },
    "wave-spectra-ensemble": {
        "description": "Ensemble wave 2D spectra with explicit frequencies and directions",
        "keys": ["perturbationNumber", "numberOfWaveDirections", "numberOfWaveFrequencies"],
        "notes": [
            "For this recommended key set, key order does not change the target template.",
            "This commonly routes to PDT 100 for ensemble wave 2D spectra.",
        ],
    },
    "waves": {
        "description": "Waves selected by period range (legacy alias of earlier wave-spectra setting)",
        "keys": ["typeOfWavePeriodInterval"],
        "notes": [
            "This scenario commonly routes to PDT 103 (period-range waves).",
            "Prefer explicit wave-spectra scenarios for PDT 99/100 workflows.",
        ],
    },
}


def parse_args():
    parser = argparse.ArgumentParser(
        description=(
            "Interactive/non-interactive wizard that maps encoding intent to a "
            "recommended key sequence and preflight commands."
        )
    )
    parser.add_argument(
        "--scenario",
        choices=sorted(SCENARIOS.keys()),
        help="Scenario name. If omitted, interactive selection is used.",
    )
    parser.add_argument(
        "--startpdtn",
        type=int,
        default=0,
        help="Starting PDTN used in generated command examples (default: 0).",
    )
    parser.add_argument(
        "--json",
        action="store_true",
        help="Emit machine-readable JSON recommendation.",
    )
    return parser.parse_args()


def choose_interactive():
    names = sorted(SCENARIOS.keys())
    print("Select an encoding scenario:")
    for idx, name in enumerate(names, start=1):
        print(f"  {idx}. {name} - {SCENARIOS[name]['description']}")

    while True:
        raw = input("Enter number: ").strip()
        try:
            choice = int(raw)
        except ValueError:
            print("Please enter a valid number.")
            continue
        if 1 <= choice <= len(names):
            return names[choice - 1]
        print("Choice out of range.")


def build_recommendation(scenario, start_pdtn):
    data = SCENARIOS[scenario]
    keys = data["keys"]
    keys_cli = " ".join(keys)
    set_expr = ",".join(f"{k}=<value>" for k in keys)

    return {
        "scenario": scenario,
        "description": data["description"],
        "start_pdtn": start_pdtn,
        "recommended_keys": keys,
        "notes": data["notes"],
        "commands": {
            "trace": f"python3 codes_pdt_trace.py -s {start_pdtn} -k {keys_cli}",
            "guard": f"python3 codes_pdt_guard.py -s {start_pdtn} -k {keys_cli}",
            "preflight": (
                "python3 codes_set_preflight.py "
                f"--startpdtn {start_pdtn} --set \"{set_expr}\""
            ),
        },
    }


def print_human(rec):
    print(f"Scenario: {rec['scenario']}")
    print(f"Description: {rec['description']}")
    print(f"Start PDTN: {rec['start_pdtn']}")
    print("Recommended key sequence:")
    for key in rec["recommended_keys"]:
        print(f"  - {key}")

    print("Notes:")
    for note in rec["notes"]:
        print(f"  - {note}")

    print("Next commands:")
    print("  - Trace:     " + rec["commands"]["trace"])
    print("  - Guard:     " + rec["commands"]["guard"])
    print("  - Preflight: " + rec["commands"]["preflight"])


def main():
    args = parse_args()

    scenario = args.scenario
    if not scenario:
        try:
            scenario = choose_interactive()
        except EOFError:
            print("Error: interactive input unavailable. Use --scenario.", file=sys.stderr)
            return 1

    rec = build_recommendation(scenario, args.startpdtn)

    if args.json:
        print(json.dumps(rec, indent=2, sort_keys=False))
    else:
        print_human(rec)

    return 0


if __name__ == "__main__":
    sys.exit(main())
