#!/usr/bin/env perl
#
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#
#######################################################################
# Script for GRIB2 parameter definitions
# Can either write the *.def files or push to the Parameter DB
#
# Reads an input TSV (tab-separated-value) file which should contain
# parameter keys as columns. See the @columns variable for expected contents
#
#######################################################################
$|=1;
use strict;
use warnings;
use DBI;
use Time::localtime;
use Getopt::Long;

my $SANITY_CHECK     = 0;
my $WRITE_TO_FILES   = 0;
my $WRITE_TO_PARAMDB = 0; # Be careful. Fill in $contactId before proceeding

# Process arguments. Must be at least one file
if (scalar @ARGV < 1) {
  &usage;
}
my $result = GetOptions (
  "s" => \$SANITY_CHECK,
  "f" => \$WRITE_TO_FILES,
  "p" => \$WRITE_TO_PARAMDB
  );

$ARGV[0] or &usage;


my ($paramId, $shortName, $name, $units, $cfVarName, $interpol);
my ($discipline, $pcategory, $pnumber, $type1, $type2, $scaledValue1, $scaleFactor1, $scaledValue2, $scaleFactor2);
my ($stat, $aero, $constit);
my ($typeGen, $localTV, $typeOfWLInt, $scaleFactorWL1, $scaledValueWL1, $scaleFactorWL2, $scaledValueWL2, $sourceSink);

my @columns = ("paramId", "shortName", "name", "units", "interpolation",
    "discipline", "parameterCategory", "parameterNumber",
    "typeOfFirstFixedSurface", "scaleFactorOfFirstFixedSurface", "scaledValueOfFirstFixedSurface",
    "typeOfSecondFixedSurface", "scaleFactorOfSecondFixedSurface", "scaledValueOfSecondFixedSurface",
    "typeOfStatisticalProcessing", "aerosolType", "constituentType", "typeOfGeneratingProcess", "localTablesVersion",
    "typeOfWavelengthInterval", "scaleFactorOfFirstWavelength", "scaledValueOfFirstWavelength",
    "scaleFactorOfSecondWavelength", "scaledValueOfSecondWavelength", "sourceSinkChemicalPhysicalProcess");

my %key_to_attrib_map = (
    'discipline'         => 4,
    'parameterCategory'  => 8,
    'parameterNumber'    => 5,
    'localTablesVersion' => 16,
    'typeOfFirstFixedSurface' => 6,
    'scaleFactorOfFirstFixedSurface' => 7,
    'scaledValueOfFirstFixedSurface' => 9,
    'typeOfStatisticalProcessing' => 11,
    'typeOfSecondFixedSurface' => 13,
    'scaledValueOfSecondFixedSurface' => 14,
    'scaleFactorOfSecondFixedSurface' => 15,
    'typeOfGeneratingProcess' => 28,
    'constituentType' => 40,
    'aerosolType' => 46
);
my $db   = "param";
my $host = $ENV{'PARAM_DB_HOST'} || 'unknown';
my $user = $ENV{'PARAM_DB_USER'} || 'unknown';
my $pass = $ENV{'PARAM_DB_PASS'} || 'unknown';
my $dbh  = 0;
my $centre_wmo   = -3; # WMO centre ID
my $centre_ecmwf = 98; # ECMWF centre ID
my $edition = 2; # GRIB edition 2
my $contactId;   # JIRA issue ID

my $PARAMID_FILENAME   = "paramId.def";
my $SHORTNAME_FILENAME = "shortName.def";
my $NAME_FILENAME      = "name.def";
my $UNITS_FILENAME     = "units.def";
my $CFVARNAME_FILENAME = "cfVarName.def";

my $tm = localtime;
my $today_date = sprintf("%04d-%02d-%02d", $tm->year+1900, ($tm->mon)+1, $tm->mday);

if ($WRITE_TO_PARAMDB || $SANITY_CHECK) {
    print "Connecting to database ...\n";
    $dbh = DBI->connect("dbi:mysql(RaiseError=>1):database=$db;host=$host",$user,$pass) or die $DBI::errstr;
    # STRICT_TRANS_TABLES:
    # Strict mode controls how MySQL handles invalid or missing values in data-change statements such as INSERT or UPDATE.
    # A value can be invalid for several reasons e.g., it might have the wrong data type for the column, or it might be out of range.
    $dbh->do( q{SET sql_mode = 'STRICT_TRANS_TABLES'} );
}

my $first = 1;
my $lcount = 0;

