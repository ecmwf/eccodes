#!/bin/sh
set -e
dir=$PWD
toolsDir=../tools/
dataDir=../data/
cd $dataDir

echo "-# The default behaviour for grib_compare without any option is to perform a bit by bit comparison of the two messages. If the messages 
are found to be bitwise different then grib_compare switches to a \"key based\" mode to find out which coded keys are different.
 To see how grib_compare works we first set the shortName=2d (2 metre dew point temperature) in the file 
 regular_latlon_surface.grib1\\n
\\verbatim 
>grib_set -s shortName=2d regular_latlon_surface.grib1 2d.grib1
\\endverbatim"

rm -f 2d.grib1 | true
$toolsDir/grib_set -s shortName=2d regular_latlon_surface.grib1 2d.grib1
echo "Then we can compare the two fields with grib_compare.\\n"
echo "\\verbatim 
>grib_compare regular_latlon_surface.grib1 2d.grib1"
set +e
$toolsDir/grib_compare regular_latlon_surface.grib1 2d.grib1
set -e
echo "\\endverbatim
In the output we see that the only \"coded\" key with different values in the two messages is indicatorOfParameter which is the
relevant key for the parameter information.
The comparison can be forced to be successful listing the keys with different values in the -b option."

echo "\\verbatim 
>grib_compare -b indicatorOfParameter regular_latlon_surface.grib1 2d.grib1"
set +e
$toolsDir/grib_compare -b indicatorOfParameter regular_latlon_surface.grib1 2d.grib1
set -e
echo "\\endverbatim
\n\n"

echo "-# Two grib messages can be very different because they have different edition, but they can contain the same
identical information in the header and the same data. To see how grib_compare can help in comparing messages with different edition we 
do 
\\verbatim 
>grib_set edition=2 reduced_gaussian_model_level.grib1 reduced_gaussian_model_level.grib2
\\endverbatim"

rm -f regular_latlon_surface.grib2 | true
$toolsDir/grib_set -s edition=2 reduced_gaussian_model_level.grib1 reduced_gaussian_model_level.grib2

echo "Then we compare the two fields with grib_compare.\\n"
echo "\\verbatim 
>grib_compare reduced_gaussian_model_level.grib1 reduced_gaussian_model_level.grib2"
set +e
$toolsDir/grib_compare reduced_gaussian_model_level.grib1 reduced_gaussian_model_level.grib2
set -e

echo "\\endverbatim
It is clear that the two messages are coded in a very different way. If we now add the -e option, the tool will compare only the 
higher level information common between the two messages."

echo "\\verbatim 
>grib_compare -e reduced_gaussian_model_level.grib1 reduced_gaussian_model_level.grib2"
set +e
$toolsDir/grib_compare -e reduced_gaussian_model_level.grib1 reduced_gaussian_model_level.grib2
set -e

echo "\\endverbatim
The comparison is successful because the two messages contain the same information coded in two different ways. 
We can display the list of keys used by grib_compare adding the option -v (verbose)."


echo "\\verbatim 
>grib_compare -ve reduced_gaussian_model_level.grib1 reduced_gaussian_model_level.grib2"
set +e
$toolsDir/grib_compare -ve reduced_gaussian_model_level.grib1 reduced_gaussian_model_level.grib2
set -e

echo "\\endverbatim
For each key the type used in the comparison is reported and for the floating point keys also the tolerance used
is printed.
\n\n"

echo "-# Some options are provided to compare only a set of keys in the messages. The option -H is used to compare only the headers 
coded in the message, it doesn't compare the data values. 
The option \"-c key1:[i|d|s|n],key2:[i|d|s|n],... \" can be used to 
compare a set of keys or namespaces. The letter after the colon is optional and it is used to force the
type used in the comparison which is otherwise assumed to be the native type of the key. 
The possible types are:
 - :i ->  integer
 - :d ->  floating point  (C type double)
 - :s ->  string
 - :n ->  namespace.
 .
When the type \"n\" is used all the set of keys belonging to the specified namespace
are compared assuming their own native type.
To illustrate how these options work we change the values coded in 
a message using grib_filter with the following rules file (see grib_filter). "
echo "\\verbatim
set bitsPerValue=10;
set values={1,2.5,3,4,5,6,70};
write \"first.grib1\";
set values={1,2.5,5,4,5,6,70};
write \"second.grib1\";
\\endverbatim"

cat > x.rules <<EOF
set bitsPerValue=10;
set values={1,2.5,3,4,5,6,70};
write "first.grib1";
set values={1,2.5,5,4,5,6,70};
write "second.grib1";
EOF
$toolsDir/grib_filter x.rules reduced_gaussian_model_level.grib1
rm -f x.rules
echo "We first compare the two files using the -H option (only headers are compared)."

echo "\\verbatim 
>grib_compare -H first.grib1 second.grib1"
set +e
$toolsDir/grib_compare -H first.grib1 second.grib1
set -e

echo "\\endverbatim
The comparison is successful because the data are not compared.
To compare only the data we have to compare the \"data namespace\"."
echo "\\verbatim 
>grib_compare -c data:n first.grib1 second.grib1"
set +e
$toolsDir/grib_compare -c data:n first.grib1 second.grib1
set -e

echo "\\endverbatim
The comparison is showing that one of seven values is different in a comparison 
with the (default) absolute tolerance=0. We can change the tolerance with the -A option:"

