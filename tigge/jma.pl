#!/usr/bin/perl -I../perl/GRIB-API/blib/lib -I../perl/GRIB-API/blib/arch
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

#!/usr/bin/perl -I/usr/local/lib/metaps/lib/grib_api/current/perl

use lib qw(/path/to/install/grib/api/lib);

use GRIB::API;
use strict;

my %DEFAULTS = (
	#	productionStatusOfProcessedData => 5,
		);

my @RULES = (
		
		
		
# Dew point
	[
		# condition
		{ discipline=>0, parameterCategory=>0, parameterNumber=>6, 
			typeOfFirstFixedSurface => 1,
		},
		# action
		{ tigge_short_name => "2d", typeOfFirstFixedSurface=>103,
			scaleFactorOfFirstFixedSurface=>0,
			scaledValueOfFirstFixedSurface=>2 }, 
	],

# Total precip.
	[
		# condition
		{ discipline=>0, parameterCategory=>1, parameterNumber=>8, 
		},
		# action
		{ tigge_short_name => "tp",}
	],
		
# Mean sea level pressure

	[
		# condition
		{ discipline=>0, parameterCategory=>3, parameterNumber=>1, 
		},
		# action
		{ tigge_short_name => "msl",}
	],
		
# Total cloud cover 

	[
		# condition
		{ discipline=>0, parameterCategory=>6, parameterNumber=>1, 
		},
		# action
		{ tigge_short_name => "tcc",}
	],
		
		
		

);

@RULES = ();

@ARGV = qw(ncep s) unless(@ARGV);
$ARGV[1] = "s" unless($ARGV[1]);

open(IN,"<$ARGV[0]") or die "$ARGV[0]: $!";
open(OUT,">$ARGV[1]") or die "$ARGV[1]: $!";

while(my $g = GRIB::API->new(\*IN))
{
# Initialise using default

	$g->set_values(\%DEFAULTS);

# Apply rules

	foreach my $r ( @RULES )
	{
		$g->set_values($r->[1]) if(match($g,$r->[0]));
	}


	$g->Write(\*OUT) or die "$ARGV[1]: $!";
}

close(OUT) or die "$ARGV[1]: $!";
#system("./tigge_check $ARGV[1] && mv $ARGV[0] done/");

# Checks if a handle matches a rule condition 
sub match {
	my ($h,$cond) = @_;
	foreach my $c ( keys %{$cond} )
	{
		return 0 unless($h->get_long($c) eq $cond->{$c});
	}
	return 1;
}

