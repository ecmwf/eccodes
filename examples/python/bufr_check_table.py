# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
#
# Description: Verify the given BUFR table (B or D) are correctly formed for ecCodes
#

import os
import sys

import numpy as np

MIN_NUM_COLUMNS = 8
LEN_DESCRIPTOR = 6  # FXY
ALLOWED_TYPES = ("long", "double", "table", "flag", "string")


def isNumeric(a_str):
    result = True
    try:
        int(a_str)
    except ValueError:
        result = False
    return result


def verify_table(table, filename):
    if table == "D":
        raise NotImplementedError("Table D verification: Not yet implemented.")

    data = np.genfromtxt(
        filename, comments="#", dtype=str, delimiter="|", usecols=np.arange(0, 8)
    )

    # If there is only 1 row, then we get a plain list and not list of lists
    if len(data.shape) == 1:
        data = np.asarray([data])

    for i, a_row in enumerate(data):
        # print(f"Checking {a_row}")
        linenum = 1 + i
        numcols = len(a_row)
        if numcols < MIN_NUM_COLUMNS:
            print(f"Error in line {linenum}: Column count (={numcols}) < required miniumum (={MIN_NUM_COLUMNS})")
            return 1
        a_code = a_row[0]
        a_key = a_row[1]
        a_type = a_row[2]
        a_scale = a_row[5]
        a_ref = a_row[6]
        a_width = a_row[7]
        if not isNumeric(a_code):
            print(f"Error in line {linenum}: Code '{a_code}' (column 1) is not numeric")
            return 1
        if len(a_code) != LEN_DESCRIPTOR:
            print(f"Error in line {linenum}: Code '{a_code}' (column 1) should be {LEN_DESCRIPTOR} digits long")
            print("\tAll descriptor codes (FXY) must have 1 digit for F, 2 for X and 3 for Y")
            return 1
        if not a_key:
            print(f"Error in line {linenum}: Key '{a_key}' (column 2) is not valid")
        if a_type not in ALLOWED_TYPES:
            print(f"Error in line {linenum}: Type '{a_type}' (column 3) is not valid")
            print(f"\tPlease choose from: {ALLOWED_TYPES}")
            return 1
        if not isNumeric(a_scale):
            print(f"Error in line {linenum}: Scale '{a_scale}' (column 6) is not numeric")
            return 1
        if not isNumeric(a_ref):
            print(f"Error in line {linenum}: Reference '{a_ref}' (column 7) is not numeric")
            return 1
        if not isNumeric(a_width):
            print(f"Error in line {linenum}: Width '{a_width}' (column 8) is not numeric")
            return 1

    print("All OK")
    return 0


def usage():
    progname = os.path.basename(sys.argv[0])
    print("Usage: ", progname, "option file")
    print("Options:")
    print("\t-B:  Input is Table B (element.table)")
    print("\t-D:  Input is Table D (sequence.def)")
    print("")


def main():
    if len(sys.argv) != 3:
        usage()
        return 1
    if sys.argv[1] == "-B":
        table = "B"
    elif sys.argv[1] == "-D":
        table = "D"
    else:
        assert False, "Invalid option. Specify -B or -D"
    filename = sys.argv[2]

    print(f"Processing {filename}: Table{table}")
    status = verify_table(table, filename)
    return status


if __name__ == "__main__":
    sys.exit(main())
