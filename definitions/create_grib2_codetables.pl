use strict;

# ------------------------------------------------------------------------
# Get the CodeFlag.txt file from WMO
# Convert it to TSV (tab-separated-values format):
#  perl csv_2_tsv.pl CodeFlag.txt
# This will create the file CodeFlag.txt.tsv
# Create the directory corresponding to the new GRIB2 version:
#  mkdir -p definitions/grib2/tables/xx
# cd to that directory:
#  cd definitions/grib2/tables/xx
# Run this script on that TSV file:
#  perl create_grib2_codetables.pl < /path/to/CodeFlag.txt.tsv
# This should create all the *.table files
# ------------------------------------------------------------------------

# The WMO CSV file should have the following columns
#   Title_en
#   SubTitle_en
#   CodeFlag
#   Value
#   MeaningParameterDescription_en
#   Note_en
#   noteIDs
#   UnitComments_en
#   Status

my $EXPECTED_COLUMN_COUNT = 9; # As of v29; May 2022
my $recnum = 0;
my $codetable;
my $discipline;
my $category;
my $filename;

while (<>) {
    ++$recnum;
    if ($recnum == 1) {
        my @columns = split(/\t/);
        my $column_count = @columns;
        if ($column_count != $EXPECTED_COLUMN_COUNT) {
            die "Error: Incorrect column count on first row. Expected $EXPECTED_COLUMN_COUNT but found $column_count!\n";
        }
        next;
    }

    s/Hovmöller/Hovmoller/;
    s/Carrée/Carree/;
    s/μm/um/;
    s/°C/degree C/;
    s/f\(n\) = C2 × f\(n-1\)/f(n) = C2 * f(n-1)/;
    s/\(see separate doc or pdf file\)/see separate doc or pdf file/;

    my ($title, $subtitle, $codeFlag, $value, $meaning, $note, $noteID, $unit, $status) = split(/\t/);

    if ($title =~ /Code table ([0-9.]+)/) {
        $codetable = $1;
        if ($subtitle =~ /Product [Dd]iscipline (\d+).*parameter category (\d+)/) {
            $discipline = $1;
            $category = $2;
            $filename = "$codetable.$discipline.$category.table";
            WriteFile($filename, $subtitle, $codeFlag, $meaning, $unit);
        }
        elsif ($subtitle =~ /Product discipline (\d+)/) {
            $discipline = $1;
            $filename = "$codetable.$discipline.table";
            WriteFile($filename, $subtitle, $codeFlag, $meaning, $unit);
        }
        elsif ($subtitle eq "") {
            $filename = "$codetable.table";
            WriteFile($filename, $title, $codeFlag, $meaning, $unit);
        }
    } elsif ($title =~ /Flag table ([0-9.]+)/) {
        $codetable = $1;
        $filename = "$codetable.table";
        WriteFlagTable($filename, $title, $codeFlag, $value, $meaning, $unit);
    }

    #exit if ($recnum >800);
}
###################################################################################################
sub WriteFile {
    my($filename, $title, $codeFlag, $meaning, $unit) = @_;
    if (!-e $filename) {
        print "Creating $filename\n";
        open (MYFILE, ">>$filename");
        #print MYFILE "# Automatically generated by ./create_tables.pl from database fm92_grib2\@wrep-db-misc-prod, do not edit\n";
        print MYFILE "# $title\n";
    }
    my $unit_text = ($unit eq "" ? "" : "($unit)");
    $unit_text =~ s/\(\(Code /(Code /;
    $unit_text =~ s/\)\)/)/;
    if ($codeFlag =~ /\-/) {
        if ($unit_text eq "") {
            print MYFILE "# $codeFlag $meaning\n";
        } else {
            print MYFILE "# $codeFlag $meaning $unit_text\n";
        }
    } else {
        my $codeFlag1 = $codeFlag; # A number
        my $codeFlag2 = $codeFlag; # A number or string abbreviation
        if ($filename eq "1.4.table") {
            # Special case. Do not just put 2nd code, translate it to shortName for 'mars type'
            $codeFlag2 = TranslateCodes_Table_1_4($codeFlag);
        }
        elsif ($filename eq "3.12.table") {
            $codeFlag2 = TranslateCodes_Table_3_12($codeFlag);
        }
        elsif ($filename eq "3.15.table") {
            $codeFlag2 = TranslateCodes_Table_3_15($codeFlag);
        }
        elsif ($filename eq "4.4.table") {
            $codeFlag2 = TranslateCodes_Table_4_4($codeFlag);
        }
        elsif ($filename eq "4.5.table") {
            $codeFlag2 = TranslateCodes_Table_4_5($codeFlag);
        }
        elsif ($filename eq "4.241.table") {
            $codeFlag2 = TranslateCodes_Table_4_241($codeFlag);
        }
        elsif ($filename eq "4.242.table") {
            $codeFlag2 = TranslateCodes_Table_4_242($codeFlag);
        }
        elsif ($filename eq "4.252.table") {
            $codeFlag2 = TranslateCodes_Table_4_252($codeFlag);
        }
        elsif ($filename eq "4.10.table") {
            $codeFlag2 = TranslateCodes_Table_4_10($codeFlag);
        }

        if ($unit_text eq "") {
            print MYFILE "$codeFlag1 $codeFlag2 $meaning\n";
        } else {
            print MYFILE "$codeFlag1 $codeFlag2 $meaning $unit_text\n";
        }
    }
}

