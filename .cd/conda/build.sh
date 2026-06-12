#!/usr/bin/env bash

set -e

export LDFLAGS="$LDFLAGS -L$PREFIX/lib -Wl,-rpath,$PREFIX/lib"
export CFLAGS="$CFLAGS -fPIC -I$PREFIX/include"

mkdir ../build && cd ../build

cmake -D CMAKE_INSTALL_PREFIX=$PREFIX \
      -D CMAKE_BUILD_TYPE=Release \
      -D INSTALL_LIB_DIR='lib' \
      -D ENABLE_JPG=1 \
      -D ENABLE_NETCDF=1 \
      -D ENABLE_PNG=1 \
      -D ENABLE_PYTHON=0 \
      -D ENABLE_FORTRAN=1 \
      -D ENABLE_ECCODES_THREADS=1 \
      -D ENABLE_AEC=1 \
      -D REPLACE_TPL_ABSOLUTE_PATHS=1 \
      -D CMAKE_FIND_ROOT_PATH=$PREFIX \
      -D CMAKE_FIND_ROOT_PATH_MODE_INCLUDE=BOTH \
      -D CMAKE_PROGRAM_PATH=$BUILD_PREFIX \
      $SRC_DIR

make -j $CPU_COUNT
make install
