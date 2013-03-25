package tigge;
use strict;

sub read_info {

	my (@files) = @_;
	my %params;

	@files = qw(master.info) unless(@files);

	foreach my $f ( @files )
	{
		open(IN,"<$f") or die "$f: $!";

		my @header;
		my @values;

		while(<IN>)
		{
			chomp;
			next if(/^#/);
			next if(/^-/);

			unless(@header)
			{
				@header = split;
				next;
			}

			@values = split;
			next unless(@values);

			die unless(@values == @header);

			my %h;
			@h{@header} = @values;

			my $name    = $h{tigge_name};
			my $levtype = $h{"mars.levtype"};
			die "No tigge name @header" unless($name);
			die "No levtype" unless($levtype);
			$name = "${name}_${levtype}";
			die "Duplicate entry $name" if(exists $params{$name});
			$params{$name} = \%h;
		}

		close(IN);

	}


	return \%params;
}

1;