###################################################################################################
sub WriteFlagTable{
    my($filename, $title, $codeFlag, $value, $meaning, $unit) = @_;
    if (!-e $filename) {
        print "Creating $filename\n";
        open (MYFILE, ">>$filename");
        #print MYFILE "# Automatically generated by ./create_tables.pl from database fm92_grib2\@wrep-db-misc-prod, do not edit\n";
        print MYFILE "# $title\n";
    }
    my $unit_text = ($unit eq "" ? "" : " ($unit)");
    if ($codeFlag =~ /\-/) {
        print MYFILE "# $codeFlag $meaning$unit_text\n";
    } else {
        print MYFILE "$codeFlag $value $meaning$unit_text\n";
    }
}

###################################################################################################
sub TranslateCodes_Table_4_10 {
    my ($code) = @_;
    return "avg"   if ($code == 0);
    return "accum" if ($code == 1);
    return "max"   if ($code == 2);
    return "min"   if ($code == 3);
    return "diff"  if ($code == 4);
    return "rms"   if ($code == 5);
    return "sd"    if ($code == 6);
    return "cov"   if ($code == 7);
    return "ratio" if ($code == 9);
    return "missing" if ($code == 255);
    return $code;
}

sub TranslateCodes_Table_3_12 {
    my ($code) = @_;
    return "ring" if ($code == 0);
    return "nested" if ($code == 1);
    return $code;
}

sub TranslateCodes_Table_3_15 {
    my ($code) = @_;
    return "pt" if ($code == 107);
    return "pv" if ($code == 109);
    return $code;
}

sub TranslateCodes_Table_1_4 {
    my ($code) = @_;
    return "an" if ($code eq "0");
    return "fc" if ($code eq "1");
    return "af" if ($code eq "2");
    return "cf" if ($code eq "3");
    return "pf" if ($code eq "4");
    return "cp" if ($code eq "5");
    return "sa" if ($code eq "6");
    return "ra" if ($code eq "7");
    return "ep" if ($code eq "8");
    return "missing" if ($code eq "255");
    return $code;
}

sub TranslateCodes_Table_4_4 {
    my ($code) = @_;
    return "m"    if ($code eq "0");
    return "h"    if ($code eq "1");
    return "D"    if ($code eq "2");
    return "M"    if ($code eq "3");
    return "Y"    if ($code eq "4");
    return "10Y"  if ($code eq "5");
    return "30Y"  if ($code eq "6");
    return "C"    if ($code eq "7");
    return "3h"   if ($code eq "10");
    return "6h"   if ($code eq "11");
    return "12h"  if ($code eq "12");
    return "s"    if ($code eq "13");
    return $code;
}

