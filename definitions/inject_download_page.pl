#!/usr/bin/env perl

use strict;use warnings;
use Data::Dumper;

$#ARGV + 1 > 0 or die "Requires input file as argument\n";
my $filename = $ARGV[0];

open FIN,$filename;
my @content = <FIN>;
close FIN;

my $section = 0; # flag indicating we are in the download section of the parameters
my $inject = 0; # flag indicating we are at the right spot to start injecting the html code

for my $line (@content) {
    if ($line =~ /GRIB API Parameters/) {
        $section = 1;
        print $line;
    } elsif ($line =~ /Download/ && $section == 1) {
        $inject = 1;
        print $line;
    } elsif ($section == 1 && $inject == 1) {
        inject();
        $inject = 2; # done injecting
    } elsif ($inject == 2) {
        if ($line =~ /\/TABLE/) {
            $section = 0;
            $inject = 0;
            print $line;
        }
    } else {
        print $line;
    }
}

sub inject {

    my @colors = ('#FFFFCC','#E0E0E0');
    my $c = 0;

    while (<DATA>) {
        my ($version,$size,$date) = split;
        print << "END";
  <TR BGCOLOR="$colors[$c]">
    <TD height="21" >$version</TD>
    <TD class="info">$size KB </TD>
    <TD ><a href="/products/data/software/download/software_files/grib_api_parameters-v$version.tar.gz" class="software">grib_api_parameters-v$version.tar.gz</a> $date</TD>
  </TR>
END

    $c = $c == 0 ? 1 : 0;
    }
}

__DATA__
1 284 09.09.2010
