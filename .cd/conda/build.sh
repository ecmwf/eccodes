#!/usr/bin/env bash

set -e

export PYTHON=
export LDFLAGS="${LDFLAGS:-} -L$PREFIX/lib -Wl,-rpath,$PREFIX/lib"
export CFLAGS="${CFLAGS:-} -fPIC -I$PREFIX/include"

mkdir ../build && cd ../build

cmake -D CMAKE_INSTALL_PREFIX=$PREFIX \
      -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_FIND_FRAMEWORK=LAST \
      -D INSTALL_LIB_DIR='lib' \
      -D ENABLE_JPG=1 \
      -D ENABLE_NETCDF=1 \
      -D ENABLE_PNG=1 \
      -D ENABLE_PYTHON=0 \
      -D ENABLE_FORTRAN=1 \
      -D ENABLE_ECCODES_THREADS=1 \
      -D ENABLE_AEC=1 \
      -D ENABLE_EXTRA_TESTS=1 \
      -D ECBUILD_DOWNLOAD_TIMEOUT=60 \
      -D REPLACE_TPL_ABSOLUTE_PATHS=1 \
      -D CMAKE_FIND_ROOT_PATH=$PREFIX \
      -D CMAKE_FIND_ROOT_PATH_MODE_INCLUDE=BOTH \
      -D CMAKE_PROGRAM_PATH=$BUILD_PREFIX \
      $SRC_DIR

make -j ${CPU_COUNT:-1} VERBOSE=1

if [[ "${RUN_CONDA_BUILD_TESTS:-1}" == "1" ]]; then
  export ECCODES_TEST_VERBOSE_OUTPUT=1
  export LD_LIBRARY_PATH="$PREFIX/lib:${LD_LIBRARY_PATH:-}"

  if [[ "${CONDA_BUILD_CROSS_COMPILATION:-}" != "1" || -n "${CROSSCOMPILING_EMULATOR:-}" ]]; then
    ctest --output-on-failure -j ${CPU_COUNT:-1}
  else
    echo "Skipping ctest: cross-compilation build without CROSSCOMPILING_EMULATOR"
  fi
fi

make install
