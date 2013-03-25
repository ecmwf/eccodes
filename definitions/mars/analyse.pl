#!/usr/bin/perl
use Data::Dumper;
opendir(DIR,".");
while($d = readdir(DIR))
{
	next unless($d =~ /^grib1.*\.def$/);
	open(IN,"<$d");
	my @x;
	while(<IN>)
	{
		chomp;
		s/#.*//;
		s/\s+/ /g;
		push @x,$_ if($_);
	}

	$q = join(";",sort @x);
	$q =~ s/;+/;/g;

	push @{$def{$q}},$d;
}

print Dumper(\%def);
