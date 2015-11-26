#!/usr/local/apps/perl/current/bin/perl -I/usr/local/lib/metaps/perl

use Getopt::Long;
use strict;

#--------------------------------------------------
#
#--------------------------------------------------

#The root page of examples in confluence!!!
my $rootPage="API examples";

my @langKeys=("f","p","c");

my %langName = ("f" => "Fortran 90",
             "c"  => "C",
             "p"  => "Python");

my %langType = ("f" => "none",
             "c"  => "cpp",
             "p"  => "python");

my %langSuffix = ("f" => "f90",
             "c"  => "c",
             "p"  => "py");

my %langDir = ("f" => "F90",
             "c"  => "C",
             "p"  => "python");

my %langComment = ("f" => "!",
             "c"  => "*",
             "p"  => "#");


my $CONF=$ENV{CONF};
die "Env var CONF should point to confluence.sh script" if ( $CONF eq "" || ! -f "$CONF" );

my $inRootDir;
my $confSpace="ECC";

#---------------------------------
# Read arguments
#---------------------------------

GetOptions("inDir=s" => \$inRootDir,
           "confSpace=s" => \$confSpace)
 	or die("Error in command line arguments\n");

print "confluence space=".$confSpace."\n";

#----------------------------------
# Dirs
#----------------------------------

#The root dir of the examples
$inRootDir="../examples";

#The genareted html files are stored here
my $htmlDir=$ENV{TMPDIR}."/res_html";


my %allExamples=read_examples();

#----------------------------------------
# Preparations
#----------------------------------------

unless ( -d  $htmlDir ) {
    mkdir $htmlDir	
}

#---------------------------------------------------
# Loop for the code types (e.g. grib, bufr, etc.)
#---------------------------------------------------

foreach my $cType (keys %allExamples) {

    print "----------------------------------------\n";
    print "  Processing examples for: $cType\n";
    print "----------------------------------------\n";

    #Get the examples
    my @examples=@{$allExamples{$cType}};

    if($#examples+1 == 0) {
        die "No examples are defined!\n";
    }

    #---------------------------------------------------
    # Create a parent page for the examples of this type
    #---------------------------------------------------

    my $parentPage=parentPageTitle($cType);
    makeParentPage($rootPage,$parentPage,$htmlDir,$cType,@examples);

    #--------------------------------
    # Loop for the examples
    #--------------------------------

    foreach my $name (@examples) {

        my $fOut=$htmlDir."/".$cType."_".$name.".html";
        open(OUT,">$fOut") or die "$fOut: $!";

        #Description
        print OUT "<h3>Description</h3>";
        print OUT "This example shows: <i>".getDescription($name)."</i>\n";

        #Create a tab container
        print OUT "<h3>Source code</h3>";
        print OUT "<ac:structured-macro ac:name=\"auitabs\"><ac:parameter ac:name=\"direction\">horizontal</ac:parameter><ac:rich-text-body>";

        #Add a tab for each language
        foreach my $i (@langKeys) {

            my $tabName=$langName{$i};
            my $fName=$name.".".$langSuffix{$i};
            my $fPath=$inRootDir."/".$langDir{$i}."/".$fName;
            my $lType=$langType{$i};

            #Read sourcode into html format
            my $c=getSourceCode($fName,$fPath,$lType);

            #If there is a source creates a tab for it
            if(length($c) > 0) {
                print OUT "<ac:structured-macro ac:name=\"auitabspage\"><ac:parameter ac:name=\"title\">".$tabName."</ac:parameter>
                <ac:rich-text-body>".$c."</ac:rich-text-body></ac:structured-macro>";
            }
        }

        #Close tab container
        print OUT  "</ac:rich-text-body></ac:structured-macro>";

        close OUT;

        #Upload the file to confluence
        my $pageTitle=examplePageTitle($name);
        loadToConf($fOut,$pageTitle,$parentPage);
    }
}

#===========================================================
#===========================================================
#
# FUNCTIONS
#
#===========================================================
#===========================================================

#====================================================
#
# Functions related to the examples
#
#====================================================

#------------------------------------------
# Read in example list
#------------------------------------------

