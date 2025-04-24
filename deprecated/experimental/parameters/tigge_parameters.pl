#!/usr/bin/perl
use Data::Dumper;
use tigge;

my $info = tigge::read_info(@ARGV[0]);

my %x;

foreach my $n ( sort keys %{$info} )
{
	my $p = $info->{$n}->{'mars.param'};
	my $z = $info->{$n};

	if(exists $x{$p})
	{
		# Find differences
		my %z;
		my @z = keys %{$x{$p}};

		foreach my $k ( keys %{$z} )
		{
			$z{$k} = $z->{$k} if($x{$p}->{$k} eq $z->{$k});
		}

		$z = \%z;
	}

	$x{$p} = $z;

}

foreach my $k ( sort { $x{$a}->{tigge_name} cmp $x{$b}->{tigge_name} } keys %x )
{
	print "----------------------------------------------\n";
	print "$x{$k}->{'mars.abbreviation'}\t\t$x{$k}->{tigge_name}\n";

	foreach my $m ( sort keys %{$x{$k}} )
	{

		next if($m =~ /\./);
		next if($m =~ /\_/);
		my $v = $x{$k}->{$m};
		next if($v =~ /#/);
		next if($v =~ /missing/i);

		print "  $m = $v;\n";
	}

	print "----------------------------------------------\n";
}

