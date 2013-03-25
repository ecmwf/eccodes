#!/usr/local/bin/perl56 -I/usr/local/lib/metaps/perl
use Data::Dumper;
use metdb qw(prod);

foreach my $x ( qw(class type stream) )
{
	my $table = "metdb::grib_$x";
	eval "use $table";

	my  @params = $table->find({},[qw(grib_code)]); 

	open(STDOUT,">$x.table");

	foreach my $p (  @params )
	{
		if($x ne "type" || $p->get_grib_code < 256)
		{
			print $p->get_grib_code, " ", $p->get_mars_abbreviation, " ", $p->get_long_name, "\n";
		}
	}

}