sub TranslateCodes_Table_4_5 {
    my ($code) = @_;
    return "sfc"  if ($code eq "1" || $code eq "7" ||
            $code eq "8"   || $code eq "17"  || $code eq "18"  ||
            $code eq "101" || $code eq "103" || $code eq "106" ||
            $code eq "162" || $code eq "166" || $code eq "174" ||
            $code eq "177");
    return "pl"   if ($code eq "100");
    return "ml"   if ($code eq "105");
    return "pt"   if ($code eq "107");
    return "pv"   if ($code eq "109");
    return "hhl"  if ($code eq "118");
    return "hpl"  if ($code eq "119");
    return "sol"  if ($code eq "151");
    return "sol"  if ($code eq "114");
    return "sol"  if ($code eq "152");
    return $code;
}

sub TranslateCodes_Table_4_241 {
    my ($code) = @_;
    return "UNDEF"  if ($code eq "0");
    return "UNMOD"  if ($code eq "1");
    return "SNOW"   if ($code eq "2");
    return "FLOOD"  if ($code eq "3");
    return "ICE"    if ($code eq "4");
    return "ITCW"   if ($code eq "5");
    return "ITCS"   if ($code eq "6");
    return "AGG"    if ($code eq "7");
    return $code;
}

sub TranslateCodes_Table_4_242 {
    my ($code) = @_;

    return "GCV2009"      if ($code eq "1");
    return "GLC2000"      if ($code eq "2");
    return "ECOCLIMAP"    if ($code eq "3");
    return "ECOCLIMAP-SG" if ($code eq "4");
    return "GLCCv2BATS"   if ($code eq "5");
    return $code;
}

