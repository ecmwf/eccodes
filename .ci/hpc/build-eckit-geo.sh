#!/bin/bash

#SBATCH --qos=nf
#SBATCH --gres=ssdtmp:20G
#SBATCH --time=00:40:00
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
  -DENABLE_GEOGRAPHY=1 \
  -DENABLE_ECKIT_GEO=1 \
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
  -DCMAKE_INSTALL_PREFIX="$CI_INSTALL_PREFIX"
cmake --build "${TMPDIR:-/tmp}/build" --parallel "${SLURM_NTASKS:-8}"
ctest --test-dir "${TMPDIR:-/tmp}/build" --output-on-failure -j "${SLURM_NTASKS:-8}"
cmake --install "${TMPDIR:-/tmp}/build"
