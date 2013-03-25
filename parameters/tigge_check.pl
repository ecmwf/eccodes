#!/usr/bin/perl
use tigge;
use Data::Dumper;

my $info   = tigge::read_info(@ARGV[0]); # "master.info");
my $checks = tigge::read_info(@ARGV[1]); # "tigge_check.info");

#system("p4 edit ../tigge/tigge_check.h");
#open(STDOUT,">../tigge/tigge_check.h");

print "parameter parameters[] = {\n";




foreach my $n ( sort keys %{$info} )
{

	if($checks->{$n}->{checks} =~ /potential_vorticity_level/)
	{
		delete $info->{$n}->{scaleFactorOfFirstFixedSurface};
		delete $info->{$n}->{scaledValueOfFirstFixedSurface};
	}


	print "   {\n";
	print "      \"$n\",\n";
	print "      $checks->{$n}->{min1},\n";
	print "      $checks->{$n}->{min2},\n";
	print "      $checks->{$n}->{max1},\n";
	print "      $checks->{$n}->{max2},\n";
	print "      {\n";
	foreach my $m ( sort keys %{$info->{$n}} )
	{
		next if($m =~ /\./);
		next if($m =~ /\_/);
		my $v = $info->{$n}->{$m};
		next if($v =~ /#/);
		next if($v =~ /missing/i);

		print "         {\"$m\", $v},\n";
	}
	print "         {NULL, },\n";
	print "      },\n";
	print "      {";
	print join(", ",map {"\&$_"} split(",",$checks->{$n}->{checks}));
	print "},\n";
	print "   },\n\n";
}

print "};\n";
