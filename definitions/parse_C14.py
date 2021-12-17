#!/usr/bin/env python3

import xml.etree.ElementTree as ET
import sys
import traceback
VERBOSE=0
infile_path = sys.argv[1]
tree = ET.parse(infile_path)
root = tree.getroot()
for anElement in root.iter('C14'):
    code    = anElement.find('CodeFigure').text
    meaning = anElement.find('Meaning_en').text
    formula = anElement.find('ChemicalFormula').text

    if "Reserved" in meaning or "-" in code:
        print(f"# {code} {meaning}")
    else:
        if formula:
            print(f"{code} {code} {meaning} {formula}")
        else:
            print(f"{code} {code} {meaning}")

            
