#!/bin/bash
# eccodes' HPC build recipe WITH the eckit-backed geography backend enabled.
#
# Identical to .ci/hpc/build.sh except for the two ENABLE_ flags below, which
# pull in the atos-hpc-gnu eckit artifact. This is the HPC counterpart of the
# runner `options = "eckit-geo"` leg, and the only configuration in which eckit
# is a dependency of eccodes at all — see the `when` predicate on the eckit
# [[deps]] entry in .ci/manifest.toml.
#
# ci-infrastructure wraps this file (it unpacks the transferred source into
# node-local $TMPDIR and cds there, exports $CMAKE_PREFIX_PATH /
# $CI_INSTALL_PREFIX, appends the sentinel), so this script owns only its #SBATCH
# resources, module loads and the build/test/install — and must NOT print
# "Finished: ..." itself.

# atos (hpc2020) selects on QoS rather than partition; ssdtmp sizes the
# node-local SSD behind $TMPDIR, which holds the unpacked source and the build.
#SBATCH --qos=nf
#SBATCH --gres=ssdtmp:20G
#SBATCH --time=00:40:00
#SBATCH --nodes=1
#SBATCH --ntasks=8

module load prgenv/gnu
module load cmake
module load ninja

# FEATURE FORTRAN is DEFAULT ON, so the Fortran compiler must be named
# explicitly — same reason as the runner leg, where the image ships only a
# versioned gfortran. ENABLE_AEC finds libaec in the stack-deps prefix.
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
