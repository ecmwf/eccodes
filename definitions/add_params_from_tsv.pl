#!/usr/bin/env perl
#
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#
#######################################################################
# Script to generate parameter definition files for GRIB2.
# Reads an input TSV (tab-separated-value) file which should contain
# the following parameter keys as columns:
#   paramId
#   shortName
#   name
#   units
#   discipline
#   parameterCategory
#   parameterNumber
#   # The following are optional keys
#   typeOfFirstFixedSurface
#   typeOfSecondFixedSurface
#   scaledValueOfFirstFixedSurface
#   scaleFactorOfFirstFixedSurface
#   scaledValueOfSecondFixedSurface
#   scaleFactorOfSecondFixedSurface
#   typeOfStatisticalProcessing
#
# It outputs the def files: name.def paramId.def shortName.def units.def
#
#######################################################################
$|=1;
use strict;
use warnings;
$ARGV[0] or die "USAGE: $0 input.tsv\n";

my ($paramId, $shortName, $name, $units);
my ($discipline, $pcategory, $pnumber, $type1, $type2, $scaledValue1, $scaleFactor1, $scaledValue2, $scaleFactor2, $stat);

my $PARAMID_FILENAME   = "paramId.def";
my $SHORTNAME_FILENAME = "shortName.def";
my $NAME_FILENAME      = "name.def";
my $UNITS_FILENAME     = "units.def";

open(OUT_PARAMID,  ">$PARAMID_FILENAME")   or die "$PARAMID_FILENAME: $!";
open(OUT_SHORTNAME,">$SHORTNAME_FILENAME") or die "$SHORTNAME_FILENAME: $!";
open(OUT_NAME,     ">$NAME_FILENAME")      or die "$NAME_FILENAME: $!";
open(OUT_UNITS,    ">$UNITS_FILENAME")     or die "$UNITS_FILENAME: $!";

my $first = 1;
while (<>) {
    chomp;
    if ($first == 1) {
        die "Error: first line of input must contain the key names" if ( $_ !~ /^paramId/ );
        $first = 0;
        next;
    }

    ($paramId, $shortName, $name, $units,
        $discipline, $pcategory, $pnumber, $type1, $type2,
        $scaledValue1, $scaleFactor1, $scaledValue2, $scaleFactor2, $stat) = split(/\t/);

    die "Error: paramID \"$paramId\" is not an integer!" if (!is_integer($paramId));
    
    write_out_file(\*OUT_PARAMID,   $name, $paramId);
    write_out_file(\*OUT_SHORTNAME, $name, $shortName);
    write_out_file(\*OUT_NAME,      $name, $name);
    write_out_file(\*OUT_UNITS,     $name, $units);
}

print "Wrote output files: $PARAMID_FILENAME, $SHORTNAME_FILENAME, $NAME_FILENAME, $UNITS_FILENAME\n";
close(OUT_PARAMID)   or die "$PARAMID_FILENAME: $!";
close(OUT_SHORTNAME) or die "$SHORTNAME_FILENAME: $!";
close(OUT_NAME)      or die "$NAME_FILENAME: $!";
close(OUT_UNITS)     or die "$UNITS_FILENAME: $!";

sub write_out_file {
    my $outfile = $_[0];
    my $name    = $_[1];
    my $key     = $_[2];

    # Assuming every parameter definition has at least discipline, category and number
    print $outfile "#$name\n";
    print $outfile "'$key' = {\n";
    print $outfile "  discipline = $discipline ;\n";
    print $outfile "  parameterCategory = $pcategory ;\n";
    print $outfile "  parameterNumber = $pnumber ;\n";

    # Optional keys
    print $outfile "  typeOfFirstFixedSurface = $type1 ;\n"                if ($type1 ne "");
    print $outfile "  typeOfSecondFixedSurface = $type2 ;\n"               if ($type2 ne "");
    print $outfile "  scaledValueOfFirstFixedSurface = $scaledValue1 ;\n"  if ($scaledValue1 ne "");
    print $outfile "  scaleFactorOfFirstFixedSurface = $scaleFactor1 ;\n"  if ($scaleFactor1 ne "");
    print $outfile "  scaledValueOfSecondFixedSurface = $scaledValue2 ;\n" if ($scaledValue2 ne "");
    print $outfile "  scaleFactorOfSecondFixedSurface = $scaleFactor2 ;\n" if ($scaleFactor2 ne "");
    print $outfile "  typeOfStatisticalProcessing = $stat ;\n"             if ($stat ne "");
    print $outfile "}\n";
}

sub is_integer {
    my $val = shift;
    return ($val =~ /^\d+$/);
}
