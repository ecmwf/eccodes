#!/usr/bin/perl -I/marsdev/data/p4/linux/amd64/grib_api/main/perl/GRIB-API/blib/lib -I/marsdev/data/p4/linux/amd64/grib_api/main/perl/GRIB-API/blib/arch


use lib qw(/home/ma/mab/grib/api/lib);

use GRIB::API;
use strict;

my %DEFAULTS = (
		gribMasterTablesVersionNumber   => 5,
		subdivisionsOfBasicAngle => undef,
		);

my @RULES = (
		
		
		
# Dew point ?
	[
		# condition
		{ discipline=>0, parameterCategory=>0, parameterNumber=>7, typeOfFirstFixedSurface => 103 },
		# action
		{ tigge_short_name => "2d" }, ],
		

# Mean sea level
	[
		# condition
		{ discipline=>0, parameterCategory=>3, parameterNumber=>1,
			typeOfFirstFixedSurface => 101 },
		# action
		{ tigge_short_name => "msl",  }, ],

# Total precip.
	[
		# condition
		{ discipline=>0, parameterCategory=>1, parameterNumber=>8, },
		# action
		{ productDefinitionTemplateNumber => 11, tigge_short_name => "tp",  marsStartStep=>0}, ],


# Single levels
	[ 
		# condition
		{ typeOfSecondFixedSurface => 255 } , 
		# action
		{ scaleFactorOfSecondFixedSurface => undef, 
		scaledValueOfSecondFixedSurface => undef } ],
	[ 
		# condition
		{ typeOfSecondFixedSurface => 8 } , 
		# action
		{ scaleFactorOfSecondFixedSurface => undef, 
		scaledValueOfSecondFixedSurface => undef } ],
	[ 
		# condition
		{ typeOfFirstFixedSurface => 1 } , 
		# action
		{ scaleFactorOfFirstFixedSurface => undef, 
		scaledValueOfFirstFixedSurface => undef } ],

	[ 
		# condition
		{ typeOfFirstFixedSurface => 101 } , 
		# action
		{ scaleFactorOfFirstFixedSurface => undef, 
		scaledValueOfFirstFixedSurface => undef } ],

);

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
system("./tigge_check -b bad $ARGV[1] && mv $ARGV[0] done/");

# Checks if a handle matches a rule condition 
sub match {
	my ($h,$cond) = @_;
	foreach my $c ( keys %{$cond} )
	{
		return 0 unless($h->get_long($c) eq $cond->{$c});
	}
	return 1;
}

