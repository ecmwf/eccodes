#!/usr/bin/perl -I/path/to/install/p4/grib_api/main/perl/GRIB-API/blib/lib -I/path/to/install/p4/grib_api/main/perl/GRIB-API/blib/arch
use lib qw(/bison/user/schuster/grib_api-0.9.5/perl/GRIB-API/lib);
use lib qw(/bison/user/schuster/grib_api-0.9.5/perl/GRIB-API/blib/lib);
use lib qw(/bison/user/schuster/grib_api-0.9.5/perl/GRIB-API/blib/arch);
use GRIB::API;
use strict;

my %DEFAULTS = (
#		reserved                        => undef,
		gribMasterTablesVersionNumber   => 5,
		versionNumberOfGribLocalTables  => 0,
		productionStatusOfProcessedData => 4,
		subdivisionsOfBasicAngle        => undef,


# Special case
		numberOfForecastsInEnsemble     => 21,

		);

my @RULES = (
		
		
		
# Minimum temperature 
	[
		# condition
		{ discipline=>0, parameterCategory=>0, parameterNumber=>5,
			typeOfFirstFixedSurface => 103 },
		# action
		{ tigge_short_name => "mn2t6" }, ],
		
# Maximum temperature 
	[
		# condition
		{ discipline=>0, parameterCategory=>0, parameterNumber=>4,
			typeOfFirstFixedSurface => 103 },
		# action
		{ tigge_short_name => "mx2t6" }, ],

# Cloud cover ?
	[
		# condition
		{ discipline=>0, parameterCategory=>6, parameterNumber=>1,
			typeOfFirstFixedSurface => 200 },
		# action
		{ tigge_short_name => "tcc",
		productDefinitionTemplateNumber=>1  }, ],

# Mean sea level
	[
		# condition
		{ discipline=>0, parameterCategory=>3, parameterNumber=>1,
			typeOfFirstFixedSurface => 101 },
		# action
		{ tigge_short_name => "msl",  }, ],

# CAPE
	[
		# condition
		{ discipline=>0, parameterCategory=>7, parameterNumber=>6,
			typeOfFirstFixedSurface => 108 },
		# action
		{ tigge_short_name => "cape",  
		}, ],

# Convective inhibition
	[
		# condition
		{ discipline=>0, parameterCategory=>7, parameterNumber=>7, 
			typeOfFirstFixedSurface => 1 },
		# action
		{ tigge_short_name => "ci",  
		}, ],

# Skin temperature
	[
		# condition
		{ discipline=>0, parameterCategory=>0, parameterNumber=>0, 
			typeOfFirstFixedSurface => 1 },
		# action
		{ tigge_short_name => "skt",  
		}, ],

# Snow depth
	[
		# condition
		{ discipline=>0, parameterCategory=>1, parameterNumber=>13, 
			typeOfFirstFixedSurface => 1 },
		# action
		{ tigge_short_name => "sd",  
		}, ],

# Total precipitation
	[
		# condition
		{ discipline=>0, parameterCategory=>1, parameterNumber=>8,
			typeOfFirstFixedSurface => 1 },
		# action
		{ tigge_short_name => "tp" }, ],


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


die "usage: $0 in out" unless(@ARGV == 2);

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

