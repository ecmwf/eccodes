#!/bin/sh
set -e 

echo "-# The bufr_filter sequentially processes all bufr messages contained in the input files and applies the rules to each of them. \\n"

echo " Input messages can be written to the output by using the \"write\" statement. The write statement can be parameterised so that output "
echo " is sent to multiple files depending on key values used in the output file name. \\n"
echo " First we write a rules_file containing the following statement:\\n \\n"
echo "\\verbatim"
echo "write \"../data/split/[bufrHeaderCentre:i]_[dataCategory].bufr[editionNumber]\";"
echo "\\endverbatim\\n"
echo "Then we create the BUFR file multitype.bufr from three others: \\n"
echo "\\verbatim"
echo ">mkdir ../data/split \\n"
echo ">cat ../data/bufr/syno_1.bufr ../data/bufr/goes_87.bufr ../data/bufr/gosat.bufr > ../data/split/multitype.bufr \\n"
echo "\\endverbatim\\n"
echo "Applying this rules_file to the \"../data/split/multitype.bufr\" bufr file we obtain several files in the ../data/split directory containing "
echo " messages split according to their key values\\n "
echo "\\verbatim"

if [[ -d ../data/split ]] 
then
    rm -f ../data/split/* 
else
    mkdir ../data/split
fi

cat ../data/bufr/syno_1.bufr ../data/bufr/goes_87.bufr ../data/bufr/gosat.bufr > ../data/split/multitype.bufr


cat > rules_file <<EOF
write "../data/split/[bufrHeaderCentre:i]_[dataCategory].bufr[editionNumber]";
EOF

echo ">bufr_filter rules_file ../data/split/multitype.bufr"
echo ">ls ../data/split"

./bufr_filter rules_file ../data/split/multitype.bufr
ls ../data/split

echo "\\endverbatim\\n"

echo "-# The bufr <b>header</b> information can be accessed without unpacking the data. This rules_file: \\n"
echo "\\verbatim"
echo "print \"[bufrHeaderCentre] [bufrHeaderSubCentre] [masterTablesVersionNumber] [localTablesVersionNumber] [numberOfSubsets]\";"
echo "\\endverbatim\\n"
echo "will result in the following output: \\n"

echo "\\verbatim"
cat > rules_file <<EOF
print "[bufrHeaderCentre] [bufrHeaderSubCentre] [masterTablesVersionNumber] [localTablesVersionNumber] [numberOfSubsets]";
EOF

echo ">bufr_filter rules_file ../data/bufr/syno_multi.bufr"

./bufr_filter rules_file ../data/bufr/syno_multi.bufr

echo "\\endverbatim\\n"

# Unpack 

echo "-# To print values from the data section the messages have to be <b>unpacked</b>. To do that we need to set key <i>unapack</i> to 1. This rules_file: \\n"

echo "\\verbatim"
echo "set unpack=1;"
echo "print \"block=[blockNumber] station=[stationNumber] lat=[latitude] lon=[longitude] t2=[airTemperatureAt2M]\";"
echo "\\endverbatim\\n"

echo "will print out some data values from the specified SYNOP bufr messages."

echo "\\verbatim"
cat > rules_file <<EOF
set unpack=1;
print "block=[blockNumber] station=[stationNumber] lat=[latitude] lon=[longitude] t2=[airTemperatureAt2M]";
EOF

echo ">bufr_filter rules_file ../data/bufr/syno_multi.bufr"

./bufr_filter rules_file ../data/bufr/syno_multi.bufr

echo "\\endverbatim\\n"

# Tranzient keys

echo "-# bufr_filter allows defining new keys with the <b>transient</b> keyword. \\n"
echo " We will further develop the previous example by creating a new key to combine the block number "
echo " and the station number into the full WMO station id: \\n"

echo "\\verbatim"
echo "set unpack=1;"
echo "transient statid=1000*blockNumber+stationNumber;"
echo "print \"statid=[statid] lat=[latitude] lon=[longitude] t2=[airTemperatureAt2M]\";"
echo "\\endverbatim\\n"

echo "The result is:"

echo "\\verbatim"
cat > rules_file <<EOF
set unpack=1;
transient statid=1000*blockNumber+stationNumber;
print "statid=[statid] lat=[latitude] lon=[longitude] t2=[airTemperatureAt2M]";
EOF

echo ">bufr_filter rules_file ../data/bufr/syno_multi.bufr"

./bufr_filter rules_file ../data/bufr/syno_multi.bufr

echo "\\endverbatim\\n"
 
#Conditional statements

echo "-# We can use <b>conditional statements</b> in bufr_filter. The syntax is: \\n"
echo "\\verbatim"
echo "if ( condition ) { block of rules } else { block of rules }"
echo "\\endverbatim\\n"
echo "The condition can be made using ==,!= and joining single block conditions with || and &&. \\n"
echo "The statement can be any valid statement also another nested condition\\n"
echo "The rules_file below shows how to filter only SYNOP messages with a specific station id:"

echo "\\verbatim"
echo "set unpack=1;"
echo "transient statid=1000*blockNumber+stationNumber;"
echo ""
echo "if (dataCategory ==0 && statid == 1003) {"
echo "  write out.bufr;"
echo "}"        
echo "\\endverbatim\\n"

#Switch

echo "-# The <b>switch</b> statement is an enhanced version of the if statement. Its syntax is the following:"
echo "\\verbatim"
echo "switch (key1,key2,...,keyn) {"
echo "    case val11,val12,...,val1n:"
echo "        # block of rules;"
echo "    case val21,val22,...,val2n:"
echo "        # block of rules;"
echo "    default:"
echo "        # [ block of rules ]"
echo "}"
echo "\\endverbatim\\n"
echo "Each value of each key given as argument to the switch statement is matched against the values specified in the case statements.\\n"
echo "If there is a match, then the block or rules corresponding to the matching case statement is executed.\\n"
echo "Otherwise, the default case is executed. The default case is mandatory if the case statements do not cover all the possibilities.\\n"
echo "The \"~\" operator can be used to match \"anything\".\\n\\n"


# Attributes 
 
echo "-# To access the keys' <b>attributes</b> use the -> operator. \\n"
echo " The example below prints the attributes of key <i>nonCoordinatePressure</i> from a SYNOP bufr message. \\n"

echo "\\verbatim"
echo "print \"nonCoordinatePressure=[nonCoordinatePressure] [nonCoordinatePressure->units]\";"
echo "print \"nonCoordinatePressure->code=[nonCoordinatePressure->code!06d]\";"
echo "print \"nonCoordinatePressure->scale=[nonCoordinatePressure->scale]\";"
echo "print \"nonCoordinatePressure->reference=[nonCoordinatePressure->reference]\";"
echo "print \"nonCoordinatePressure->width=[nonCoordinatePressure->width]\";"
echo "print \"nonCoordinatePressure->percentConfidence=[nonCoordinatePressure->percentConfidence] [nonCoordinatePressure->percentConfidence->units]\";"
echo "print \"nonCoordinatePressure->percentConfidence->code=[nonCoordinatePressure->percentConfidence->code!06d]\";"
echo "print \"nonCoordinatePressure->percentConfidence->scale=[nonCoordinatePressure->percentConfidence->scale]\";"
echo "print \"nonCoordinatePressure->percentConfidence->reference=[nonCoordinatePressure->percentConfidence->reference]\";"
echo "print \"nonCoordinatePressure->percentConfidence->width=[nonCoordinatePressure->percentConfidence->width]\";"
echo "\\endverbatim\\n"

echo "The result is:"

echo "\\verbatim"
cat > rules_file <<EOF
set unpack=1;
print "nonCoordinatePressure=[nonCoordinatePressure] [nonCoordinatePressure->units]";
print "nonCoordinatePressure->code=[nonCoordinatePressure->code!06d]";
print "nonCoordinatePressure->scale=[nonCoordinatePressure->scale]";
print "nonCoordinatePressure->reference=[nonCoordinatePressure->reference]";
print "nonCoordinatePressure->width=[nonCoordinatePressure->width]";
print "nonCoordinatePressure->percentConfidence=[nonCoordinatePressure->percentConfidence] [nonCoordinatePressure->percentConfidence->units]";
print "nonCoordinatePressure->percentConfidence->code=[nonCoordinatePressure->percentConfidence->code!06d]";
print "nonCoordinatePressure->percentConfidence->scale=[nonCoordinatePressure->percentConfidence->scale]";
print "nonCoordinatePressure->percentConfidence->reference=[nonCoordinatePressure->percentConfidence->reference]";
print "nonCoordinatePressure->percentConfidence->width=[nonCoordinatePressure->percentConfidence->width]";
EOF

echo ">bufr_filter rules_file ../data/bufr/syno_1.bufr"

./bufr_filter rules_file ../data/bufr/syno_1.bufr

echo "\\endverbatim\\n"

# Access by rank

echo "-# To <u>access keys by <b>rank</b></u> (i.e. by their occurrence in the message) use the # operator. \\n"
echo " The example below prints the value from the 4th occurrence of key <i>pressure</i> from a TEMP bufr message. "
echo " As a reference, we also print all the pressure values found in the message.\\n"

echo "\\verbatim"
echo "set unpack=1;"
echo "print \"pressure=[#4#pressure] [#4#pressure->units]\";"
echo "print \"pressure=[pressure]\";"
echo "\\endverbatim\\n"

echo "The result is:"

echo "\\verbatim"
cat > rules_file <<EOF
set unpack=1;
print "pressure=[#4#pressure] [#4#pressure->units]";
print "pressure=[pressure]";
EOF

echo ">bufr_filter rules_file ../data/bufr/temp_101.bufr"

./bufr_filter rules_file ../data/bufr/temp_101.bufr

echo "\\endverbatim\\n"

# Access by condition

echo "-# It is possible to <u>access elements by <b>conditions</b></u> imposed on coordinate descriptors. \\n"
echo " The example below prints the temperature values on temperature significant levels from a TEMP bufr message."
echo " For temperature significant levels the key <i>verticalSoundingSignificance</i>=4 and this"
echo " is what we use in the condition:  \\n"

echo "\\verbatim"
echo "set unpack=1;"
echo "print \"[/verticalSoundingSignificance=4/airTemperature]\";"
echo "\\endverbatim\\n"

echo "The result is:"

echo "\\verbatim"

cat > rules_file <<EOF
set unpack=1;
print "[/verticalSoundingSignificance=4/airTemperature]";
EOF

echo ">bufr_filter rules_file ../data/bufr/temp_101.bufr"

./bufr_filter rules_file ../data/bufr/temp_101.bufr

echo "\\endverbatim\\n"


# Access by condition 2

echo "-# Another example for accessing keys by condition is to read scatterometer data."
echo "File asca_139.bufr contains a single message with 2016 subsets in a compressed form."
echo "In this case each subset has exactly the same structure: they store one location with"
echo "several beams and one backscatter value in each beam. To print the backScatter values for beamIdentifier=2 from all the subsets" 
echo "we can simply define the condition like this: \\n"

echo "\\verbatim"
echo "set unpack=1;"
echo "print \"/beamIdentifier=2/backscatter=[/beamIdentifier=2/backscatter]\";"
echo "\\endverbatim\\n"

echo "The result is:"
echo "\\verbatim"

cat > rules_file <<EOF
set unpack=1;
print "/beamIdentifier=2/backscatter=[/beamIdentifier=2/backscatter]";
EOF

echo ">bufr_filter rules_file ../data/bufr/asca_139.bufr"

./bufr_filter rules_file ../data/bufr/asca_139.bufr > tmp_file
head tmp_file
echo " and many more values ......"

echo "\\endverbatim\\n"

# Accessing subsets

echo "-# Accessing keys by condition provides the means to read individual <b>subsets</b> from uncompressed data."
echo "File synop_multi_subset.bufr contains a single message with several subsets in an uncompressed form."
echo "To access a given subset we can simply use key <b>subsetNumber</b> in the filter condition. The example below shows" 
echo "how to read the blockNumber, stationNumber and airTemperaturefor the first 3 subsets in the message: \\n"

echo "\\verbatim"
echo "set unpack=1;"
echo "print \"subsetNumber=1 blockNumber=[/subsetNumber=1/blockNumber] stationNumber=[/subsetNumber=1/stationNumber] airTempearture=[/subsetNumber=1/airTemperature]\";"
echo "print \"subsetNumber=2 blockNumber=[/subsetNumber=2/blockNumber] stationNumber=[/subsetNumber=2/stationNumber] airTempearture=[/subsetNumber=2/airTemperature]\";"
echo "print \"subsetNumber=3 blockNumber=[/subsetNumber=3/blockNumber] stationNumber=[/subsetNumber=3/stationNumber] airTempearture=[/subsetNumber=3/airTemperature]\";"
echo "\\endverbatim\\n"

echo "The result is:"
echo "\\verbatim"

cat > rules_file <<EOF
set unpack=1;
print "subsetNumber=1 blockNumber=[/subsetNumber=1/blockNumber] stationNumber=[/subsetNumber=1/stationNumber] airTempearture=[/subsetNumber=1/airTemperature]";
print "subsetNumber=2 blockNumber=[/subsetNumber=2/blockNumber] stationNumber=[/subsetNumber=2/stationNumber] airTempearture=[/subsetNumber=2/airTemperature]";
print "subsetNumber=3 blockNumber=[/subsetNumber=3/blockNumber] stationNumber=[/subsetNumber=3/stationNumber] airTempearture=[/subsetNumber=3/airTemperature]";
EOF

echo ">bufr_filter rules_file ../data/bufr/synop_multi_subset.bufr"

./bufr_filter rules_file ../data/bufr/synop_multi_subset.bufr > tmp_file
head tmp_file

echo "\\endverbatim\\n"

rm -f rules_file || true
rm -f tmp_file || true


