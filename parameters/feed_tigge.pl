#!/usr/local/bin/perl56

# feeds all static infos into psql tigge_test   
# such as: - title, 
#          - name, 
#          - abbreviation
#          - unit           
# 
# the information comes from master.info except for unit   
# 
# an evironmental variable ECCODES_DEFINITION_PATH must exist 
# master.info is in ECCODES_DEFINITION_PATH/../parameters
# the units are taken from ECCODES_DEFINITION_PATH/definitions/*
# 
# a table parameters with the above columns must exist
#

use DBI;

use strict;

my $what = $ARGV[0];
die "'test' or 'prod'? type either 'feed_tigge test' or 'feed_tigge prod'" unless( $what eq "test" or
                                                                                   $what eq "prod" );

my $DHB = DBI->connect("dbi:Pg(RaiseError=>1):dbname=tigge_$what;host=datasvc","max", "");

my $prepare = "select levtype, grib1_numbers from parameters";
my $sth = $DHB->prepare( $prepare );
$sth->execute();

my $path = "/home/max/grib_api/parameters";
open( IN, "<$path/master.info" ) or die "can not open file master.info";
my @master_info = <IN>;
close( IN );

while( my ( $levtype, $grib1_numbers ) = $sth->fetchrow_array() ) { 
	my @parameter = grep(/^$grib1_numbers\s+/, @master_info);
    @parameter    = grep(/\s+$levtype\s+/,@parameter);

	die "more than one match for levtype = $levtype and grib1_numbers = $grib1_numbers" if( scalar( @parameter ) > 1 ); 
	die "no match found for levtype = $levtype and grib1_numbers = $grib1_numbers" unless( @parameter );

	my @entries = split(" ", $parameter[0] );
	my $abbreviation = $entries[1];
	my $name         = $entries[2];

	my $title        = get_title( $name );
	my $unit         = get_unit( $entries[4], $entries[5], $entries[6], $entries[13] );

    # check whether parameter entry exists 
	my $prepare = "select levtype, grib1_numbers from parameters where levtype=? and grib1_numbers=?";
	my $sth = $DHB->prepare( $prepare );
	$sth->execute( $levtype, $grib1_numbers);

    if( $sth->fetchrow_array() ) {
	
        print "updating $title ( $name ): $unit and $abbreviation\n"; 
	
		my @strings = ( "title='$title'", "name='$name'", "unit='$unit'", "abbreviation='$abbreviation'" );
		foreach my $string ( @strings ) {
			my $prepare = "update parameters set $string where levtype=? and grib1_numbers=?";
			my $sth = $DHB->prepare( $prepare );
			$sth->execute( $levtype, $grib1_numbers);
		}
	}    
}


sub get_title {
	my ( $name ) = @_;

	$name =~ s/u_vel/u-vel/;
	$name =~ s/v_vel/v-vel/;
	$name =~ s/time_int/time-int/;
	$name =~ s/land_sea/land-sea/;
    $name =~ s/_/ /g;

    return ucfirst( $name )
}


sub get_unit {
    my ( $disc, $cat, $num, $stat_proc ) = @_;

    my $path    = "/home/max/grib_api/definitions/grib2";
	open( IN2, "<$path/4.2.$disc.$cat.table" ) or die "Could not open file $path/4.2.$disc.$cat.table";
    my @content = <IN2>;
    close IN2;

    my ($what)  = grep(/^\s*$num\s+\w+/, @content);

    my ( $unit ) = ( $what =~ /^\s*$num\s+.+\s+\(([\w|\s|\-|%]*)\)\Z/ );
    $unit        = "unknown" unless( $unit );
    $unit        = "" if( $unit eq "Numeric" ); 
	chomp( $unit );

	if( $stat_proc == 1 ) {
		if( $unit !~ /\ss/ ) {
			$unit = $unit . " s"; 
		}
		elsif( $unit =~ /\ss-1/ ) {
			$unit =~ s/s-1//
		}
		elsif( $unit =~ /\ss\s/ ) {
		    $unit =~ s/\ss\s/s2/
		}
		else {
			$unit =~ s/\ss(-?\d+)\s/\ss$1+1\s/
		}
	}

    return $unit 
}

