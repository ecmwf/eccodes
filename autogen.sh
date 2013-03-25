#!/bin/sh

autotools_dir=/usr/bin

export PATH=$autotools_dir:$PATH

autoreconf=$autotools_dir/autoreconf

goodversion=`$autoreconf --version | grep 2.65`
if [ -z "$goodversion" ]
then
echo
  echo "!!!!! ERROR: wrong autoreconf version"
  echo "We have to use autoconf 2.65"
  echo "ABORTED"
  echo
  exit 1
fi

echo -----------------------
$autoreconf --version
echo -----------------------
$autoreconf -i -f -Im4


