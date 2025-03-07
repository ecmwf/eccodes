ecCodes
=======


<h3 align="center">
  <a href="https://github.com/ecmwf/codex/raw/refs/heads/main/ESEE"><img src="https://github.com/ecmwf/codex/raw/refs/heads/main/ESEE/foundation_badge.svg" alt="Static Badge"></a>
  <a href="https://github.com/ecmwf/codex/raw/refs/heads/main/Project%20Maturity"><img src="https://github.com/ecmwf/codex/raw/refs/heads/main/Project%20Maturity/graduated_badge.svg" alt="Static Badge"></a>
  <a href="https://codecov.io/gh/ecmwf/eccodes"><img src="https://codecov.io/gh/ecmwf/eccodes/branch/develop/graph/badge.svg" alt="codecov"></a>
  <a href="https://anaconda.org/conda-forge/eccodes"><img src="https://img.shields.io/conda/vn/conda-forge/eccodes?label=Anaconda.org"></a>
  <br>
  <a href="https://github.com/ecmwf/eccodes/actions/workflows/ci.yml?query=branch%3Amaster"><img src="https://img.shields.io/github/actions/workflow/status/ecmwf/eccodes/ci.yml?branch=master&label=Linux%20%26%20MacOS%3A%20master" alt="Linux & macOS: master"></a>
  <a href="https://ci.appveyor.com/project/ecmwf/eccodes/branch/master"><img src="https://img.shields.io/appveyor/ci/ecmwf/eccodes/master.svg?label=Windows%3A%20master" alt="Windows: master"></a>

  <a href="https://github.com/ecmwf/eccodes/actions/workflows/ci.yml?query=branch%3Adevelop"><img src="https://img.shields.io/github/actions/workflow/status/ecmwf/eccodes/ci.yml?branch=develop&label=Linux%20%26%20MacOS%3A%20develop" alt="Linux & macOS: develop"></a>
  <a href="https://ci.appveyor.com/project/ecmwf/eccodes/branch/develop"><img src="https://img.shields.io/appveyor/ci/ecmwf/eccodes/develop.svg?label=Windows%3A%20develop" alt="Windows: develop"></a>
</h3>


> [!IMPORTANT]
> This software is **Graduated** and subject to ECMWF's guidelines on [Software Maturity](https://github.com/ecmwf/codex/raw/refs/heads/main/Project%20Maturity).

ecCodes is a package developed by ECMWF which provides an application programming interface
and a set of tools for decoding and encoding messages in the following formats:

   * WMO FM-92 GRIB edition 1 and edition 2
   * WMO FM-94 BUFR edition 3 and edition 4
   * WMO GTS abbreviated header (only decoding)

A useful set of command line tools provide quick access to the messages.
C, Fortran 90 and Python interfaces provide access to the main ecCodes functionality.

ecCodes is an evolution of GRIB API.
It is designed to provide the user with a simple set of functions to access data from
several formats with a key/value approach.

Documentation can be found here:
   https://confluence.ecmwf.int/display/ECC/ecCodes+Home

INSTALLATION
------------

1. Download ecCodes from https://confluence.ecmwf.int/display/ECC/Releases

2. Unpack distribution:
   ```
   tar -xzf eccodes-x.y.z-Source.tar.gz
   ```

3. Create a separate directory to build ecCodes:
   ```
   mkdir build
   cd build
   ```

4. Run cmake pointing to the source and specify the installation location:
   ```
   cmake  ../eccodes-x.y.z-Source -DCMAKE_INSTALL_PREFIX=/path/to/where/you/install/eccodes
   ```

   It is strongly recommended that you install into a clean directory

5. Compile, test and install:
   ```
   make
   ctest
   make install
   ```

To add the Python3 bindings, use pip3 install from PyPI as follows:
   ```
   pip3 install eccodes
   ```

For more details, please see:
https://confluence.ecmwf.int/display/ECC/ecCodes+installation

If you encounter any problems please visit our Support Portal:

   https://support.ecmwf.int



COPYRIGHT AND LICENSE
----------------------

(C) Copyright 2005- ECMWF.

This software is licensed under the terms of the Apache Licence Version 2.0
which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

