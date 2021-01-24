#!/usr/local/bin/perl
   eval 'exec perl -S $0 "$@"'
      if $runnning_under_some_shell;

########################################################################
#
# Simple program to emulate numdiff
# It compares two files which must have the same number of lines
# ignoring small numeric differences or/and different numeric formats
#
# The user can specify an absolute tolerance for the comparisons
#
# It exits with status 0 if the files are considered the same
# otherwise status is 1.
# 
# Only the first difference is printed
#
########################################################################

use strict;
use File::Compare;
use Getopt::Long;

# Process arguments. Must be at least two files
if (scalar @ARGV < 2) {
  &usage;
}
# Tolerance is optional
my $tolerance = 1e-9;
my $res = GetOptions (
  "t=s" => \$tolerance
  );

my $fileA = $ARGV[0];
my $fileB = $ARGV[1];
die "$!" unless (-e $fileA && -e $fileB);

#print "DEBUG:  f1=$fileA, f2=$fileB, tol=$tolerance\n";

my $line_num=1;
use File::Compare 'cmp';

sub munge($) {
    my $line = $_[0];
    for ($line) {
        s/^\s+//;   # Trim leading whitespace.
        s/\s+$//;   # Trim trailing whitespace.
    }
    return ($line);
}

my $delta = $tolerance;

if (not cmp($fileA, $fileB, sub {
        my $arg1 = munge $_[0];
        my $arg2 = munge $_[1];
        # $arg1 is a line from $fileA, $arg2 is a line from $fileB
        my $result = 0;
        my @data1 = split(/\s/, $arg1);
        my @data2 = split(/\s/, $arg2);
        my $data1_length = @data1;
        my $data2_length = @data2;
        if ($data1_length != $data2_length) {
            print "Line $line_num lengths differs: [$arg1] <=> [$arg2]\n";
            return 1;
        }
        foreach my $idx (0 .. $#data1) {
            my $d1 = $data1[$idx];
            my $d2 = $data2[$idx];
            #print "Line $line_num: Comparing |$d1| with |$d2| ...\n";
            $result = (abs($d1 - $d2) > $delta);
            if ($result) {
                print "Line $line_num differs: [$d1] <=> [$d2]\n";
                print "Absolute error = ". abs($d1 - $d2) . "\n";
                return $result; # Failed
            }
        }
        ++$line_num;
        return $result;
        # abs(munge $_[0] - munge $_[1])>$delta
    } ))
{
    #print "FLOAT: fileA and fileB are considered the same. HOORA\n";
    exit 0;
}

# Comparison failed. Check if the files have different number of lines
my $linesA = 0;
my $linesB = 0;
open (FILE, $fileA) or die "Can't open $fileA: $!";
$linesA++ while (<FILE>);
close FILE;

open (FILE, $fileB) or die "Can't open $fileB: $!";
$linesB++ while (<FILE>);
close FILE;

if ($linesA != $linesB) {
   print STDERR "Files do not have the same number of lines\n";
}

exit 1;  # Files considered different


###################################################
sub usage {
   print <<USAGE;

Usage: $0 [-t tolerance] file1 file2

USAGE
   exit 1
}
