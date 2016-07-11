#!/usr/bin/env perl
use Data::Dumper;

$|=1;
my $debug = 0;

if (scalar @ARGV < 2) {
  &usage;
}

# Open grib_api.h and extract all macros of the form
#  #define GRIB.*  number
#
my $grib_header_file = $ARGV[0];
my $ecco_header_file = $ARGV[1];

open GRIB_HEADER_FILE, $grib_header_file or die $!;
my @grib_lines = <GRIB_HEADER_FILE>;
close GRIB_HEADER_FILE;

# Store each GRIB_ macro and its value
foreach (@grib_lines) {
    if (/^\s*#define\s+(GRIB[_A-z0-9]+)\s+(.+)/) {
        my $grib_macro = $1;
        my $grib_value = $2;
        #print "m=|$grib_macro| \t\t v=|$grib_value|\n";
        $macro_map{$grib_macro} = $grib_value;
    }
}
#print Data::Dumper->Dump([\%macro_map], ["macro_map"]), $/ if ($debug);

open ECCO_HEADER_FILE, $ecco_header_file or die $!;
my @ecco_lines = <ECCO_HEADER_FILE>;
close ECCO_HEADER_FILE;
# Apply the value from our map to the equivalent CODES_ macros
foreach (@ecco_lines) {
    if (/^\s*#define\s+(CODES[_A-z0-9]+)\s+(GRIB.+)/) {
        my $ecco_macro = $1;
        my $grib_macro = $2;
        print "#define $ecco_macro $macro_map{$grib_macro}\n";
    }
    #elsif (/^\s*typedef\s+struct\s+(grib_.*)\s+(codes_.*) *;/) {
    #    my $grib_struct = $1;
    #    my $ecco_struct = $2;
    #    print "typedef struct $ecco_struct $ecco_struct;\n";
    #}
    else {
        print;
    }
}

###################################################
sub usage {
   print "$0 grib_api.h eccodes.h\n";
   exit 1
}
