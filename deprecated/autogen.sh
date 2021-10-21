#!/bin/sh

autotools_dir=/usr/bin

export PATH=$autotools_dir:$PATH

autoreconf=$autotools_dir/autoreconf

echo -----------------------
$autoreconf --version
echo -----------------------
$autoreconf -i -f -Im4

