# ------------------------------------------------------------------------
# Convert a CSV file into a TSV file
# i.e. replace all delimiters from comma to TAB
# All commas within strings are preserved
# ------------------------------------------------------------------------

my $f=@ARGV[0];
open(IN,"<$f") or die "$f: $!";
open(OUT,">$f.tsv") or die "$f.tsv: $!";

my $line;
while (defined ($line = <IN>)) {
	chomp($line);
	my @list = ($line =~ /(".*?"|[^,]*)(,)|(".*?"|[^,]*)/g) or die;
	my $i=0;
	while ($i <= $#list) {
		if ($list[$i] eq ",") {$list[$i]="\t";}
		$i++;
	}
	my $newline=join('',@list);
	# Remove all double-quotes
	$newline =~ s/\"//g;
	print OUT "$newline\n";
}

close (IN);
close (OUT);
print "Created $f.tsv\n";
