#!/usr/local/bin/perl56
use DBI;
use strict;
use Data::Dumper;

sub usage
{
    $0 =~ s#.*/##;
    my $s = " " x length($0);
print STDERR <<EOF;

Usage: $0 [options]

where options are:
       $s [-h                 ] | [--help                 ]
       $s [-a                 ] | [--alltables            ]
       $s [-l <list of tables>] | [--list <list of tables>]

EOF
}


 if( $#ARGV < 0 )
  {
    &usage;
    exit(1);
  }

my $db = DBI->connect("dbi:Pg(RaiseError=>1,Taint=>0,AutoCommit=>1):dbname=metdb_test;host=datasvc;user=max","","");

# Existing local Grib tables in database
#   my $table = "grib_versions";
#   my $what  = "grib_code";
#   my $where  = "mars_abbreviation='98'";

   my $table = "grib_parameters";
   my $what  = "grib_code_table";
   my $where  = "grib_originating_centre='98'";

    my $d=$db->prepare("select distinct $what from $table where $where order by $what");
       $d->execute();
       my @tables;
    while( my (@x) = $d->fetchrow_array )
     {
      push @tables, $x[0] ;
     }

#Deal with option

my @versions;

SWITCH:
{
if ($ARGV[0] =~ /^[-+].+/)
{
   if ($ARGV[0] eq "-a" || $ARGV[0] eq "--alltables")
    {
      @versions = @tables;
      last SWITCH;
    }
   if ($ARGV[0] eq "-l" || $ARGV[0] eq "--list")
    {
         shift @ARGV;
        if( $#ARGV < 0 )
         {
           print STDERR "Specify list of Grib tables\n";
           exit(1);
         }
        foreach my $x (@ARGV)
        {
          my $ok=1;
          push @versions, $x ;
           foreach my $y (@tables)
            {
               $ok=0 if($x==$y);
            }
           if($ok)
            {
               print STDERR "\n";
               print STDERR "Unrecognized Grib table: $x\n";
               exit(1);
            } 
         }
       last SWITCH;
    }
   if ($ARGV[0] eq "-h" || $ARGV[0] eq "--help")
    {
            &usage;
           $db->disconnect();
            exit;
    }
       print STDERR "Unrecognized option: $ARGV[0]\n";
            &usage;
           $db->disconnect();
            exit(1);
}
else {
       print STDERR "Unrecognized option: $ARGV[0]\n";
            &usage;
           $db->disconnect();
            exit;
}
}


    $table = "grib_parameters";
    $what  = "grib_parameter, mars_abbreviation, long_name, unit, comment";

foreach my $version (@versions)
{
   my $where = "where grib_originating_centre='98' and grib_code_table='$version'";
   my $s=$db->prepare("select $what from $table $where order by $what");

   $s->execute;

   my $out="local_table_2_version_$version";
   open(G,">$out") or die "can not write to $out";

   while(my (@x) = $s->fetchrow_array)
   {

      my $param  = '0' x 3;
      my $offset = length($param) - length($x[0]);
      substr($param, $offset, length($x[0])) = $x[0]; 

        $offset = 1;
      $x[1] = 'None' unless( $x[1] );
      my $abbr = ' ' x 6;
      substr($abbr, $offset, length($x[1])) = $x[1]; 

      my $long = ' ' x 70;
      substr($long, $offset, length($x[2])) = $x[2]; 

      $x[3] = '-' unless( $x[3] );
      my $unit = ' ' x 30;
      substr($unit, $offset, length($x[3])) = $x[3]; 

      printf G "%s   %s   %s   %s \n",$param,$abbr,$long,$unit; 

   }
}

close(G);

$db->disconnect();
