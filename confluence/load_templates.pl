#!/usr/local/apps/perl/current/bin/perl -I/usr/local/lib/metaps/perl -I.

use confUtils;
use Getopt::Long;
use strict;

my %res=();

#The confluence space
my $confSpace="ECC";

#---------------------------------
# Read arguments
#---------------------------------

GetOptions("confSpace=s" => \$confUtils::confSpace)
    or die("Error in command line arguments\n");

print "confluence space=".$confUtils::confSpace."\n";

#----------------------------------
# Dirs
#----------------------------------

#The root dir of the examples
my $inFile="../definitions/bufr/templates/BufrTemplate.def";

#The genareted html files are stored here
my $htmlDir=$ENV{TMPDIR}."/res_html";

#--------------------------------------------------
# The confluence pages
#--------------------------------------------------

#The root page of templates in confluence!!!
my $pageTitle="BUFR templates";
my $parentPage="";

#----------------------------------------
# Preparations
#----------------------------------------

unless ( -d  $htmlDir ) {
    mkdir $htmlDir  
}

#---------------------------------------------------
# Read the templates file
#---------------------------------------------------

read_templates();

#---------------------------------------------------
# Generate confluence tabel
#---------------------------------------------------

my $fOut=$htmlDir."/bufr_templates.html";
open(OUT,">$fOut") or die "$fOut: $!";

print OUT "<p>ecCodes provides a simple way to define the structure of a BUFR message using the key \"BufrTemplate\".<br />
Here are all the available values for this key:</p><br />";


my $header="Teplate name</th><th>Unexpanded descriptors</th>";
      
print OUT "<table><tbody>"."\n";
print OUT "<tr><th>Template name</th><th>Unexpanded descriptors</th></tr>\n";

foreach my $name (sort {$a cmp $b} keys %res) {

    #Get the examples
    my @desc=$res{$name};

    print OUT "<tr><td>$name</td><td>".$res{$name}."</td></tr>\n";
    print  "$name--> $res{$name}\n";
}

print OUT "</tbody></table>"."\n";   

close(OUT);

#Upload to confluence
confUtils::loadToConf($fOut,$pageTitle,$parentPage); 


#==========================================================
#===========================================================
#
# FUNCTIONS
#
#===========================================================
#===========================================================

sub read_templates {

    #my %res=();
    my $fIn=$inFile;
    open(IN,"<$fIn") or die "$fIn: $!";
    
    my $line;   
    my $actType;
    
    while (defined ($line = <IN>)) {
        chomp($line);
        if ($line =~ /.*#/) {next;}
        
        #print $line."\n";
    
        $line =~ s/\s+//g;
    
        #print $line."\n";
        my @desc;
        my ($a,$b)=($line =~ /\"(\S+)\"=\{unexpandedDescriptors=(\S+);\}/);
        
        unless ($a) {next;}

        $res{$a}=$b;

        my ($c) = ($b =~ /\[(\S+)\]/);
        if($c) {
            $res{$a}=$c;
        }

        #print "    ".$res{$a}."\n";
    }
    
    close(IN);
}
      

