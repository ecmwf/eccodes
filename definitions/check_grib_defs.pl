#!/usr/bin/env perl

#
# Copyright 2005-2017 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

#########################################################################################
# Load in the definition files for GRIB "concepts" and check:
#  1. They have the same number of parameters
#  2. The params occur in the same order
#  3. Each parameter has same keys and values
#  4. Some basic rules are adhered to
#
# URLs:
#  http://perldoc.perl.org/perldsc.html#MORE-ELABORATE-RECORDS
#########################################################################################
$|=1;
#use strict;
use Test::More;
use Data::Dumper;
use Cwd;

my $GRIB1_MAX_TABLE2VERSION = 3;  # The maximum allowable value for WMO GRIB1 table2Version
$extra_info= 0;                   # Do we print more info?
$debug = 0;
$check_duplicate_paramIDs = 0;    # We tolerate this but maybe not for new data

$errmsg = "";
my $key;
my $pwd = getcwd;
$localConcept = 0;
# Determine if the parameters we are checking are LOCAL CONCEPTS or WMO ones
if ($pwd =~ /\/localConcepts\//) {
    print "It's local concepts\n";
    $localConcept = 1;
}

@files = qw(name.def paramId.def shortName.def units.def);
foreach my $f (@files) {
    die "Where is $f?\nI expected to find: @files\n" unless -f $f;
}

while (my $arg = shift @ARGV){
    if ($arg =~ /-D(\w+)=(\w+)/) {
        $var_name = $1; $value = $2;
        $$var_name = $value;
        #$$1 = $2;  same as above but more compact
    }
}

my %name_map = process("name.def");
my $count = scalar(keys %name_map);

ok($count > 0, "Check some params found");
die "No params found." if ($count eq 0);

my %paramId_map   = process("paramId.def");
print Data::Dumper->Dump([\%paramId_map], ["paramId_map"]), $/ if ($debug);

if ($extra_info) {
    # Define an array of all hashes: key -> hash
    my @all_maps = ();
    print "paramId.def: Num parameters = " . $count . " \n";
    print "paramId.def: Scanning for duplicate definitions...\n";
    my $num_duplicates = 0;
    for $key (keys %paramId_map) {
        @hashes = @{ $paramId_map{$key} };
        
        #if (@hashes > 1) {
            #print "\t$key: @{ $name_map{$key} }\n";
        #    print Data::Dumper->Dump([\$name_map{$key}], ["Map for $key"]);
        #    ++$num_duplicates;
        #}
        # Iterate through the hashes array. Each entry in @hashes is a hash
        foreach $ahash (@hashes) {
            # See if our little map exists in the pool of all maps seen so far
            #print Data::Dumper->Dump([\$ahash], ["Map for ahash"]);
            for $m1 (@all_maps) {
                #print "\t", Data::Dumper->Dump([\$m1], ["Map for m1"]);
                #my $same = is_deeply(\$m1, \$ahash);
                my $same = eq_hash(\$m1, \$ahash);
                if ($same) {
                    print "\nThe following mapping occurs somewhere else!!\n";
                    print "Key=$key,\t", Data::Dumper->Dump([\$ahash], [" "]);
                    #exit 2;
                }
            }
            push(@all_maps, $ahash);
        }
    }
    #print "DONE\n";
}

my %shortName_map = process("shortName.def");
my %units_map     = process("units.def");

# Check maps are the same
is_deeply(\%name_map, \%paramId_map,   'Check name and paramId are the same');
is_deeply(\%name_map, \%shortName_map, 'Check name and shortName are the same');
is_deeply(\%name_map, \%units_map,     'Check name and units are the same');

if (-f "cfVarName.def") {
   my %cfVar_map     = process("cfVarName.def");
   is_deeply(\%name_map, \%cfVar_map, 'Check name and cfVarName are the same');
} else {
   print "\n\tWARNING: Did not find a cfVarName.def file!!!\n\n";
}

done_testing();

check_paramIDs("paramId.def");


