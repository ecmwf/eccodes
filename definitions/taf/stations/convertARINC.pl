use strict;

my $f="ARINC.TXT";
my $line;
my @l;

my $c=3.2808399;

open(IN,"<$f") or die "$f: $!";

while (defined($line = <IN>)) {
		chomp($line);
		$line=~ s/\"//g;
		@l=split(/,/,$line);
		print $l[0]."|".$l[3]."|".$l[4]."|".int($l[6]*$c)."\n";
}

close(IN);