if ($SANITY_CHECK) {
    my %map_sn = ();   # map of shortNames
    my %map_pid = ();  # map of paramIds
    print "Checking sanity: uniqueness of paramId and shortName keys ...\n";
    while (<>) {
        chomp;
        s/\r//g;  # Remove DOS carriage returns
        if ($first == 1) {
            $first = 0;
            next;
        }
        $lcount++;
        ($paramId, $shortName, $name, $units) = split(/\t/);

        die "Error: shortName=$shortName is duplicated (line ", $lcount+1, ")\n" if (exists $map_sn{$shortName});
        $map_sn{$shortName}++; # increment count in shortName map

        die "Error: paramId=$paramId is duplicated (line ", $lcount+1, ")\n" if (exists $map_pid{$paramId});
        $map_pid{$paramId}++; # increment count in paramId map

        die "Error: paramId=$paramId is not an integer (line ", $lcount+1, ")\n" if (!is_integer($paramId));

        my $x = $dbh->selectrow_array("select * from param.param where id = ?",undef,$paramId);
        die "Error: paramId=$x exists in the database (line ", $lcount+1, ")\n" if (defined $x);

        # Will die if it fails
        get_db_units_code($units);

        $x = $dbh->selectrow_array("select shortName from param.param where shortName = ?",undef,$shortName);
        die "Error: shortName=$x exists in the database (line ", $lcount+1, ")\n" if (defined $x);
    }
    print "\nSanity checking completed. $lcount rows checked. No errors.\nExiting.\n";
    exit 0;
}

if ($WRITE_TO_FILES) {
    create_or_append(\*OUT_PARAMID,   "$PARAMID_FILENAME");
    create_or_append(\*OUT_SHORTNAME, "$SHORTNAME_FILENAME");
    create_or_append(\*OUT_NAME,      "$NAME_FILENAME");
    create_or_append(\*OUT_UNITS,     "$UNITS_FILENAME");
    create_or_append(\*OUT_CFVARNAME, "$CFVARNAME_FILENAME");
}


