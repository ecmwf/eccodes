package tigge::tools::convert;
use Exporter;
use base qw(Exporter);
@EXPORT=qw(TiggeNameToGrib1 TiggeAbbreviationToGrib1 Grib1ToGrib2 Grib1ToNewId);


use DBI;

use strict;

sub TiggeNameToGrib1 {
	my ( $self ) = @_;

	my $DHB1 = DBI->connect("dbi:Pg(RaiseError=>1):dbname=tigge_prod;host=datasvc","max", "");

	my $prepare = "select grib1_numbers, name, levtype from parameters";
	my $sth = $DHB1->prepare( $prepare );
	$sth->execute();

    my %result;

    while( my ( $grib1, $name, $levtype ) = $sth->fetchrow_array() ) {
		$result{ $grib1 . "_" . $levtype } = lc $name;
	}

    return \%result
}


sub TiggeAbbreviationToGrib1 {
    my ( $self ) = @_;

    my $DHB1 = DBI->connect("dbi:Pg(RaiseError=>1):dbname=tigge_prod;host=datasvc","max", "");

    my $prepare = "select grib1_numbers, abbreviation, levtype from parameters";
    my $sth = $DHB1->prepare( $prepare );
    $sth->execute();

    my %result;

    while( my ( $grib1, $abbreviation, $levtype ) = $sth->fetchrow_array() ) {
		$result{ $grib1 . "_" . $levtype } = lc $abbreviation;
    }

    return \%result
}


sub Grib1ToGrib2 {
    my ( $self ) = @_;

	my $DHB1 = DBI->connect("dbi:Pg(RaiseError=>1):dbname=tigge_prod;host=datasvc","max", "");

	my $prepare = "select grib1_numbers from parameters";
	my $sth1 = $DHB1->prepare( $prepare );
	$sth1->execute();

    my %result;

	while( my ( $grib1 ) = $sth1->fetchrow_array() ){

    	my $DHB2 = DBI->connect("dbi:Pg(RaiseError=>1):dbname=metdb_test;host=datasvc","max", "");

    	my $prepare = "select distinct levtype from grib2_test where param_id=?";
    	my $sth2 = $DHB2->prepare( $prepare );
    	$sth2->execute( $grib1 );

    	while( my ( $levtype ) = $sth2->fetchrow_array() ){

        	my $prepare = "select keyword, value from grib2_test where param_id=? and levtype=?";
        	my $sth2 = $DHB2->prepare( $prepare );
        	$sth2->execute( $grib1, $levtype );

            my %key_value;

        	while( my ( $key, $value ) = $sth2->fetchrow_array() ){
	
				$key_value{ $key } = $value  if( $value ne "255" and $value ne "pressure level [Pa]" )		            
            }	
    	
        	$result{ $grib1 . "_" . $levtype } = \%key_value
        }
	}

    return \%result
}


sub Grib1ToNewId {
	my ( $self ) = @_;

    my $DHB1 = DBI->connect("dbi:Pg(RaiseError=>1):dbname=tigge_prod;host=datasvc","max", "");

    my $prepare = "select grib1_numbers from parameters";
    my $sth1 = $DHB1->prepare( $prepare );
    $sth1->execute();

    my %result;

    while( my ( $NewId ) = $sth1->fetchrow_array() ){

		my $grib1 = $NewId; 
		$grib1 =~ s/^(\d\d\d)(\d\d\d)\Z/\2.\1/; 
		$grib1 =~ s/^(\d{1,3})\Z/\1.128/;

		$grib1 =~ s/^0*//;
		$result{ $grib1 } = $NewId + 0;
	}

	return \%result
}

1;
