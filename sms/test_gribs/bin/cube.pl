#!/usr/local/bin/perl56 -I/marsdev/data/max/webapps
use strict;


use LWP::UserAgent;
my $ua  = new LWP::UserAgent;

use webmars::parameter;
use webmars::mars::request2;
use Data::Dumper;

@ARGV=qw(marsod) unless(@ARGV);
$ARGV[1]=9876 unless($ARGV[1]);

open(DEF,">$ARGV[0].def");

my $m = ($ARGV[0] eq "marsms") ? 1 : 0;

print DEF <<"EOF";
suite mars_server
limit test_archive  3
limit test_retrieve 3
limit compare  3
limit grib_api  3
limit sample  3
limit qsub 50
export MARSMS $m

family $ARGV[0]
	edit DATABASE  $ARGV[0]
	limit $ARGV[0] 2
	inlimit qsub
EOF


my $host = "$ARGV[0]:$ARGV[1]";

my $tmp = tools::tmpfile->new;

my $cube = 0;
my $empty = 0;

my @unset = qw(
	year month branch
);

my %replace = (
	satellite => "ident",
	band      => "obstype",
);

my %seen;

my $url = "http://$host/cgi/lastcube";
my $req = HTTP::Request->new(GET => $url);
my $res = $ua->request($req);
my $last = $res->content;


my $suite={};
my $smslabel = $ENV{SMSLABEL};

# $last=10;
if($ARGV[2])
{
	$cube=$ARGV[2];
	$last=$ARGV[2];
}

while($cube <= $last)
{
	system("smslabel $smslabel '$cube out of $last'")
		if($smslabel && (($cube % 100) == 0));

	unless(-f $cube)
	{

		for(;;)
		{

			my $url = "http://$host/cgi/cube?id=$cube";
			my $req = HTTP::Request->new(GET => $url);
			my $res = $ua->request($req);

			my $text = $res->content;


			open(OUT,">$cube") or die "$cube: $!";
			print OUT $text;
			close(OUT) or die "$cube: $!";

			last unless($text =~ /exception/i);

		}

	}


	my $req = webmars::mars::request2->new_from_cube($cube);
	$cube++;

	my ($v) =  $req->get_values("expver");

	next unless($v == 1 or $v eq "");


	my @r = $req->split_for_batch;

	foreach my $s ( @r )
	{
		foreach my $r ( $s->individual_requests )
		{
			process($r->clone,1,2,2);
			process($r->clone,2,2,2);
		}
	}

}

system("smslabel $smslabel '$cube out of $last'")	if($smslabel);
put($suite,qw(CLASS STREAM TYPE LEVTYPE RUN) );


sub process {
		my ($r,$run,$max,$step) = @_;

		my ($year)  = $r->get_values("year");
		my ($month) = $r->get_values("month");

		map { $r->unset_value($_) } @unset;

		foreach my $k ( keys %replace )
		{
			my @x = $r->get_values($k);
			if(scalar(@x))
			{
				$r->unset_value($k);
				$r->set_values($replace{$k},@x);
			}
		}

		# print Dumper($r);

		
		my $j = $run - 1;
		my @s;
		for(my $i = 0; $i < $max ; $i++) { push @s,$j; $j += $step;}

		foreach my $x ( qw(date step levelist number direction frequency ) )
		{

			my @x = $r->get_values($x);
			next unless(@x);

			my @u;
			foreach my $s ( @s) 
			{
				push @u,$x[$s] if(defined $x[$s]);
			}

			unless(@u)
			{
				for(my $i = 0; $i < $max; $i++)
				{
					push @u,$x[$i]  if(defined $x[$i]);

				}
			}

			$r->restrict_values($x,\@u);
			my @v = $r->get_values($x);
			die Dumper($r,\@x,\@u,\@s,$x) unless(@v);

		}

		my @x =  $r->get_values("date");
		unless(@x)
		{
			#print "$cube year=$year month=$month @x\n";
			#print $r->as_batch,"\n";

			if($month <= 12)
			{
				my @strmonth = qw(jan feb mar apr may jun jul aug sep oct nov dec);
				$month = @strmonth[$month-1];
			}
			else
			{
				$month =~ s/-//g;
				$month .= "01" unless(length($month) == 8);
			}

			$r->set_values("date",$month);

#			print $r->as_batch,"\n";

		}


		my $year = int($year/10) . "0";



		my $c = $r->clone;
		map { $c->unset_value($_) } qw(date time);

		my $t = join("_",$c->target_name,$year,$run);
		$t =~ s/\W/_/g;


		next if($seen{$t});


		my $req = $r->as_batch;
		$req =~ s/\n//gs;
		$req =~ s/retrieve,//;

		my ($class)   = $r->get_values("class");
		my ($type)    = $r->get_values("type");
		my ($stream)  = $r->get_values("stream");
		my ($levtype) = $r->get_values("levtype");

		$levtype = "sfc" unless($levtype);
		$type    = "xx"  unless($type);
		$stream  = "xx"  unless($stream);



if(0) {
		foreach my $e qw(country model icmodel bcmodel dbase rki rty)
		{
			my ($s) =  $r->get_values($e);
			$stream .= "_$s" if(defined $s);
		}
}


		push @{$suite->{$class}->{$stream}->{$type}->{$levtype}->{$run}}, [$t,$req,$cube-1];

		# open(OUT,">reqs/$t.req") or die "reqs/$t.req: $!";
		# print OUT $r->as_batch,",target='$t.data'\n\n";
		# close(OUT)  or die "reqs/$t.req: $!";

		$seen{$t}++;
}


sub put {
	my ($suite,$name,@rest) = @_;

	unless($name)
	{
		foreach my $s ( @{$suite} )
		{
			my ($t,$req,$cube) = @{$s};
print DEF <<"EOF";

family $t
	edit REQ "$req"
	edit CUBE "$cube"
	task prod_retrieve ; inlimit $ARGV[0]
	task test_archive  ; inlimit test_archive;  trigger prod_retrieve == complete
	task test_retrieve ; inlimit test_retrieve; trigger test_archive == complete
	task compare ; inlimit compare;             trigger test_retrieve == complete
	task sample ; label info "" ; inlimit sample;             trigger prod_retrieve == complete
	#task sample ; label info "" ; inlimit sample;             trigger prod_retrieve == complete
	#task request ; label info "" ; inlimit request;             trigger prod_retrieve == complete
	#task gribex ; label info "" ; inlimit gribex;             trigger prod_retrieve == complete
	#task cleanup ; inlimit compare;             trigger compare == complete
endfamily
EOF
		}
		return;
	}

	foreach my $x ( sort keys %{$suite} )
	{
		print DEF "family $x; edit $name '$x'\n";
		put($suite->{$x},@rest);
		print DEF "endfamily\n";
	}
}

