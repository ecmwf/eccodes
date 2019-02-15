#!/usr/bin/env perl

# Author: mow
# Date:   06/07/2010
# Updated: 08/07/2010
#
# Description:
# Since I could not find a way to tell doxygen not to include 
# some functions into the documentation, I have written this script
# to remove all the function I do not want documented from the source file
# before feeding it as input to doxygen. Such a pain.
#
# Update: Doxygen can be told to ignore some sections in the documentation
# by using the @cond,@endcond block, however that does not work with the 
# current version of doxygen used at the moment to create the documentation.

use strict; use warnings;

my @exclude = qw(
    with_numpy
    require
    grib_get_string
    grib_set_string
    grib_get_long
    grib_get_double
    grib_set_long
    grib_set_double
    grib_set_double_array
    grib_get_double_array
    grib_set_long_array
    grib_get_long_array
    grib_get_string_array
    grib_set_string_array
    grib_index_get_long
    grib_index_get_string
    grib_index_get_double
    grib_index_select_long
    grib_index_select_double
    grib_index_select_string
    GRIB_CHECK
    grib_get_double_element
    grib_get_double_elements
    Bunch
    );

my %ignore = ();
for (@exclude) { $ignore{$_} = 1; }

my @lines = <>;

my $dirty = 0;
foreach (@lines) {
    if ( /^(?:def|class) +(.*)\(/) {
        $dirty = exists $ignore{$1} ? 1 : 0;
    }

    print if !$dirty;
}
