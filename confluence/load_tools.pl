#!/usr/local/apps/perl/current/bin/perl -I/usr/local/lib/metaps/perl -I.

use confUtils;
use Getopt::Long;
use strict;

#--------------------------------------------------
# 
#--------------------------------------------------

#The root page of examples in confluence!!!
my $rootPage="Command line tools";

#---------------------------------
# Read arguments
#---------------------------------

#GetOptions("inDir=s" => \$inRootDir)
# 	or die("Error in command line arguments\n");
 
#----------------------------------
# Dirs
#----------------------------------

#The tools executales and shell scripts 
my $toolsDir="../tools";

#The genareted html files are stored here
my $htmlDir=$ENV{TMPDIR}."/res_tools_html";

my %cmdSwitches=();
my %allTools=read_config();

#----------------------------------------
# Preparations
#----------------------------------------

unless ( -d  $htmlDir ) {
    mkdir $htmlDir	
}

#---------------------------------------------------
# Loop for the code types (e.g. grib, bufr, etc.)
#---------------------------------------------------

foreach my $cType (keys %allTools) {
    
    print "----------------------------------------\n";
    print "  Processing tools for: $cType\n";
    print "----------------------------------------\n";
    
    #Get the examples
    my @tools=@{$allTools{$cType}};  
    
    if($#tools+1 == 0) {
        die "No tools are defined!\n";
    }

    #---------------------------------------------------
    # Create a parent page for the tools of this type
    #---------------------------------------------------

    print "--> Genarate parent page\n";

    my $parentPage=parentPageTitle($cType);
    makeParentPage($rootPage,$parentPage,$htmlDir,$cType,@tools);
   
    #--------------------------------
    # Loop for the tools
    #--------------------------------
    
    foreach my $name (@tools) {
    
        print "-------------------------------\n--> tool: ".$name."\n";
        
        my $fOut=$htmlDir."/".$cType."_".$name.".html";
        open(OUT,">$fOut") or die "$fOut: $!"; 
        
        my $str=getDoc($name);
        
        $str=$str.getExample($name);
        
        print OUT $str;
        
        close OUT;
        
        #Upload the file to confluence 
        my $pageTitle=toolsPageTitle($name);
        confUtils::loadToConf($fOut,$pageTitle,$parentPage);       
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

sub read_config {

	my %res=();
	my $fIn="tools.par";
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
            
            #If ! presents we skip this tool
            if($line =~ /^\s*!/) {
                next;
            
            #A switch is specified for the tool's shell script
            } elsif( $line =~ /\S+ \S+/) {
                      
                 my ($b,$c)=($line =~ /(\S+) (\S+)/);  
                 if($b) {
                        push(@{$res{$actType}},$b);
                        $cmdSwitches{$b}="$c";
                 }
            
            #The tools shell script has to be called without a switch          
            } else {          
                my ($b) =($line =~ /(\S+)/);
                #print "ex: ".$b."\n";
                if($b) {
                    push(@{$res{$actType}},$b);
                }    
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
   
   return $codeType." tools";  
}   

#--------------------------------------------------------
# Create parent page for a given type of examples
#--------------------------------------------------------

sub makeParentPage {
    my ($rootPage,$pageTitle,$outDir,$cType,@xmp) = @_;
    
    my $f=$outDir."/parent_".$cType."html";
  
    open(OUT,">$f") or die "$f: $!"; 
        
    my $str="<table><tbody>";
    $str=$str."<tr><th>Name</th><th>Description</th></tr>";
    
    foreach my $xm (@xmp) {
        $str=$str."<tr><td>".confUtils::linkToPage($xm,toolsPageTitle($xm))."</td><td>".getDescription($xm)."</td></tr>";
    }        
    $str=$str."</tbody></table>";                   
    
    print OUT $str;
    
    close OUT;
    
    #Load it to confluence
    confUtils::loadToConf($f,$pageTitle,$rootPage);
}

#-----------------------------------------------------------------------------
# Genarate the page title for a given example
#-----------------------------------------------------------------------------
 
sub toolsPageTitle {
   
   my ($exName) = @_;
   
   return $exName; 
}    
  
#-----------------------------------------------------------
# Get the file path for an example with the given language 
#-----------------------------------------------------------

sub toolsPath {    
    my ($name) = @_;
    return $toolsDir."/".$name;
}

#-----------------------------------------------------------
# Check if the file for a given example exists.  
#-----------------------------------------------------------

sub hasExample {
    
    my ($name,$lang) = @_;
    #my $f=examplePath($name,$lang);
    #if( -e $f ) {return 1};
    
    #print "$f no!!!\n";
    
    return 0;
}

sub docPath {
    
    my ($name) = @_;
    
    my $fTmp=$htmlDir."/doc_".$name.".tmp";
    
    my $exe=$name;
    if( -e $toolsDir."/".$exe) {
        
        my $swt;
        if(defined($cmdSwitches{$name})) {
            $swt=$cmdSwitches{$name};
        }    
       
	my $cmd;
	
	#If the switch is desc it does not run the shell script but 
	#use the .desc file"
	if ($swt eq "desc") {
		$cmd="cd ".$toolsDir."; cp ".$name.".desc ".$fTmp;
	} else {
        	$cmd="cd ".$toolsDir.";./".$exe."  > ".$fTmp;
        }
	
	print "\tGenerate documentation with command:\n\t\t".$cmd."\n";
	
	system($cmd);
        return $fTmp;
    }
    
    return "";    
} 
   
#------------------------------------------------------------
# 
#------------------------------------------------------------

sub examplePath {
    
    my ($name) = @_;
    
    my $fTmp=$htmlDir."/ex_".$name.".tmp";
    
    my $script=$name.".sh";
    if( -e $toolsDir."/".$script) {
        
        my $cmd="cd ".$toolsDir.";./".$script."  > ".$fTmp;
        print "\tGenerate examples text with command:\n\t\t".$cmd."\n";        
        system($cmd);
        return $fTmp;
    } 
    
    return "";    
}    


#------------------------------------------------------------
# Generate confluence formatted html string form a source file
#------------------------------------------------------------

sub getDoc {
    
    my ($name) = @_;
    
    my $fPath=docPath($name);
    
    my $str;
    my $tbody=0;
       
    open (IN,"<$fPath") or return "";
    while (defined (my $line = <IN>)) {
            
        $line =~ s/^\s+//g;
        $line=~ s/\n//g;
        
        if($line =~ /^NAME\s*/) {
            next;
        }
        
        if($line =~ /^DESCRIPTION|USAGE|OPTIONS\s*/) {
            
            if($tbody == 1) {
                $str=$str."</p>";
                $tbody=0;
            }    
            
            $str=$str."<h2>".$line."</h2>";
           
            unless($line =~ /OPTIONS\s*/)
            {
                $str=$str."<p>";
                $tbody=1;
            }    
            
        } elsif( $line =~ /^\s*-\w /) {
            
            if($tbody == 1) {
                $str=$str."</p>";
                $tbody=0;
            }    
            
            my @v=split(/\t|\s{2,}/,$line);
            
            if($#v+1 > 0) {
                $str=$str."<br /><b>".@v[0]."</b><br /><p>";
                if($#v+1 > 1) {
                    for(my $k=1; $k <= $#v; $k++) {
                        $str=$str.@v[$k]." ";
                    }
                }        
            }

            $tbody=1;
        
        } else {
            
            if($line =~ /\S/) {
                $str=$str.$line." ";
            } elsif ($tbody == 1) {
                $str=$str."</p>";
                $tbody=0;
            }
        }  
    } 
    
    close IN;
    if($tbody == 1) {
        $str=$str."</p>";
    }    
    
    return $str;
}    

#--------------------------------------------------------
# Extract the description of a given example
#--------------------------------------------------------

sub getDescription {
    
    my ($name) = @_;
    
    my $fPath=docPath($name);
    
    print "\tExtract desription from: ".$fPath."\n";
     
    my $str;
    
    local $/=undef;  
    open (IN,"<$fPath") or return "";
    $str=<IN>;
    close IN;
    
    ($str) = ($str =~ /DESCRIPTION\s*([\S|\s]+)USAGE/);
    $str =~ s/\n/ /g; 
    
    return $str;
}  

#--------------------------------------------------------
# 
#--------------------------------------------------------

sub getExample {
    
    my ($name) = @_;
    
    my $fPath=examplePath($name);    
    
    unless($fPath) {
        return "";
    }   
    
    my $str;
   
    local $/=undef;  
    open (IN,"<$fPath") or return "";
    $str=<IN>;
    close IN;
    
    my @v=split(/-#/,$str);
    
    $str="<h2>".$name." examples</h2>";
    $str=$str."<ol>";
    foreach my $block (@v) {
        print "block=".$block."\n";
        
        #If there is text
        if($block =~ /\S+/) {
        
            $block =~ s/\\n//g;
            $block =~ s/\n>/\n> /g;
            $block =~ s/&&/&amp;&amp;/g;
            
            my $blockMod;
            my $prevLine;
            my $inList=0;
            foreach my $line (split(/\n/,$block)) {
                    
                if($line =~ /\\verbatim/) {
                    $line =~ s/\\verbatim/<ac:structured-macro ac:name="code"><ac:plain-text-body><!\[CDATA\[/g;  
                } elsif($line =~ /\\endverbatim/)  {
                    $line =~ s/\\endverbatim/\]\]><\/ac:plain-text-body><\/ac:structured-macro>/g;                    
                } elsif($line =~ /\\code/)  {    
                    $line =~ s/\\code/<ac:structured-macro ac:name="code"><ac:plain-text-body><!\[CDATA\[/g;
                } elsif($line =~ /\\endcode/)  {    
                    $line =~ s/\\endcode/\]\]><\/ac:plain-text-body><\/ac:structured-macro>/g;
                } elsif($line =~ /^\s*- /) {
                    $line =~ s/^\s*- /<li>/;
                    
                    if($inList ==  1) {
                        $line="</li>".$line;
                    } else {
                         $line="<ul>".$line;
                    }     
                         
                    $inList=1;
                }
                
                if($line =~ /^\s*\.\s*/ and $inList==1) {
                        $line="</li></ul>";
                        $inList=0;
                }       
                
                $prevLine=$line;
                
                $blockMod=$blockMod.$line."\n";    
            }        
          
            $str=$str."<li><p>".$blockMod."</p></li><br />";
        }

    }

    $str=$str."</ol>";


    return $str;
}    
