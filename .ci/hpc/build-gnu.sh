#!/bin/bash

#SBATCH --qos=nf
#SBATCH --gres=ssdtmp:20G
#SBATCH --time=01:30:00
#SBATCH --nodes=1
#SBATCH --ntasks=8

module load prgenv/gnu
module load cmake
module load ninja

cmake -S "$CI_SOURCE_DIR" -B "${TMPDIR:-/tmp}/build" \
  -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_Fortran_COMPILER=gfortran \
  -DENABLE_EXTRA_TESTS=1 \
  -DENABLE_ECCODES_OMP_THREADS=1 \
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
  -DCMAKE_INSTALL_PREFIX="$CI_INSTALL_PREFIX"
cmake --build "${TMPDIR:-/tmp}/build" --parallel "${SLURM_NTASKS:-8}"
ctest --test-dir "${TMPDIR:-/tmp}/build" --output-on-failure -j "${SLURM_NTASKS:-8}"

# Install to node-local SSD, then copy the tree across in one pass. eccodes
# installs ~24k small definition files; doing that as individual `cmake
# --install` operations straight onto shared scratch is metadata-bound and has
# taken anywhere from 7 to >35 minutes depending on filesystem load.
stage="${TMPDIR:-/tmp}/stage"
DESTDIR="$stage" cmake --install "${TMPDIR:-/tmp}/build"
# The fetcher takes the artifact from CI_INSTALL_ARCHIVE, not from the install
# tree; .part + mv so it only ever appears complete.
mkdir -p "$(dirname "$CI_INSTALL_ARCHIVE")"
tar -cf - -C "$stage$CI_INSTALL_PREFIX" . | zstd -T0 -q -o "$CI_INSTALL_ARCHIVE.part"
mv "$CI_INSTALL_ARCHIVE.part" "$CI_INSTALL_ARCHIVE"