# -------------------------------------------------------------------------
# Function to return a hash:
#    key   = parameter long name
#    value = an array holding 1 or more hashes
#
# E.g.
# hash = {
#  'Reactive tracer 10 mass mixing ratio' => [
#       {
#           'parameterCategory' => '210',
#           'parameterNumber' => '149',
#           'discipline' => '192'
#       },
#       {
#           'parameterCategory' => '211',
#           'parameterNumber' => '149',
#           'discipline' => '192'
#       }
#   ],
#  'downward shortwave radiant flux density' => [
#       {
#           'parameterCategory' => '201',
#           'parameterNumber' => '1',
#           'discipline' => '192'
#       }
#   ],
# .... etc
#
# -------------------------------------------------------------------------
sub process {
    my ($filename) = @_;
    open FILE, $filename or die "Tried to open $filename\n$!";
    my @lines = <FILE>;
    close(FILE);
    print "Processing $filename\n";

    my $error = 0; # boolean: 1 if at least one error encountered
    my %map1 = ();
    my %map2 = ();  # inner map
    my $lineNum = 0;
    my $desc = "";
    my $concept = "";
    my $this;           # a line in file
    foreach $this (@lines) {
        $lineNum++;
        chomp $this;
        if ($lineNum == 1 && $this !~ /^#/ ) {
            die "File: $filename, first line should be a comment!";
        }
        # Description line
        if ($this =~ /^\s*#\s*(.*)\s*/) {
            $desc = $1;
            $desc =~ s/^\s+//;  #remove leading spaces
            $desc =~ s/\s+$//;  #remove trailing spaces
            die "File: $filename, line: $lineNum: Description contains invalid characters." if (non_printable($desc));
            die "File: $filename, line: $lineNum: Empty description." if ($desc eq "");
        }
        # key = value
        elsif ($this =~ /(\w+)\s*=\s*([^ ]+)\s*;/ && $desc) {
            $key = $1;
            $val = $2;
            if (!is_valid_keyval($key, $val, $localConcept)) {
                $error = 1;
                print "File: $filename, line: $lineNum: $errmsg (name=$desc)\n";
            }
            # Users will set parameters by shortname or ID
            if ($filename eq 'paramId.def' || $filename eq 'shortName.def') {
                # The 'typeOfSecondFixedSurface' key has side effects and can change the scale values/factors!
                # So make sure it comes BEFORE the scale keys! So if we come across this key ensure none of
                # scale keys came before it
                if ( $key =~ /typeOfSecondFixedSurface/ && 
                    (exists($map2{'scaleFactorOfFirstFixedSurface'}) ||
                     exists($map2{'scaledValueOfFirstFixedSurface'}) ||
                     exists($map2{'scaleFactorOfSecondFixedSurface'}) ||
                     exists($map2{'scaledValueOfSecondFixedSurface'})) )
                {
                    print "File: $filename, line: $lineNum: TypeOfSurface problem (GRIB-229): Please check: $desc\n";
                    #print "DUMP,\t", Data::Dumper->Dump([\%map2], [" "]);
                    $error = 1;
                }
            }
            $map2{$key} = $val;
        }
        elsif ($this =~ /'(.*)' *= *{/) {
            $concept = $1;
            die "File: $filename, line: $lineNum: Value contains invalid characters." if (non_printable($concept));
            if ($filename eq 'cfVarName.def') {
               #if ($concept =~ /^[0-9]/) {
               # Check CF naming convention. Do not allow numeric initial char or ~
               if ($concept !~ /^[A-z]/) {
                  $error = 1;
                  die "File: $filename, line: $lineNum: Invalid netcdf variable name: $concept";
               }
            }
        }
        # Hit the end brace
        elsif ($this =~ /^\s*}\s*$/) {
            my %map2_copy = %map2;  # copy inner hash
            # Store this inner hash in our array
            push @{ $map1{$desc} }, \%map2_copy;
            %map2 = ();             # Empty inner map for next param
        }
    }
    exit 1 if $error;
    return (%map1);
}
###################################
sub is_valid_keyval {
    my $key = shift;
    my $val = shift;
    my $local = shift;
    return 0 if (!is_valid_octet($key,$val));
    return 0 if (!is_valid_table2Version($key,$val,$local));
    return 0 if (!is_goodval($key,$val));

    return 1;
}

sub is_valid_octet {
    my $key = shift;
    my $val = shift;
    # Rule: Some keys are are only 1 octet so can only be 0->255
    if ($val > 255 || $val < 0) {
        if ($key eq 'discipline' || $key eq 'parameterCategory' || $key eq 'parameterNumber' ||
             $key eq 'indicatorOfParameter' || $key eq 'table2Version')
        {
            $errmsg = "Bad $key: \"$val\". Can only be 0->255";
            return 0;
        }
    }
    return 1;
}

sub is_valid_table2Version {
    my $key = shift;
    my $val = shift;
    my $is_local = shift;
    if (!$is_local && $key eq 'table2Version') {
        # GRIB edition 1 rule: in the WMO dir, table2Version <= 3
        if ($val > $GRIB1_MAX_TABLE2VERSION) {
            $errmsg = "Bad table2Version: \"$val\". Is this a local concept?";
            return 0;
        }
    }
    return 1;
}

sub is_goodval {
    my $key = shift;
    my $val = shift;
    
    if ($key eq 'discipline' || $key eq 'parameterCategory' || $key eq 'parameterNumber' ||
        $key eq 'indicatorOfParameter' || $key eq 'table2Version')
    {
        if (!is_integer($val)) {
            $errmsg = "Invalid value for $key: \"$val\". Expected a number!";
            return 0;
        }
    }
    return 1;
}

sub is_integer {
    my $val = shift;
    return ($val =~ /^\d+$/);
}

sub non_printable {
    my $str = shift;
    return ($str =~ /[^[:ascii:]]/);
}

################
sub check_paramIDs {
    my ($filename) = @_;
    open FILE, $filename or die "Tried to open $filename\n$!";
    my @lines = <FILE>;
    close(FILE);

    my $warnings = 0; # count of the number of warnings
    my %id_map = ();
    my $lineNum = 0;
    my $a_pid; # a parameter ID
    my $this;           # a line in file
    foreach $this (@lines) {
        $lineNum++;
        chomp $this;
        # a parameter ID
        if ($this =~ /^\s*'(.*)'\s*/) {
            $a_pid = $1;
            die "File: $filename, line: $lineNum: paramID \"$a_pid\" is not an integer!" if (!is_integer($a_pid));
            
            if ($check_duplicate_paramIDs) {
                if (exists $id_map{$a_pid}) {
                    print "WARNING: File: $filename, line: $lineNum: Duplicate paramID found: $a_pid\n";
                    $warnings++;
                } else {
                    $id_map{$a_pid} = 1;
                }
            }
        }
    }
    print "**\n* Duplicate paramIDs: Encountered $warnings warning(s)\n**\n" if ($warnings>0);
}