while (<>) {
    chomp;
    s/\r//g;  # Remove DOS carriage returns
    if ($first == 1) {
        check_first_row_column_names($_);
        $first = 0;
        next;
    }
    $lcount++;

    ($paramId, $shortName, $name, $units, $interpol,
     $discipline, $pcategory, $pnumber,
     $type1, $scaleFactor1, $scaledValue1, $type2, $scaleFactor2, $scaledValue2,
     $stat, $aero, $constit,
     $typeGen, $localTV, $typeOfWLInt, $scaleFactorWL1, $scaledValueWL1, $scaleFactorWL2, $scaledValueWL2, $sourceSink
     ) = split(/\t/);

    die "Error: paramID \"$paramId\" is not an integer (input row=$lcount)!\n"             if (!is_integer($paramId));
    die "Error: shortName \"$shortName\" has an invalid character (input row=$lcount)!\n"  if ($shortName =~ /[ '"]/);
    die "Error: name \"$name\" should have uppercase 1st letter or digit (input row=$lcount)!\n"   if ($name !~ /^[A-Z0-9]/);
    die "Error: typeOfFirstFixedSurface \"$type1\" is not an integer (input row=$lcount)!\tPick a value from Code Table 4.5\n"
        if ($type1 ne "" && !is_integer($type1));
    die "Error: typeOfSecondFixedSurface \"$type2\" is not an integer (input row=$lcount)!\tPick a value from Code Table 4.5\n"
        if ($type2 ne "" && !is_integer($type2));

    $units = "~" if ($units eq "");
    $cfVarName = $shortName;
    $cfVarName = '\\'.$shortName if ($shortName =~ /^[0-9]/);
    $scaleFactorWL1 = undef if ($scaleFactorWL1 =~ /missing/);
    $scaledValueWL1 = undef if ($scaledValueWL1 =~ /missing/);
    $scaleFactorWL2 = undef if ($scaleFactorWL2 =~ /missing/);
    $scaledValueWL2 = undef if ($scaledValueWL2 =~ /missing/);

    $scaledValue1 = undef if ($scaledValue1 =~ /missing/);
    $scaleFactor1 = undef if ($scaleFactor1 =~ /missing/);
    $scaledValue2 = undef if ($scaledValue2 =~ /missing/);
    $scaleFactor2 = undef if ($scaleFactor2 =~ /missing/);

    if ($WRITE_TO_FILES) {
        write_out_file(\*OUT_PARAMID,   $name, $paramId);
        write_out_file(\*OUT_SHORTNAME, $name, $shortName);
        write_out_file(\*OUT_NAME,      $name, $name);
        write_out_file(\*OUT_UNITS,     $name, $units);
        write_out_file(\*OUT_CFVARNAME, $name, $cfVarName);
    }

    if ($WRITE_TO_PARAMDB) {
        my $units_code = get_db_units_code($units);
        my $is_chem = "";
        my $is_aero = "";
        my $is_srcsink = "";
        if ($aero ne "") {
            $is_aero = "1";
            $is_chem = "";
            $is_srcsink = "";
        }
        if ($constit ne "") {
            $is_aero = "";
            $is_chem = "1";
            $is_srcsink = "";
        }
        if ($sourceSink ne "") {
            $is_aero = "";
            $is_chem = "";
            $is_srcsink = "1";
        }
        my $centre = $localTV ne "" ? $centre_ecmwf : $centre_wmo;

        die "Error: Both aerosolType and constituentType cannot be set!" if ($constit ne "" && $aero ne "");
        die "Error: No contact ID provided\n" if (!$contactId);
        print "Inserting paramId $paramId (centre=" . centre_as_str($centre) . ") ...\n";
        $dbh->do("insert into param(id,shortName,name,units_id,insert_date,update_date,contact) values (?,?,?,?,?,?,?)",undef,
            $paramId, $shortName, $name , $units_code, $today_date, $today_date, $contactId) or die $dbh->errstr;

        # Check what we inserted did actually go in
        my $x = $dbh->selectrow_array("select shortName from param.param where shortName = ?",undef,$shortName);
        die "Insertion of $paramId did not work - problem with shortName $shortName\n" if (! defined $x);

        # Table 'grib' columns: param_id  edition  centre  attribute_id  attribute_value  param_version
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,4, $discipline,0);
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,8, $pcategory,0);
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,5, $pnumber,0);
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,6, $type1,0)        if ($type1 ne "");
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,13,$type2,0)        if ($type2 ne "");

        # Either missing or has a value
        if (! defined $scaledValue1 || $scaledValue1 ne "") {
            $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,9, $scaledValue1,0);
        }
        if (! defined $scaleFactor1 || $scaleFactor1 ne "") {
            $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,7, $scaleFactor1,0);
        }
        if (! defined $scaledValue2 || $scaledValue2 ne "") {
            $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,14,$scaledValue2,0);
        }
        if (! defined $scaleFactor2 || $scaleFactor2 ne "") {
            $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,15,$scaleFactor2,0);
        }
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,11,$stat,0)         if ($stat ne "");

        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,46,$aero,0)         if ($aero ne "");
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,40,$constit,0)      if ($constit ne "");

        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,53,$is_chem,0)      if ($is_chem ne "");
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,54,$is_aero,0)      if ($is_aero ne "");
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,28,$typeGen,0)      if ($typeGen ne "");

        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,16,$localTV,0)      if ($localTV ne "");
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,57,$typeOfWLInt,0)  if ($typeOfWLInt ne "");
        
        if (! defined $scaleFactorWL1 || $scaleFactorWL1 ne "") {
            $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,58,$scaleFactorWL1,0);
        }
        if (! defined $scaledValueWL1 || $scaledValueWL1 ne "") {
            $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,59,$scaledValueWL1,0);
        }
        if (! defined $scaleFactorWL2 ||$scaleFactorWL2 ne "") {
            $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,60,$scaleFactorWL2,0);
        }
        if (! defined $scaledValueWL2 || $scaledValueWL2 ne "") {
            $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,61,$scaledValueWL2,0);
        }
        $dbh->do("insert into grib values (?,?,?,?,?,?)",undef, $paramId,$edition,$centre,64,$sourceSink,0)  if ($is_srcsink ne "");

        # format is only GRIB2 hence grib1 entry=0 and grib2=1
        $dbh->do("insert into param_format(param_id,grib1,grib2) values (?,?,?)",undef,$paramId,0,1);
    }
} # for each input line

if ($WRITE_TO_FILES) {
    print "Wrote output files: $PARAMID_FILENAME $SHORTNAME_FILENAME $NAME_FILENAME $UNITS_FILENAME $CFVARNAME_FILENAME\n";
    close(OUT_PARAMID)   or die "$PARAMID_FILENAME: $!";
    close(OUT_SHORTNAME) or die "$SHORTNAME_FILENAME: $!";
    close(OUT_NAME)      or die "$NAME_FILENAME: $!";
    close(OUT_UNITS)     or die "$UNITS_FILENAME: $!";
    close(OUT_CFVARNAME) or die "$CFVARNAME_FILENAME: $!";
}
if ($WRITE_TO_PARAMDB) {
    print "Wrote to Parameter Database. Number of rows processed = $lcount\n";
}

# -------------------------------------------------------------------
sub centre_as_str {
    my $cc = shift;
    return "WMO"   if ($cc eq $centre_wmo);
    return "ECMWF" if ($cc eq $centre_ecmwf);
    return "Unknown";
}
sub get_db_units_code {
    my $u = shift;
    my $unit_id = $dbh->selectrow_array("select id from units where name = ?",undef,$u);
    die "Error: Unit not found: '$u'\n" if (!$unit_id);
    return $unit_id;
}

