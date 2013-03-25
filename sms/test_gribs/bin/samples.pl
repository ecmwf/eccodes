#!/usr/local/bin/perl56 -I/marsdev/data/max/webapps
use strict;


use LWP::UserAgent;
my $ua  = new LWP::UserAgent;

use webmars::parameter;
use webmars::mars::request2;
use Data::Dumper;

my @tests;
opendir(DIR,"/marsdev/data/max/grib_api/tests.ecmwf");
foreach my $t ( sort map { s/\.test$//; $_;  } grep { /\.test$/} readdir(DIR) )
{
	my $exit = 0;
	open(IN,"</marsdev/data/max/grib_api/tests.ecmwf/$t.test");
	while(<IN>)
	{
		chomp;
		$exit++ if(/^exit$/);
	}
	close(IN);
	push @tests,$t unless($exit);
}
closedir(DIR);

print "@tests\n";

open(DEF,">samples.def");

print DEF <<"EOF";
suite mars_server
limit grib_api  3
limit qsub 50

family samples
	inlimit qsub
EOF

foreach my $t ( @tests)
{
	my $x = $t eq "memory" ? 1 : 5;
	print DEF "limit $t $x\n";
}

opendir(S,"/vol/dataserv/grib_samples") or die "opendir /vol/dataserv/grib_samples: $!";

foreach my $d ( sort readdir(S) )
{
	next if($d =~ /problem/ );
#	next unless($d =~ /brute/ );
	next if($d =~ /^\./);

	opendir(DIR,"/vol/dataserv/grib_samples/$d");

	print DEF "family $d\n";

	foreach my $x ( sort  readdir(DIR))
	{
		next unless($x =~ /\.\w\w\w\w/);

		my $y = $x;
		$y =~ s/\W/_/g;
		print DEF <<"EOF";
family $y #; autocancel +00:01
		edit SAMPLEDIR /vol/dataserv/grib_samples/$d
	edit FILE "$x"
	task copy
EOF


foreach my $t ( @tests)
{
	print DEF <<"EOF";
	task $t
		trigger copy == complete
		inlimit /mars_server/samples:$t
		edit SMSSCRIPT $ENV{SMSFILES}/test.sms
		label info ""
EOF
}

	print DEF <<"EOF";
endfamily
EOF
	}

	print DEF "endfamily\n";

}