echo "\\verbatim 
>grib_compare -A 2 -c data:n first.grib1 second.grib1"
set +e
$toolsDir/grib_compare -A 2 -c data:n first.grib1 second.grib1
set -e

echo "\\endverbatim
and we see that the comparison is successful if the absolute tolerance is set to 2.
We can also set the relative tolerance for each key with the option -R:"
echo "\\verbatim 
>grib_compare -R packedValues=0.4 -c data:n first.grib1 second.grib1"
set +e
$toolsDir/grib_compare -R packedValues=0.4 -c data:n first.grib1 second.grib1
set -e

echo "\\endverbatim
and we get again a successful comparison because the relative tolerance is bigger than the
relative absolute difference of two corresponding values.
Another possible choice for the tolerance is to be equal to the packingError,
which is the error due to the packing algorithm. If we change the decimalPrecision of 
a packed field we introduce a packing error sometimes bigger than the original
packing error."

echo "\\verbatim 
>grib_set -s changeDecimalPrecision=0 first.grib1 third.grib1"
set +e
$toolsDir/grib_set -s changeDecimalPrecision=0 first.grib1 third.grib1
set -e

echo "\\endverbatim
and we compare the two fields using the -P option (tolerance=packingError)."
echo "\\verbatim 
>grib_compare -P -c data:n first.grib1 third.grib1"
set +e
$toolsDir/grib_compare -P -c data:n first.grib1 third.grib1
set -e

echo "\\endverbatim
the comparison is successful because their difference is within the biggest of
the two packing error. With the option -P the comparison is failing only 
if the original data coded are different, not if the packing precision is
changed. If we try again to compare the fields without the -P option:"
echo "\\verbatim 
>grib_compare -c data:n first.grib1 third.grib1"
set +e
$toolsDir/grib_compare -c data:n first.grib1 third.grib1
set -e

echo "\\endverbatim
we see that some values are different and that the maximum absolute differenc
is close to the biggest packing error (max diff=0.48 packingError=0.5).
The packing error was chosen to be 0.5 by setting decimalPrecision to 0 which
means that we don't need to preserve any decimal figure.\n\n"

echo "-# When we already know that the fields are not numerically identical, but have
similar statistical characteristics we can compare their statistics namespaces:"

echo "\\verbatim 
>grib_compare -c statistics:n first.grib1 third.grib1"
set +e
$toolsDir/grib_compare -c statistics:n first.grib1 third.grib1
set -e

echo "\\endverbatim
and we see that maximum, minimum, average, standard deviation, skewness and kurtosis are compared.
While the values are different by 0.48 the statistics comparison shows that the difference in
the statistical values is never bigger than 0.052"

echo "\\verbatim 
>grib_compare -A 0.052 -c statistics:n first.grib1 third.grib1"
set +e
$toolsDir/grib_compare -A 0.052 -c statistics:n first.grib1 third.grib1
set -e

echo "\\endverbatim
The statistics namespace is available also for spherical harmonics data and provides
information about the field in the geographic space computing them in the spectral space for 
performance reasons.
\n\n"

echo "-# When a file contains several fields and some keys are different, it is useful to have a summary
report of the keys found different in the messages. This can be obtained with the option -f.
We change few keys in a file:"

echo "\\verbatim 
>grib_set -w typeOfLevel=surface -s step=48 tigge_pf_ecmwf.grib2 out.grib2"
set +e
$toolsDir/grib_set -w typeOfLevel=surface -s step=48 tigge_pf_ecmwf.grib2 out.grib2
set -e

echo "\\endverbatim
and comparing with the -f option:"

echo "\\verbatim 
>grib_compare -f tigge_pf_ecmwf.grib2 out.grib2"
set +e
$toolsDir/grib_compare -f tigge_pf_ecmwf.grib2 out.grib2 | head -11
echo "...  output deleted "
$toolsDir/grib_compare -f tigge_pf_ecmwf.grib2 out.grib2 | tail -10
set -e

echo "\\endverbatim
we get a list of all the different messages in the files and a summary report
of the different keys.
\n\n"
rm -f out.grib2

echo "-# We can change the order of the messages in a file using grib_copy with the
-B option:"

echo "\\verbatim 
>grib_copy -B typeOfLevel tigge_pf_ecmwf.grib2 out.grib2"
set +e
$toolsDir/grib_copy -B typeOfLevel tigge_pf_ecmwf.grib2 out.grib2
set -e

echo "\\endverbatim
If we now compare the two files:"

echo "\\verbatim 
>grib_compare -f tigge_pf_ecmwf.grib2 out.grib2"
set +e
$toolsDir/grib_compare -f tigge_pf_ecmwf.grib2 out.grib2 | head -20
echo "...    very long output "
set -e

echo "\\endverbatim
the comparison is failing because of the different order of the messages. 
We can use the -r option to compare the files assuming that the messages are not in the
same order:"

echo "\\verbatim 
>grib_compare -r tigge_pf_ecmwf.grib2 out.grib2"
set +e
$toolsDir/grib_compare -r tigge_pf_ecmwf.grib2 out.grib2
set -e

echo "\\endverbatim
and we have a successful comparison because for each message in the first file an identical 
message is found in the second file. This option should be used carefully as it is
very time expensive.

\n\n"

rm -f first.grib1 second.grib1 third.grib1 out.grib2

cd $dir

