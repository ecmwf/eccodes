#!/usr/local/bin/perl56 -I/usr/local/lib/metaps/perl

use strict; use warnings;

# Generate the mars tables from paramDB
#  stream.table
#  type.table
#  class.table

use Data::Dumper;
use DBI;

#use DBConfig;
#my %Config = DBConfig::get_config;
#my $db=$Config{DB};
#my $host=$Config{HOST};
#my $user=$Config{USER};
#my $pass=$Config{PASS};

my $db="param";
my $host="grib-param-db-prod.ecmwf.int";
my $user="ecmwf_ro";
my $pass="ecmwf_ro";

my $dbh = DBI->connect("dbi:mysql(RaiseError=>1):database=$db;host=$host","$user","$pass") or die $DBI::errstr;

my $mars_dir = "mars";

foreach my $att qw(class type stream) {
    my $sth = $dbh->prepare("select grib_code,mars_abbreviation,long_name from grib_$att order by grib_code");
    $sth->execute();

    my $mars_file = "${mars_dir}/${att}.table";
    open OUT,">${mars_file}" or die $!;
    print OUT "0 0 Unknown\n";

    while (my @row = $sth->fetchrow_array) {
        #print Data::Dumper->Dump(\@row);
        
        # NOTE:
        # The parameter DB type table has extra entries which cannot fit into
        # an octet (range of values of mars.type is 0->255) so we skip these
        if ($att eq "type") {
            my $type_code = $row[0];
            next if ($type_code > 255);
        }
        print OUT join " ",@row;
        print OUT "\n";
    }
    print "Wrote ${mars_file}\n";

    close(OUT);
}
