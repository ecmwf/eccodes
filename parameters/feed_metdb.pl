#!/usr/local/bin/perl56

# What to do:
#
# 1) change info in the file master.info 
#
# 2)
#    the following tabe must exist in psql metdb_test;
#
#    create table grib2_test  (
#            param_id                    integer,
#            levtype                     text,
#	         keyword                     text,
#		     value                       text,
#			 insert_date                 date default current_date,
#			 update_date                 date default current_date,
#
#            PRIMARY KEY ("param_id", "levtype", "keyword", "value")
#			 );			

use strict;

use DBI;

open( IN, "<master.info" );

# =======================================================================================
# here the script starts

my $DHB = DBI->connect("dbi:Pg(RaiseError=>1):dbname=metdb_test;host=datasvc","max", "");

my $prepare = "delete from grib2_test";
my $sth = $DHB->prepare( $prepare );
$sth->execute();
						
my $header = 0;
my $data   = 0;
my @headers;
while( <IN> ) {

    my $current = $_;
	chomp($current);

	if( m/^#/ ) {
		next
	}
	elsif( m/^\-+\Z/ ) {
		$data = 1;        # now the data section comes
		next
	}
	elsif( !m/^#/ and $data == 0  ) {
	    $header = 1;
		@headers = split( /\s/, $current );
	    next
	}	
	
	my @data = split( " ", $current );

    my $grib1;
	my $levtype;
	my $count = 0;
	foreach my $name ( @headers ) {
		
		$grib1   = $data[ $count ] if( $name =~ m/^mars.param\Z/ );
		$levtype = $data[ $count ] if( $name =~ m/^mars.levtype\Z/ );

        if( $name =~ m/^tigge_name\Z/ ) {
			$count++;
			next
		}

        if( $name =~ /^\w+\Z/ and $data[ $count ] =~ /\d+/ ) {
			my $keyword = $name;
			my $value   = $data[ $count ];
		
		    my $prepare = "insert into grib2_test values( '$grib1', '$levtype', '$keyword', '$value' )";
			my $sth = $DHB->prepare( $prepare );

			print $prepare . "\n";
			
			$sth->execute();
		}	

		$count++;
	}
	
}	

