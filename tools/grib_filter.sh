#!/bin/sh
set -e
echo "-# The grib_filter processes sequentially all grib messages contained in the input files and applies the rules to each one of them. \\n"
echo " Input messages can be written to the output by using the \"write\" statement. The write statement can be parameterised so that output "
echo " is sent to multiple files depending on key values used in the output file name. \\n"
echo " If we write a rules_file containing the only statement:\\n \\n"
echo "\\verbatim"
echo "write \"../data/split/[centre]_[date]_[dataType]_[levelType].grib[editionNumber]\";"
echo "\\endverbatim\\n"
echo "Applying this rules_file to the \"../data/tigge_pf_ecmwf.grib2\" grib file we obtain several files in the ../data/split directory containing "
echo " fields split according to their key values\\n "
echo "\\verbatim"

if [[ -d ../data/split ]] 
then
 rm -f ../data/split/* 
else
 mkdir ../data/split
fi

cat > rules_file <<EOF
write "../data/split/[centre]_[date]_[dataType]_[levelType].grib[editionNumber]";
EOF

echo ">grib_filter rules_file ../data/tigge_pf_ecmwf.grib2"
echo ">ls ../data/split"

./grib_filter rules_file ../data/tigge_pf_ecmwf.grib2
ls ../data/split

echo "\\endverbatim\\n"

echo "-# The key values in the file name can also be obtained in a different format by indicating explicitly the type required after a colon."
echo " - :i for integer"
echo " - :d for double"
echo " - :s for string"
echo " ."
echo "The following statement works in a slightly different way from the previous example, "
echo " including in the output file name the integer values for centre and dataType.\\n"

echo "\\verbatim"
echo "write \"../data/split/[centre:i]_[date]_[dataType:i]_[levelType].grib[editionNumber]\";"
echo "\\endverbatim\\n"
echo "Running the same command again we obtain a different list of files.\\n"
echo "\\verbatim"

if [[ -d ../data/split ]] 
then
 rm -f ../data/split/* 
else
 mkdir ../data/split
fi

cat > rules_file <<EOF
write "../data/split/[centre:i]_[date]_[dataType:i]_[levelType].grib[editionNumber]";
EOF

echo ">grib_filter rules_file ../data/tigge_pf_ecmwf.grib2"
echo ">ls ../data/split"

./grib_filter rules_file ../data/tigge_pf_ecmwf.grib2
ls ../data/split

echo "\\endverbatim\\n"

echo "-# Other statements are allowed in the grib_filter syntax:"
echo "  - if ( condition ) { block of rules } else { block of rules }\\n"
echo "    The condition can be made using ==,!= and joining single block conditions with || and && \\n"
echo "    The statement can be any valid statement also another nested condition\\n"
echo "  - set keyname = keyvalue;"
echo "  - print \"string to print also with key values like in the file name\""
echo "  - transient keyname1 = keyname2;"
echo "  - comments beginning with #"
echo "  - defined(keyname) to check if a key is defined in a message"
echo "  - missing(keyname) to check if the value of the key is set to MISSING"
echo "  - To set a key value to MISSING, use 'set key=MISSING;' (note the case)"
echo "  - You can also make an assertion with 'assert(condition)'. If condition is false, it will abort the filter."
echo "  ."
echo "A complex example of grib_filter rules is the following to change temperature in a grib edition 1 file." 
echo "\\verbatim"

echo "# Temperature"
echo "if ( level == 850 && indicatorOfParameter == 11 ) {"
echo "    print \"found indicatorOfParameter=[indicatorOfParameter] level=[level] date=[date]\";"
echo "    transient oldtype = type ;"
echo "    set identificationOfOriginatingGeneratingSubCentre=98;"
echo "    set gribTablesVersionNo = 128;"
echo "    set indicatorOfParameter = 130;"
echo "    set localDefinitionNumber=1;"
echo "    set marsClass=\"od\";"
echo "    set marsStream=\"kwbc\";"
echo "    # Negatively/Positively Perturbed Forecast"
echo "    if ( oldtype == 2 || oldtype == 3 ) {"
echo "      set marsType=\"pf\";"
echo "      set experimentVersionNumber=\"4001\";"
echo "    }"
echo "    # Control Forecast"
echo "    if ( oldtype == 1 ) {"
echo "      set marsType=\"cf\";"
echo "      set experimentVersionNumber=\"0001\";"
echo "    }"
echo "    set numberOfForecastsInEnsemble=11;"
echo "    write;"
echo "    print \"indicatorOfParameter=[indicatorOfParameter] level=[level] date=[date]\";"
echo "    print;"
echo "}"

echo "\\endverbatim\\n"

echo "-# Here is an example of an IF statement comparing a key with a string. "
echo "Note you have to use the \"is\" keyword for strings and not \"==\", and to negate you add the \"!\" before the whole condition:\\n \\n"
echo "\\verbatim"
echo "# Select Geopotential Height messages which are not on a Reduced Gaussian Grid"
echo "if (shortName is \"gh\" && !(gridType is \"reduced_gg\" )) {"
echo "    set step = 72;"
echo "}"
echo "\\endverbatim\\n"

echo "-# The switch statement is an enhanced version of the if statement. Its syntax is the following:"
echo "\\verbatim"
echo "switch (key1) {"
echo "    case val1:"
echo "        # block of rules;"
echo "    case val2:"
echo "        # block of rules;"
echo "    default:"
echo "        # block of rules"
echo "}"
echo "\\endverbatim\\n"
echo "Each value of each key given as argument to the switch statement is matched against the values specified in the case statements.\\n"
echo "If there is a match, then the block or rules corresponding to the matching case statement is executed.\\n"
echo "Otherwise, the default case is executed. The default case is mandatory if the case statements do not cover all the possibilities.\\n"
echo "The \"~\" operator can be used to match \"anything\".\\n\\n"
echo "Following is an example showing the use of the switch statement:\\n"
echo "\\verbatim"
echo "processing paramId=[paramId] [shortName] [stepType]";
echo "switch (shortName) {"
echo "    case "tp" :"
echo "        set stepType="accum";"
echo "    case "10u" :"
echo "        set typeOfLevel="surface";"
echo "    default:"
echo "}"
echo "\\endverbatim\\n"
