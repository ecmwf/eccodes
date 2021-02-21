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
#   scaleFactorOfFirstFixedSurface
#   scaledValueOfFirstFixedSurface
#   typeOfSecondFixedSurface
#   scaleFactorOfSecondFixedSurface
#   scaledValueOfSecondFixedSurface
#   typeOfStatisticalProcessing
#
# It outputs the def files:
#    name.def paramId.def shortName.def units.def cfVarName.def
#
#######################################################################
$|=1;
use strict;
use warnings;
$ARGV[0] or die "USAGE: $0 input.tsv\n";

my $WRITE_TO_FILES = 1;
my $WRITE_TO_PARAMDB = 0;

my ($paramId, $shortName, $name, $units, $cfVarName);
my ($discipline, $pcategory, $pnumber, $type1, $type2, $scaledValue1, $scaleFactor1, $scaledValue2, $scaleFactor2, $stat);

my $PARAMID_FILENAME   = "paramId.def";
my $SHORTNAME_FILENAME = "shortName.def";
my $NAME_FILENAME      = "name.def";
my $UNITS_FILENAME     = "units.def";
my $CFVARNAME_FILENAME = "cfVarName.def";

if ($WRITE_TO_FILES) {
    create_or_append(\*OUT_PARAMID,   "$PARAMID_FILENAME");
    create_or_append(\*OUT_SHORTNAME, "$SHORTNAME_FILENAME");
    create_or_append(\*OUT_NAME,      "$NAME_FILENAME");
    create_or_append(\*OUT_UNITS,     "$UNITS_FILENAME");
    create_or_append(\*OUT_CFVARNAME, "$CFVARNAME_FILENAME");
}

my $first = 1;
while (<>) {
    chomp;
    s/\r//g;  # Remove DOS carriage returns
    if ($first == 1) {
        check_first_row_column_names($_);
        $first = 0;
        next;
    }

    ($paramId, $shortName, $name, $units,
        $discipline, $pcategory, $pnumber, $type1, $type2,
        $scaledValue1, $scaleFactor1, $scaledValue2, $scaleFactor2, $stat) = split(/\t/);

    die "Error: paramID \"$paramId\" is not an integer!" if (!is_integer($paramId));

    $units = "~" if ($units eq "");
    $cfVarName = $shortName;
    $cfVarName = '\\'.$shortName if ($shortName =~ /^[0-9]/);

    if ($WRITE_TO_FILES) {
        write_out_file(\*OUT_PARAMID,   $name, $paramId);
        write_out_file(\*OUT_SHORTNAME, $name, $shortName);
        write_out_file(\*OUT_NAME,      $name, $name);
        write_out_file(\*OUT_UNITS,     $name, $units);
        write_out_file(\*OUT_CFVARNAME, $name, $cfVarName);
    }
}

if ($WRITE_TO_FILES) {
    print "Wrote output files: $PARAMID_FILENAME $SHORTNAME_FILENAME $NAME_FILENAME $UNITS_FILENAME $CFVARNAME_FILENAME\n";
    close(OUT_PARAMID)   or die "$PARAMID_FILENAME: $!";
    close(OUT_SHORTNAME) or die "$SHORTNAME_FILENAME: $!";
    close(OUT_NAME)      or die "$NAME_FILENAME: $!";
    close(OUT_UNITS)     or die "$UNITS_FILENAME: $!";
    close(OUT_CFVARNAME) or die "$CFVARNAME_FILENAME: $!";
}

# -------------------------------------------------------------------
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

sub check_first_row_column_names {
    my $line = shift; # This is the first row
    my @keys = split(/\t/, $line);
    die "Error: 1st row column titles wrong: Column 1 should be 'paramId'\n"   if ($keys[0] ne "paramId");
    die "Error: 1st row column titles wrong: Column 2 should be 'shortName'\n" if ($keys[1] ne "shortName");
    die "Error: 1st row column titles wrong: Column 3 should be 'name'\n"      if ($keys[2] ne "name");
    die "Error: 1st row column titles wrong: Column 4 should be 'units'\n"     if ($keys[3] ne "units");

    die "Error: 1st row column titles wrong: Column 5 should be 'discipline'\n"        if ($keys[4] ne "discipline");
    die "Error: 1st row column titles wrong: Column 6 should be 'parameterCategory'\n" if ($keys[5] ne "parameterCategory");
    die "Error: 1st row column titles wrong: Column 7 should be 'parameterNumber'\n"   if ($keys[6] ne "parameterNumber");

    die "Error: 1st row column titles wrong: Column 8 should be 'typeOfFirstFixedSurface'\n" if ($keys[7] ne "typeOfFirstFixedSurface");
    die "Error: 1st row column titles wrong: Column 9 should be 'scaleFactorOfFirstFixedSurface'\n" if ($keys[8] ne "scaleFactorOfFirstFixedSurface");
    die "Error: 1st row column titles wrong: Column 10 should be 'scaledValueOfFirstFixedSurface'\n" if ($keys[9] ne "scaledValueOfFirstFixedSurface");
    
    die "Error: 1st row column titles wrong: Column 11 should be 'typeOfSecondFixedSurface'\n" if ($keys[10] ne "typeOfSecondFixedSurface");
    die "Error: 1st row column titles wrong: Column 12 should be 'scaleFactorOfSecondFixedSurface'\n" if ($keys[11] ne "scaleFactorOfSecondFixedSurface");
    die "Error: 1st row column titles wrong: Column 13 should be 'scaledValueOfSecondFixedSurface'\n" if ($keys[12] ne "scaledValueOfSecondFixedSurface");
    die "Error: 1st row column titles wrong: Column 14 should be 'typeOfStatisticalProcessing'\n" if ($keys[13] ne "typeOfStatisticalProcessing");
}

sub create_or_append {
    my $outfile = $_[0];
    my $fname   = $_[1];

    if (-f "$fname") {
        open($outfile,  ">>$fname") or die "$fname: $!";
    } else {
        open($outfile,  ">$fname")  or die "$fname: $!";
    }
}

sub is_integer {
    my $val = shift;
    return ($val =~ /^\d+$/);
}
