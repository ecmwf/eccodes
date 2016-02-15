#!/usr/local/bin/perl56 
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

use lib qw(blib/lib blib/arch);
use GRIB::API;
use GRIB::API::Tie;
use GRIB::API::Debug;
use Data::Dumper;
use IO::Handle;

open(IN,"<data.grib");
while(my $x = GRIB::API->new(\*IN))
{
	my @x = $x->get_keys("mars");
	print "@x\n";
}

__END__

autoflush STDOUT 1;

unless( -f "data.grib")
{
	open(MARS,"|mars");
	print MARS "ret,tar=data.grib,level=500,grid=5/5,area=e\n";
	close(MARS);
}

#GRIB::API::debug();


{
	print Dumper($x->get_long("date"));
	print Dumper($x->get_long("foo"));
	print Dumper($x->get_double("class"));
	print Dumper($x->get_string("class"));
	print Dumper($x->get_string("expver"));
	print "set ", Dumper($x->set_long("date",20050101));


	print Dumper($x->get_long("date"));
	$x->set_string("class","rd") or die;
	$x->set_string("klass","rd") or warn;
	print Dumper($x->get_string("class"));

	my $class = "xxxx";
	print Dumper([$class=$x->get_bytes("expver"),$class]);

	my $class = "xxxx";
	print Dumper([$class=$x->get_bytes("expver"),$class]);

	$class = pack "L" , 0x41414141;
	print Dumper([$class]);
	$x->set_bytes("expver",$class) or warn;
	print Dumper($x->get_string("expver"));

	print Dumper($x->get_size("values"));
	my @x = $x->get_double_array("values");
	print scalar(@x),"\n";
	my $t = $x->get_double_array("values");
	print ref($t),"\n";

if(1) {
	foreach my $t ( @x ) { $t *= 2; }
	$x->set_double_array("values",\@x) or die;
	my @y = $x->get_double_array("values");

	my $i = 0;
	foreach my $t ( @x )
	{
		my $x = ($t - $y[$i])/($t?$t:1);
		$x = -$x if($x<0);
		die "$t $y[$i] $x" if($x > 1e6);
		$i++;
	}
}


	my $i = $x->iterator();
	print "iterator $i\n";
	if($i)
	{
		while( ($lat,$lon,$value) = $i->next())
		{
			print "($lat,$lon,$value)\n";
		}
	}


	@x = $x->get_keys();
	print Dumper(\@x);

	open(OUT,">foo");
	$x->Write(\*OUT);
	close(OUT);


	my %x;
	tie %x, "GRIB::API::Tie",$x;
	print "--- ",$x{class},"\n";
	$x{class} = "e4";
	print "--- ",$x{class},"\n";
	$x{class} = 1;
	print "--- ",$x{class},"\n";


	foreach my $z ( keys %x )
	{
		print "$z = [$x{$z}]\n" ;
		print "    size=[", $x->get_size($z),"] type=[", $x->get_type($z), "] class=[" , $x->get_accessor_class($z), "]\n";
		;#unless($z =~ /grib(\w+)?section_?\d/i);
	}

	$r = $x->get("values");
	print $r,"\n";
	$r = undef;


	print Dumper(\%x);

	my $y= GRIB::API::create("GRIB\0\0\0\1" . "\0" x 400000);
	my %y;
	tie %y, "GRIB::API::Tie",$y;

	#%y = %x;
#	print Dumper(\%y);
}
