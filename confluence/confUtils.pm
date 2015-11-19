package confUtils;

use strict;

#The confluence space. Can be overridden with the -c switch!! See the main script.
our $confSpace="ECC";

my $CONF=$ENV{CONF}; 
die "Env var CONF should point to confluence.sh script" if ( $CONF eq "" || ! -f "$CONF" );

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
    
    my $cmd="$CONF -a addPage --noConvert --replace --space \"$confSpace\" --title \"$title\" --parent \"$parentPage\" --file $fHtml";
    
    print "\t\tUpload to conflunce with command:\n\t$cmd\n";   
    system($cmd);
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

sub makeCode {
    
    my ($text) = @_;

    return "<ac:structured-macro ac:name=\"code\"><ac:plain-text-body><![CDATA[".$text."]]></ac:plain-text-body></ac:structured-macro>";
}

1;
