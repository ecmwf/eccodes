#!/bin/sh

usage ()
{
   prog=`basename $0`
   echo "Usage: $prog [-v] [-c] data_dir"
   echo
   echo "-v  verbose"
   echo "-c  clean downloaded files"
   echo "-h  prints this help message"
   echo
}

VERBOSE=0
CLEAN=0
while :
do
   case "$1" in
      -h) usage ; exit 0;;
      -v) VERBOSE=1
         echo "Running with verbose setting"
         ;;
      -c) CLEAN=1
         echo "Cleaning downloaded files"
         ;;
      --) shift ; break ;;
      -*) usage ; exit 0;;
      *)  break;;
   esac
   shift
done

DATA_DIR=$1
if [ -z "$DATA_DIR" ]; then
   echo "Error: No directory specified." 2>&1
   usage
   exit 1
fi

grib_files=`cat $DATA_DIR/grib_data_files.txt`
tigge_files=`cat $DATA_DIR/tigge/tigge_data_files.txt | sed -e 's:^:tigge/:'`
bufr_files=`cat $DATA_DIR/bufr/bufr_data_files.txt $DATA_DIR/bufr/bufr_ref_files.txt | sed -e 's:^:bufr/:'`
metar_files=`cat $DATA_DIR/metar/metar_data_files.txt $DATA_DIR/metar/metar_ref_files.txt | sed -e 's:^:metar/:'`
gts_files=`cat $DATA_DIR/gts/gts_data_files.txt $DATA_DIR/gts/gts_ref_files.txt | sed -e 's:^:gts/:'`

files="$grib_files $tigge_files $bufr_files $metar_files $gts_files"

if [ $CLEAN -eq 1 ]; then
   for f in $files; do
      rm -f $f
      rm -f ".downloaded"
   done
   exit 0
fi

# Check if all downloads are already done
if [ -f "${DATA_DIR}/.downloaded" ]; then
   if [ $VERBOSE -eq 1 ]; then
      echo "All downloads are already done. Exiting."
   fi
   exit 0
fi

[ -d "${DATA_DIR}/tigge" ] || mkdir "${DATA_DIR}/tigge"

TIMEOUT_SECS=15

# Decide what tool to use to download data
DNLD_PROG=""
if command -v wget >/dev/null 2>&1; then
   PROG=wget
   OPTIONS="--tries=1 --timeout=$TIMEOUT_SECS -nv -q -O"
   if [ $VERBOSE -eq 1 ]; then
      OPTIONS="--tries=1 --timeout=$TIMEOUT_SECS -nv -O"
   fi
   DNLD_PROG="$PROG $OPTIONS"
fi
if command -v curl >/dev/null 2>&1; then
   PROG=curl
   OPTIONS="--connect-timeout $TIMEOUT_SECS --silent --show-error --fail --output"
   if [ $VERBOSE -eq 1 ]; then
      OPTIONS="--connect-timeout $TIMEOUT_SECS --show-error --fail --output"
   fi
   DNLD_PROG="$PROG $OPTIONS"
fi
if test "x$DNLD_PROG" = "x"; then
   echo "Cannot find tool to transfer data from download server. Aborting." 1>&2
   exit 1
fi

download_URL="http://download.ecmwf.org"
cd ${DATA_DIR}
echo "Checking data files for testing..."
downloading=0
for f in $files; do
   # If we haven't already got the file, download it
   if [ ! -f "$f" ]; then
      if [ $VERBOSE -eq 1 ]; then
         echo "$DNLD_PROG $f ${download_URL}/test-data/grib_api/data/$f"
      fi
      if [ $downloading = 0 ]; then
        echo "Downloading..."
        downloading=1
      fi
      $DNLD_PROG $f ${download_URL}/test-data/grib_api/data/$f
      if [ $? -ne 0 ]; then
          echo
          echo "Failed to download file \"$f\" from \"${download_URL}\"" 2>&1
          echo "Aborting" 2>&1
          exit 1
      fi

      if [ $VERBOSE -eq 1 ]; then
          echo "Downloaded $f ..."
      fi
   fi
done

# Add a file to indicate we've done the download
touch .downloaded
echo "Completed."
