# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

import getopt
import os
import re
import sys
import traceback

from eccodes import *

VERBOSE = 1  # verbose error reporting
DEBUG = 0


def usage():
    progname = os.path.basename(sys.argv[0])
    print("Usage: ", progname, "[options] bufr_file")
    print("Options:")
    print("\t-e generate the file defining Table B descriptors (element.table)")
    print("\t-s generate the file defining Table D descriptors (sequence.def)")
    print("")


def infer_type(units, scale):
    ktype = "double"
    if re.search("^CCITT", units) or re.search("^CHARACTER", units):
        return "string"
    if re.search("CODE\s*TABLE", units):
        return "table"
    if re.search("FLAG\s*TABLE", units):
        return "flag"
    if re.search("NUMERIC", units) or int(scale) <= 0:
        return "long"
    return ktype


def generate_tables(INPUT, what):
    assert what in ("element", "sequence")

    f = open(INPUT, "rb")

    cnt = 0
    if what == "element":
        print("#code|abbreviation|type|name|unit|scale|reference|width")

    # loop for the messages in the file
    while 1:
        # get handle for message
        bufr = codes_bufr_new_from_file(f)
        if bufr is None:
            break

        # Sanity check
        if codes_get(bufr, "dataCategory") != 11:
            # print('BUFR message dataCategory must be 11 (BUFR tables). Ignoring')
            codes_release(bufr)
            continue
        if codes_get(bufr, "numberOfSubsets") == 0:
            # When we reach a message whose number of subsets == 0, we're done.
            # All messages after this one encode observations (not tables)
            codes_release(bufr)
            break

        if DEBUG:
            print(f"Processing message {cnt+1}")
        codes_set(bufr, "unpack", 1)

        # Each table message contains three delayed replications (031001):
        # First is for Table A (skipped), second for Table B and third for Table D

        replications = codes_get_array(bufr, "delayedDescriptorReplicationFactor")
        fDesc = codes_get_array(bufr, "fDescriptorToBeAddedOrDefined")
        xDesc = codes_get_array(bufr, "xDescriptorToBeAddedOrDefined")
        yDesc = codes_get_array(bufr, "yDescriptorToBeAddedOrDefined")

        assert len(fDesc) == len(xDesc)
        assert len(fDesc) == len(yDesc)
        assert len(replications) > 0
        assert len(fDesc) == replications[1] + replications[2]

        if what == "element":
            try:
                elementName1 = codes_get_array(bufr, "elementNameLine1")
                elementName2 = codes_get_array(bufr, "elementNameLine2")
                unitsName = codes_get_array(bufr, "unitsName")
                unitsScaleSign = codes_get_array(bufr, "unitsScaleSign")
                unitsScale = codes_get_array(bufr, "unitsScale")
                unitsReferenceSign = codes_get_array(bufr, "unitsReferenceSign")
                unitsReferenceValue = codes_get_array(bufr, "unitsReferenceValue")
                elementDataWidth = codes_get_array(bufr, "elementDataWidth")
            except CodesInternalError as err:
                if DEBUG:
                    print("No element descriptors found")
                pass

            for i in range(len(fDesc)):
                fd = fDesc[i]
                xd = xDesc[i]
                yd = yDesc[i]
                if fd == "0":
                    # ecCodes key (abbreviation) => first word of elementNameLine1
                    el1 = elementName1[i].rstrip()
                    key = str.split(el1)[0]
                    # description (name) => Combine rest of elementNameLine1 and elementNameLine2
                    desc = " ".join(str.split(el1)[1:]) + elementName2[i].rstrip()
                    if len(desc) == 0:
                        desc = key
                    units = unitsName[i].rstrip()
                    sign = unitsScaleSign[i]
                    scale = int(unitsScale[i])
                    if sign == "-":
                        scale = -1 * scale
                    sign = unitsReferenceSign[i]
                    reference = int(unitsReferenceValue[i])
                    if sign == "-":
                        reference = -1 * reference
                    width = elementDataWidth[i].rstrip()
                    ktype = infer_type(units, scale)
                    print(
                        f"{fd}{xd}{yd}|{key}|{ktype}|{desc}|{units}|{scale}|{reference}|{width}"
                    )
        if what == "sequence":
            try:
                descriptorDefiningSequence = codes_get_array(
                    bufr, "descriptorDefiningSequence"
                )
            except CodesInternalError as err:
                if DEBUG:
                    print("No sequence descriptors found")
                pass
            k = 0
            rindex = 3  # From index 3 onwards we have the contents of sequences
            for i in range(len(fDesc)):
                fd = fDesc[i]
                xd = xDesc[i]
                yd = yDesc[i]
                if fd == "3":
                    repls = replications[rindex]
                    if repls:
                        print(f'"{fd}{xd}{yd}" = [ ', end="")
                        comma = ","
                        for j in range(repls):
                            seq = descriptorDefiningSequence[k + j]
                            if j == repls - 1:
                                comma = ""
                            print(f"{seq}{comma} ", end="")
                        k = k + repls
                        rindex = rindex + 1
                        print("]")

        cnt += 1

        codes_release(bufr)

    f.close()


def main():
    if len(sys.argv) < 2:
        usage()
        return 1
    try:
        what = "element"
        options = "es"
        opts, args = getopt.getopt(sys.argv[1:], options)
        for o, a in opts:
            if o == "-e":
                what = "element"
            elif o == "-s":
                what = "sequence"
            else:
                assert False, "Invalid option"
        # Check we have a BUFR file to process
        if not args:
            usage()
            return 1
        for arg in args:
            generate_tables(arg, what)
    except getopt.GetoptError as err:
        print("Error: ", err)
        usage()
        return 1
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + "\n")

        return 1


if __name__ == "__main__":
    sys.exit(main())