sub read_examples {

	my %res=();
	my $fIn="examples.par";
	open(IN,"<$fIn") or die "$fIn: $!";
	
    my $line;	
    my $actType;
    while (defined ($line = <IN>)) {
		chomp($line);
		if ($line =~ /.*#/) {next;}
		
        #print $line."\n";

        $line =~ s/\"//g;

        my ($a)=($line =~ /(\S+)\s*\(/);
        if($a and $a !~ /\!/) {
            $actType=$a;
            #print "type: ".$a."\n";
            #$res{$actType};
            next;
        }

        if( $line =~ /\)/) {
            $actType="";
            next;
        }

        if(length($actType) > 0) {
            my ($b) =($line =~ /(\S+)/);
            #print "ex: ".$b."\n";
            if($b) {
                push(@{$res{$actType}},$b);
            }
        }
	}	

    close IN;

	return %res;
}

#-----------------------------------------------------------------------------
# Genarate the page title for the parent page for a given type of examples
#-----------------------------------------------------------------------------

sub parentPageTitle {

   my ($codeType) = @_;
   return $codeType." examples";
}

#--------------------------------------------------------
# Create parent page for a given type of examples
#--------------------------------------------------------

sub makeParentPage {
    my ($rootPage,$pageTitle,$outDir,$cType,@xmp) = @_;

    my $f=$outDir."/parent_".$cType."html";

    open(OUT,">$f") or die "$f: $!";

    my $str="<table><tbody>";
    $str=$str."<tr><th>Example</th><th>Description</th></tr>";

    foreach my $xm (@xmp) {
        $str=$str."<tr><td>".linkToPage($xm,examplePageTitle($xm))."</td><td>".getDescription($xm)."</td></tr>";
    }
    $str=$str."</tbody></table>";

    print OUT $str;

    close OUT;

    #Load it to confluence
    loadToConf($f,$pageTitle,$rootPage);
}

#-----------------------------------------------------------------------------
# Genarate the page title for a given example
#-----------------------------------------------------------------------------
sub examplePageTitle {

   my ($exName) = @_;
   return $exName;
}

#-----------------------------------------------------------
# Get the file path for an example with the given language
#-----------------------------------------------------------

sub examplePath {
    my ($name,$lang) = @_;
    return $inRootDir."/".$langDir{$lang}."/".$name.".".$langSuffix{$lang};
}

#-----------------------------------------------------------
# Check if the file for a given example exists.
#-----------------------------------------------------------

sub hasExample {

    my ($name,$lang) = @_;
    my $f=examplePath($name,$lang);
    if( -e $f ) {return 1};

    print "$f no!!!\n";

    return 0;
}

#------------------------------------------------------------
# Generate confluence formatted html string form a source file
#------------------------------------------------------------

sub getSourceCode {

    my ($title,$fPath,$lang) = @_;
    my $codeTxt;

    print $fPath."\n";

    my $str;

    if($lang eq "") {
        $codeTxt=fortran_file_to_string($fPath);
        #return "<p /><code><ac:macro ac:name=\"html\"><ac:plain-text-body><![CDATA[".$codeTxt."]]></ac:plain-text-body></ac:macro></code>";
        return "<p /><pre>".$codeTxt."</pre>";
    } else {
        open(IN,"<$fPath") or  return ""; # die "$fPath: $!";
        $codeTxt=join("", <IN>);
        close IN;
    }

    $str="
    <ac:structured-macro ac:name=\"code\">
        <ac:parameter ac:name=\"title\">".$title."</ac:parameter>
        <ac:parameter ac:name=\"linenumbers\">false</ac:parameter>
        <ac:parameter ac:name=\"language\">".$lang."</ac:parameter>
        <ac:plain-text-body><![CDATA[".$codeTxt."]]></ac:plain-text-body>
    </ac:structured-macro>";

    return $str;
}

#--------------------------------------------------------
# Extract the description of a given example
#--------------------------------------------------------

sub getDescription {

    my ($name) = @_;
    my $res;  

    foreach my $lang (@langKeys) {
            
        if(hasExample($name,$lang) == 1 ) { 

            my $f=examplePath($name,$lang);

            my $found=0;
            open (IN,"<$f") or return "";
            while (defined (my $line = <IN>)) {
                my $comment=$langComment{$lang};

                if($comment ne "*") {
                    $line =~ s/\Q${comment}/\*/g;
                }
         
                #First row
                if($found==0) {
                    if($line =~ /\*\s*Description:/) {
                        $found=1;
                        my ($r)=($line =~ /\*\s*Description:\s*(.+)/);
                        if(length($r) > 2) {
                            $res=$r;
                        }
                    }
                } else {
                    my ($r)=($line =~ /\*\s+(.+)/);
                    if(length($r) > 2) {
                        $res=$res." ".$r;
                    } else {
                        last;
                    }
                }
            }

            if($res) {
                return $res;
            }

        }
        
        #print $f."\n";
        #print "descr: ".$res."\n";
    }

    return $res;
}

#====================================================
#
# Generic functions related to confluence
#
#====================================================

#--------------------------------
# Upload a page to confluence
#--------------------------------

sub loadToConf {

    my ($fHtml,$title,$parentPage) = @_;
    system("$CONF -a addPage --noConvert --replace --space \"$confSpace\" --title \"$title\" --parent \"$parentPage\" --file $fHtml");
}

#---------------------------------------
# Create link to a page
#---------------------------------------

sub linkToPage {

    my ($page,$label) = @_;

    my $str="<ac:link>
  <ri:page ri:content-title=\"".$page."\"/>
  <ac:plain-text-link-body>
    <![CDATA[".$label."]]>
  </ac:plain-text-link-body>
  </ac:link>";

   return $str;
}

sub makeInfo {

    my ($text) = @_;
    return "<ac:structured-macro ac:name=\"info\"><ac:parameter ac:name=\"icon\">false</ac:parameter>".
              "<ac:rich-text-body>".$text."</ac:rich-text-body></ac:structured-macro>";
}
