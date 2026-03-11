#!/bin/bash

# the procedure for adding a new ext dependency to be bundled in here:
# - add git checkout, compile, etc
# - ensure the version ends up in python_wrapper/src/versions.txt
# - ensure the licence ends up in python_wrapper/src/copying/, and fname is referenced in copying/list.json
# - ensure the .so ends up in target/lib64/ with the expected libname
# - validate that the resulting wheel contains all the above
# additionally, make sure this script is aligned with /buildscripts/compile.sh and /buildscripts/wheel-linux.sh,
# in particular when it comes to install targets and package data, etc

# note also that for macos, we assume that the agent has the libraries already installed, as we can't run this in docker
# we thus only prepare the license files

set -euo pipefail

mkdir -p python_wrapper/src/copying
mkdir -p /tmp/eccodes/target/eccodes/include/
mkdir -p /tmp/eccodes/target/eccodes/cmake/

if [ "$(uname)" != "Darwin" ] ; then
    echo "installing and copying deps for platform $(uname)"
    mkdir -p /tmp/eccodes/target/eccodes/lib64/

    ## yum-available prereqs -- assumed to be installed in the base due to privileges here
    for p in libpng-devel gobject-introspection-devel
    do
        v=$(dnf list --installed $p | tail -n 1 | sed 's/[ \t]\+/ /g' | cut -f 2 -d\ )
        echo "$p: yum $v" >> python_wrapper/src/versions.txt
    done

    ## buildable prereqs
    ### openjpg
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

    ### libaec
    # comes from cxx-deps

    # copy the libs, instead of having auditwheel done it later. This is a bit risky because cmake will later write in this
    # very directory... but it works
    # png
    cp /lib64/libpng16.so.16 /tmp/eccodes/target/eccodes/lib64/libpng16.so.16
    # openjpeg
    cp /tmp/openjpeg/target/lib64/libopenjp2.so.7 /tmp/eccodes/target/eccodes/lib64/libopenjp2.so.7

    # aec -- needs headers and cmake due to dependent on by eg gribjump
    cp /cxx-deps/lib64/libaec.so* /tmp/eccodes/target/eccodes/lib64
    cp /cxx-deps/include/libaec.h /tmp/eccodes/target/eccodes/include
    cp /cxx-deps/lib64/cmake/libaec/* /tmp/eccodes/target/eccodes/cmake


else
    echo "copying deps for platform $(uname)"
    mkdir -p /tmp/eccodes/target/eccodes/lib/

    BREWBASE="$(brew --cellar)" # "/opt/homebrew/Cellar" or "/usr/local/Cellar"
    LIBOPENJP="$(ls -d $BREWBASE/openjpeg/* | head -n1)"
    LIBPNG="$(ls -d $BREWBASE/libpng/* | head -n1)"

    for lib in $LIBOPENJP $LIBPNG ; do
        echo "will copy from $lib/lib with contents $(ls $lib/lib/*dylib)"
        cp $lib/lib/*dylib /tmp/eccodes/target/eccodes/lib
        echo "will copy from $lib/include with contents $(ls $lib/include)"
        cp -R $lib/include/* /tmp/eccodes/target/eccodes/include
    done


    # aec -- needs headers and cmake due to dependent on by eg gribjump
    cp /tmp/cxx-deps/lib/libaec*dylib /tmp/eccodes/target/eccodes/lib
    cp /tmp/cxx-deps/include/libaec.h /tmp/eccodes/target/eccodes/include
    cp /tmp/cxx-deps/lib/cmake/libaec/* /tmp/eccodes/target/eccodes/cmake

fi


echo "libaec v1.1.4" >> python_wrapper/src/versions.txt # TODO read from somewhere... there aint any VERSION file, needs to parse

echo "license setup"

## licenses
wget https://raw.githubusercontent.com/glennrp/libpng/libpng16/LICENSE -O python_wrapper/src/copying/libpng.txt
wget https://raw.githubusercontent.com/uclouvain/openjpeg/master/LICENSE -O python_wrapper/src/copying/libopenjpeg.txt
# TODO switch libaec license to the checked out by cxxdeps
wget https://raw.githubusercontent.com/MathisRosenhauer/libaec/master/LICENSE.txt -O python_wrapper/src/copying/libaec.txt
cp LICENSE python_wrapper/src/copying/libeccodes.txt
echo '{"libeccodes": {"path": "copying/libeccodes.txt", "home": "https://github.com/ecmwf/eccodes"}, "libaec": {"path": "copying/libaec.txt", "home": "https://github.com/MathisRosenhauer/libaec"}, "home": "https://github.com/uclouvain/openjpeg"}, "libpng": {"path": "copying/libpng.txt", "home": "https://github.com/glennrp/libpng"}}' > python_wrapper/src/copying/list.json

echo "$(git remote -v | head -1 | awk '{print $2;}') $(git rev-parse HEAD)" >> python_wrapper/src/versions.txt
