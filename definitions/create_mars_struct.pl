#!/usr/local/bin/perl56 -I/usr/local/lib/metaps/perl
use strict;
use File::Path ;
use File::Copy;
use DBI;
use Data::Dumper;

my $db="param";
my $host="grib-param-db-prod.ecmwf.int";
my $user="ecmwf_ro";
my $pass="ecmwf_ro";
my $filename; my $filebase; my $out; my $conceptDir;
my $query; my $q; my $qh;

my $dbh  = DBI->connect("dbi:mysql(RaiseError=>1):database=$db;host=$host",$user,$pass) or die $DBI::errstr;

my $table;
my $number;
my $query="select id,shortName from param";
my %grib1=();
my $name; my $value; my $centre;

my $qh=$dbh->prepare($query);
$qh->execute();
$query="select grib.centre,attribute.name,grib.attribute_value,param_version 
		from grib,attribute where edition=1 and grib.param_id=? 
		 and ( grib.attribute_id=3 or grib.attribute_id=1) and attribute.id=grib.attribute_id  
		order by grib.centre,param_version,attribute_id";
my $qh1=$dbh->prepare($query);
my %values;
my %pvalues;
my $mars;

while (my ($paramId,$shortName)=$qh->fetchrow_array )
{
	die "$paramId" unless($shortName);
	if ($shortName =~ /\w+/ ) {
		push @{$values{$shortName}},$paramId;
		push @{$pvalues{$paramId}},$shortName;
	}

	$qh1->execute($paramId);
	%grib1=();
	my $centre_last="";
	my $param_version;

	while ( ($centre,$name,$value,$param_version)=$qh1->fetchrow_array ) {
		if (join(":",$centre,$param_version) ne $centre_last) {
			$centre_last=join(":",$centre,$param_version);
			if (exists $grib1{"indicatorOfParameter"} && exists $grib1{"table2Version"}) {
				my $mars=$grib1{"indicatorOfParameter"}.".".$grib1{"table2Version"};
				push @{$values{$mars}},$paramId;
				push @{$pvalues{$paramId}},$mars;
				%grib1=();
			}
		}
		$grib1{$name}=$value;	
	}

	if (exists $grib1{"indicatorOfParameter"} && exists $grib1{"table2Version"}) {
		my $mars=$grib1{"indicatorOfParameter"}.".".$grib1{"table2Version"};
		push @{$values{$mars}},$paramId;
		push @{$pvalues{$paramId}},$mars;
	}
}

open(my $mars_param_out,"> mars_param.table")
                or die "unable to open mars_param.table: $!";

foreach my $k ( sort keys %values ) {
	print $mars_param_out "$k ";
	print $mars_param_out join(" ", sort { $a <=> $b } @{$values{$k}});
	print $mars_param_out " | \n";
}

close $mars_param_out or die " mars_param.table: $!";
 
open(my $param_id_out,"> param_id.table")
                or die "unable to open param_id.table: $!";

foreach my $k ( sort keys %pvalues ) {
	print $param_id_out "$k ";
	print $param_id_out join(" ", sort { $a <=> $b } @{$pvalues{$k}});
	print $param_id_out " | \n";
}

close $param_id_out or die "param_id.table: $!";
