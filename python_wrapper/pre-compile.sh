#!/bin/bash

# the procedure for adding a new ext dependency to be bundled in here:
# - add git checkout, compile, etc
# - ensure the version ends up in python_wrapper/src/versions.txt
# - ensure the licence ends up in python_wrapper/src/copying/, and fname is referenced in copying/list.json
# - ensure the .so ends up in target/lib64/ with the expected libname
# - validate that the resulting wheel contains all the above
# additionally, make sure this script is aligned with /buildscripts/compile.sh and /buildscripts/wheel-linux.sh,
# in particular when it comes to install targets and package data, etc

mkdir -p python_wrapper/src/copying
mkdir -p /tmp/eccodes/target/eccodes/lib64/

## yum-available prereqs
for p in libaec-devel libpng-devel gobject-introspection-devel
do
    yum install -y $p
    # TODO improve
    yum install $p 2>&1 > tmp
    cat tmp
    v=$(grep 'already installed' < tmp | awk '{print $2;}' | sed 's/\\d://')
    echo "yum $p $v" >> python_wrapper/src/versions.txt
done
rm tmp


## buildable prereqs
GIT_OPENJPEG=https://github.com/uclouvain/openjpeg
OPENJPEG_VERSION=v2.5.2

git clone --branch $OPENJPEG_VERSION --depth=1 $GIT_OPENJPEG /src/openjpeg

mkdir -p /tmp/openjpeg/build 
cd /tmp/openjpeg/build
cmake /src/openjpeg/ -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/tmp/openjpeg/target
cmake --build . --target install

cd -
cd /src/openjpeg && echo "$(git remote -v | head -1 | awk '{print $2;}') $(git rev-parse HEAD)" > /tmp/openjpeg/version.txt
cd -
cat /tmp/openjpeg/version.txt >> python_wrapper/src/versions.txt

## licenses
wget https://raw.githubusercontent.com/MathisRosenhauer/libaec/master/LICENSE.txt -O python_wrapper/src/copying/libaec.txt
wget https://raw.githubusercontent.com/uclouvain/openjpeg/master/LICENSE -O python_wrapper/src/copying/libopenjpeg.txt
wget https://raw.githubusercontent.com/glennrp/libpng/libpng16/LICENSE -O python_wrapper/src/copying/libpng.txt
cp LICENSE python_wrapper/src/copying/libeccodes.txt
echo '{"libeccodes": {"path": "copying/libeccodes.txt", "home": "https://github.com/ecmwf/eccodes"}, "libaec": {"path": "copying/libaec.txt", "home": "https://github.com/MathisRosenhauer/libaec"}, "home": "https://github.com/uclouvain/openjpeg"}, "libpng": {"path": "copying/libpng.txt", "home": "https://github.com/glennrp/libpng"}}' > python_wrapper/src/copying/list.json

echo "$(git remote -v | head -1 | awk '{print $2;}') $(git rev-parse HEAD)" >> python_wrapper/src/versions.txt

cp /tmp/openjpeg/target/lib64/libopenjp2.so.7 /tmp/eccodes/target/eccodes/lib64/libopenjp2.so.7
cp /lib64/libaec.so.0 /tmp/eccodes/target/eccodes/lib64/libaec.so.0
cp /lib64/libpng16.so.16 /tmp/eccodes/target/eccodes/lib64/libpng16.so.16