sub TranslateCodes_Table_4_252 {
    my ($code) = @_;

    return "SEOC" if ($code eq "1");
    return "INWB" if ($code eq "2");
    return "BARE" if ($code eq "3");
    return "HIV" if ($code eq "4");
    return "LOV" if ($code eq "5");
    return "URBAN" if ($code eq "6");
    return "SEAO" if ($code eq "1001");
    return "LAKE" if ($code eq "1002");
    return "RIVE" if ($code eq "1003");
    return "NO" if ($code eq "1004");
    return "ROCK" if ($code eq "1005");
    return "SNOW" if ($code eq "1006");
    return "BOBD" if ($code eq "1007");
    return "TEBD" if ($code eq "1008");
    return "TRBD" if ($code eq "1009");
    return "TEBE" if ($code eq "1010");
    return "TRBE" if ($code eq "1011");
    return "BONE" if ($code eq "1012");
    return "TENE" if ($code eq "1013");
    return "BOND" if ($code eq "1014");
    return "SHRB" if ($code eq "1015");
    return "BOGR" if ($code eq "1016");
    return "GRAS" if ($code eq "1017");
    return "TROG" if ($code eq "1018");
    return "C3W" if ($code eq "1019");
    return "C3S" if ($code eq "1020");
    return "C4" if ($code eq "1021");
    return "FLTR" if ($code eq "1022");
    return "FLGR" if ($code eq "1023");
    return "LCZ1" if ($code eq "1024");
    return "LCZ2" if ($code eq "1025");
    return "LCZ3" if ($code eq "1026");
    return "LCZ4" if ($code eq "1027");
    return "LCZ5" if ($code eq "1028");
    return "LCZ6" if ($code eq "1029");
    return "LCZ7" if ($code eq "1030");
    return "LCZ8" if ($code eq "1031");
    return "LCZ9" if ($code eq "1032");
    return "LCZ10" if ($code eq "1033");
    return "TEBDU" if ($code eq "1034");
    return "TRBDU" if ($code eq "1035");
    return "TEBEU" if ($code eq "1036");
    return "TRBEU" if ($code eq "1037");
    return "BONEU" if ($code eq "1038");
    return "TENEU" if ($code eq "1039");
    return "BONDU" if ($code eq "1040");
    return "GNATU" if ($code eq "1501");
    return "GNOFO" if ($code eq "1502");
    return "GFORE" if ($code eq "1503");
    return "GNOVE" if ($code eq "1504");
    return "GTREE" if ($code eq "1505");
    return "GLVEG" if ($code eq "1506");
    return "GNFLV" if ($code eq "1507");
    return "GCROP" if ($code eq "1508");
    return "GGRAS" if ($code eq "1509");
    return "GBARO" if ($code eq "1510");
    return "GC3CR" if ($code eq "1511");
    return "GBFOR" if ($code eq "1512");
    return "GNFOR" if ($code eq "1513");
    return "GDBFO" if ($code eq "1514");
    return "GTGRA" if ($code eq "1515");
    return "GNFNF" if ($code eq "1516");
    return "GBFFT" if ($code eq "1517");
    return "GBTRE" if ($code eq "1518");
    return "GCGRA" if ($code eq "1519");
    return "GBDTR" if ($code eq "1520");
    return "GBETR" if ($code eq "1521");
    return "GNETR" if ($code eq "1522");
    return "GLCZU" if ($code eq "1523");
    return "GIWAT" if ($code eq "1524");
    return "GPTEBD" if ($code eq "1525");
    return "GPTRBD" if ($code eq "1526");
    return "GPTEBE" if ($code eq "1527");
    return "GPTRBE" if ($code eq "1528");
    return "GPBONE" if ($code eq "1529");
    return "GPTENE" if ($code eq "1530");
    return "GPBOND" if ($code eq "1531");
    return "CRMF" if ($code eq "2001");
    return "SHGR" if ($code eq "2002");
    return "EVNE" if ($code eq "2003");
    return "DENE" if ($code eq "2004");
    return "DEBR" if ($code eq "2005");
    return "EVBR" if ($code eq "2006");
    return "TAGR" if ($code eq "2007");
    return "DESE" if ($code eq "2008");
    return "TUND" if ($code eq "2009");
    return "IRCR" if ($code eq "2010");
    return "SEDE" if ($code eq "2011");
    return "ICGL" if ($code eq "2012");
    return "BOMA" if ($code eq "2013");
    return "INWA" if ($code eq "2014");
    return "OCEA" if ($code eq "2015");
    return "EVSH" if ($code eq "2016");
    return "DESH" if ($code eq "2017");
    return "MFWO" if ($code eq "2018");
    return "INFO" if ($code eq "2019");
    return "WLMI" if ($code eq "2020");
    return "BASO" if ($code eq "2021");
    return "URBA" if ($code eq "2022");
    return "LORI" if ($code eq "2023");
    return "HIRI" if ($code eq "2024");
    return "GLVE" if ($code eq "2501");
    return "GHVE" if ($code eq "2502");
    return "PFIC" if ($code eq "3001");
    return "RACR" if ($code eq "3002");
    return "MCVE" if ($code eq "3003");
    return "MVCR" if ($code eq "3004");
    return "COBS" if ($code eq "3005");
    return "CLBD" if ($code eq "3006");
    return "OPBD" if ($code eq "3007");
    return "CLNE" if ($code eq "3008");
    return "ONDE" if ($code eq "3009");
    return "COBN" if ($code eq "3010");
    return "MFSG" if ($code eq "3011");
    return "MGFS" if ($code eq "3012");
    return "COSH" if ($code eq "3013");
    return "COHV" if ($code eq "3014");
    return "SPVE" if ($code eq "3015");
    return "COBFF" if ($code eq "3016");
    return "CLBFF" if ($code eq "3017");
    return "COVEF" if ($code eq "3018");
    return "ASAA" if ($code eq "3019");
    return "BAAR" if ($code eq "3020");
    return "WABO" if ($code eq "3021");
    return "PESI" if ($code eq "3022");
    return "GRDT" if ($code eq "3501");
    return "TBECO" if ($code eq "4001");
    return "TBDC" if ($code eq "4002");
    return "TBDO" if ($code eq "4003");
    return "TNECO" if ($code eq "4004");
    return "TNDCO" if ($code eq "4005");
    return "TMLCO" if ($code eq "4006");
    return "TCOFF" if ($code eq "4007");
    return "TCOFS" if ($code eq "4008");
    return "MTVG" if ($code eq "4009");
    return "TBURN" if ($code eq "4010");
    return "SCOE" if ($code eq "4011");
    return "SCOD" if ($code eq "4012");
    return "HCCO" if ($code eq "4013");
    return "SHSC" if ($code eq "4014");
    return "FSHCO" if ($code eq "4015");
    return "CROP" if ($code eq "4016");
    return "MCTNV" if ($code eq "4017");
    return "MCSH" if ($code eq "4018");
    return "BAARE" if ($code eq "4019");
    return "WBNA" if ($code eq "4020");
    return "SINA" if ($code eq "4021");
    return "URAR" if ($code eq "4022");
    return $code;
}