sub write_out_file {
    my $outfile = $_[0];
    my $name    = $_[1];
    my $key     = $_[2];

    # Assuming every parameter definition has at least discipline, category and number
    print $outfile "#$name\n";
    print $outfile "'$key' = {\n";
    print $outfile "  discipline = $discipline ;\n";
    print $outfile "  parameterCategory = $pcategory ;\n";
    print $outfile "  parameterNumber = $pnumber ;\n";

    # Optional keys
    print $outfile "  typeOfFirstFixedSurface = $type1 ;\n"                if ($type1 ne "");
    print $outfile "  typeOfSecondFixedSurface = $type2 ;\n"               if ($type2 ne "");

    $scaledValue1 = "missing()" if (! defined $scaledValue1);
    $scaledValue2 = "missing()" if (! defined $scaledValue2);
    $scaleFactor1 = "missing()" if (! defined $scaleFactor1);
    $scaleFactor2 = "missing()" if (! defined $scaleFactor2);

    print $outfile "  scaledValueOfFirstFixedSurface = $scaledValue1 ;\n"  if ($scaledValue1 ne "");
    print $outfile "  scaleFactorOfFirstFixedSurface = $scaleFactor1 ;\n"  if ($scaleFactor1 ne "");
    print $outfile "  scaledValueOfSecondFixedSurface = $scaledValue2 ;\n" if ($scaledValue2 ne "");
    print $outfile "  scaleFactorOfSecondFixedSurface = $scaleFactor2 ;\n" if ($scaleFactor2 ne "");
    print $outfile "  typeOfStatisticalProcessing = $stat ;\n"             if ($stat ne "");

    print $outfile "  aerosolType = $aero ;\n"         if ($aero ne "");
    print $outfile "  constituentType = $constit ;\n"  if ($constit ne "");
    if ($sourceSink eq "") {
        print $outfile "  is_aerosol = 1 ;\n"              if ($aero ne "");
        print $outfile "  is_chemical = 1 ;\n"             if ($constit ne "");
    } else {
        print $outfile "  is_chemical_srcsink = 1 ;\n";
        print $outfile "  sourceSinkChemicalPhysicalProcess = $sourceSink ;\n";
    }
    print $outfile "  typeOfGeneratingProcess = $typeGen ;\n"  if ($typeGen ne "");
    print $outfile "  localTablesVersion = $localTV ;\n"       if ($localTV ne "");

    print $outfile "  typeOfWavelengthInterval = $typeOfWLInt ;\n"          if ($typeOfWLInt ne "");
    print $outfile "  scaleFactorOfFirstWavelength = $scaleFactorWL1 ;\n"   if ($scaleFactorWL1 ne "");
    print $outfile "  scaledValueOfFirstWavelength = $scaledValueWL1 ;\n"   if ($scaledValueWL1 ne "");
    print $outfile "  scaleFactorOfSecondWavelength = $scaleFactorWL2 ;\n"  if ($scaleFactorWL2 ne "");
    print $outfile "  scaledValueOfSecondWavelength = $scaledValueWL2 ;\n"  if ($scaledValueWL2 ne "");

    print $outfile "}\n";
}

sub check_first_row_column_names {
    my $line = shift; # This is the first row
    my @keys = split(/\t/, $line);
    my $c = 0;
    my $numkeys = scalar @keys;
    my $numcols = scalar @columns;
    die "Error: 1st row column titles wrong: Expected $numcols columns, got $numkeys.\nColumns should be:\n@columns\n"
        if ($numkeys != $numcols);
    for ( my $i = 0; $i < $numkeys; $i++ ) {
        if ( $keys[$i] ne $columns[$i] ) {
            die "Error: 1st row column titles wrong: check column ", $i+1, ". Expected '$columns[$i]', got '$keys[$i]'.\n";
        }
    }
    #if (@keys ~~ @columns) {
    #    print "[@keys] and [@columns] match\n";
    #} else {
    #    die "Error: must use these columns: @columns\n";
    #}
}

sub create_or_append {
    my $outfile = $_[0];
    my $fname   = $_[1];

    if (-f "$fname") {
        open($outfile,  ">>$fname") or die "Error: $fname: $!";
    } else {
        open($outfile,  ">$fname")  or die "Error: $fname: $!";
    }
}

sub is_integer {
    my $val = shift;
    return ($val =~ /^\d+$/);
}

sub usage {
   print <<USAGE;

Usage: $0 [-s] [-f] [-p] file.tsv
       Input has to be a tab-separated values (TSV) file

       -s  Perform sanity checks and exit
       -f  Write out def files (paramId.def, name.def etc)
       -p  Write to Parameter Database (Be careful!)

USAGE
   exit 1
}
