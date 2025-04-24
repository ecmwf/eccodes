#!/usr/local/bin/perl56

use strict;


@ARGV = qw(master.info) unless(@ARGV);


my $path = $ENV{ ECCODES_DEFINITION_PATH };
$path    = "../definitions" unless( $path );
$path    = $path . "/grib2";

my $start = 0;
while( <> ) {

	if( m/^\-+\Z/ ) {
		$start = 1;
		next
	}
	elsif( $start == 0 ) {
		next
	}	

	my ( $grib, $abbr, $title, $lev, $disc, $cat, $num,  
	     $sfc1a, $sfc1b, $sfc1c, $sfc2a, $sfc2b, $sfc2c, 
         $stat) = split(/\s+/, $_);

	my @where = ( "4.2.$disc.$cat.table", 
                  "4.5.table", "4.5.table", 
                  "4.10.table" ); 
    my @what  = ( $num, $sfc1a, $sfc2a, $stat );

	my $level1 = get_level_value( $sfc1a, $sfc1b, $sfc1c );
	my $level2 = get_level_value( $sfc2a, $sfc2b, $sfc2c );

	my @result;
    my $count = 0;
	foreach my $file ( @where ) {

       	my $number  = $what[ $count++ ]; 
        next unless( $number =~ m/\d+/ );

        my $file = "$path/$file";
        open( IN2, "<$file") or die "Could not open file $file";
		my @content = <IN2>;
		close IN2;
		
		my ($what)  = grep(/^\s*$number\s+\w+/, @content); 

    	my ( $what ) = ( $what =~ /^\s*$number\s+\w+\s+([\w|\s|-]+)/ );  
    	$what = "unknown" unless( $what ); 
        chomp( $what );
	
		my $test = $what;
		my $foo  = chop( $test );
		chop( $what ) if( $foo =~ m/\s/ ); 
	
	    $what = $what . "($level1)" if( $count == 2 and $level1 =~ m/\d+/ );
		$what = $what . "($level2)" if( $count == 3 and $level2 =~ m/\d+/ );
		push @result, $what;
    }
	
	print "$grib\t$title ($abbr):\n";
	print "\t\t" . join(", ", @result) . "\n\n";

}


sub get_level_value {
	my ( $code, $value1, $value2 ) = @_;

	return if( $value1 == 255 and $value2 == 255 );

	return if( $value1 eq "#" );

	my $file = "$path/4.5.table";
    open( IN3, "<$file") or die "Could not open file $file";
    my @content = <IN3>;
    close IN3;

    my ($what)  = grep(/^\s*$code\s+\w+/, @content);

    my ( $unit ) = ( $what =~ /^\s*$code\s+[\w|\s]+\(([\w|\s|-]+)\)/ );
    chomp( $unit );

    my $value = 10**(-$value2) * $value1;
	return "$value $unit" 
}

