#!/usr/bin/perl
use tigge;
use Data::Dumper;

my $h = tigge::read_info(@ARGV);

show($h,qw(discipline parameterCategory parameterNumber));
show($h,qw(typeOfStatisticalProcessing));
show($h,qw(typeOfFirstFixedSurface typeOfSecondFixedSurface));


sub show {
	my ($h,@keys) = @_;

	my %p;
	print '=' x 80, "\n";
	print "@keys\n";
	print '=' x 80, "\n";

	foreach my $k ( sort keys %{$h} )
	{
		my %y = %{$h->{$k}};
		my @v = @y{@keys};
		my $p = join("-",@v);

		push @{$p{$p}}, $h->{$k};
	}

	foreach my $k ( sort keys %p )
	{
		print "$k\n";
		foreach my $p ( @{$p{$k}} )
		{
			print "   ",$p->{tigge_name}," ($p->{'mars.levtype'})\n";
		}
	}

	print '-' x 80, "\n";
	print "\n";

}
