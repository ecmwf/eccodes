.. SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Building the Rust Crates
========================

.. code-block:: bash

   cargo add eccodes
   cargo build

This builds ecCodes, eckit and libaec from source as part of ``cargo build``
(the ``vendored`` feature, on by default). It needs Rust 1.90 or later, C
and C++17 compilers, CMake and git. The first build also needs network
access, to download ecbuild and the releases matching the crate versions.
When the crate comes from a checkout of the eccodes repository, as a path or
git dependency, the C sources of that checkout are compiled instead.

To link an installed ecCodes, 2.34.0 or later, switch to the ``system``
feature and point CMake at the installations with ``ECCODES_DIR`` and
``ECKIT_DIR`` or ``CMAKE_PREFIX_PATH``:

.. code-block:: bash

   cargo add eccodes --no-default-features --features system
   ECCODES_DIR=/path/to/eccodes/install cargo build

The installed library must be built with ``ENABLE_ECCODES_THREADS=ON``.

Use ``system`` when the program must share one installation with other
software, for example on the HPC or in a bundle build and ``vendored`` for a
self-contained build.

The shared libraries of a vendored build are found at run time through
rpath entries on the binary. An application adds them from its build
script:

.. code-block:: toml

   [build-dependencies]
   bindman-utils = "0.2"

.. code-block:: rust

   // build.rs
   fn main() {
       bindman_utils::emit_rpaths();
   }

Running the Examples
--------------------

The programs quoted in :doc:`usage` are examples of the ``eccodes`` crate,
in ``rust/crates/eccodes/examples``. From a checkout of the repository:

.. code-block:: bash

   cd rust
   cargo run -p eccodes --example quickstart
   cargo run -p eccodes --example messages

Runtime Files
-------------

The library reads its key definitions and sample messages from
``share/eccodes`` of the installation it was built for. A vendored build
compiles the path of its own build directory in, so a binary runs on the
machine that built it. A binary shipped elsewhere needs one of: the
``memfs`` feature of ``eccodes-sys``, which embeds both sets in
``libeccodes``; the environment variables ``ECCODES_DEFINITION_PATH`` and
``ECCODES_SAMPLES_PATH``; ``Library::set_definitions_path`` and
``Library::set_samples_path`` at start-up. The build copies both sets to
``eccodes_resources`` in the Cargo target profile directory, to ship next to
the binary and name in those variables.

Cargo Features
--------------

Besides ``vendored`` and ``system``, the ``eccodes`` crate has ``aec``, on
by default, for the CCSDS packing of operational ECMWF GRIB2; ``threads``,
switched on by ``vendored``, for the library's internal locking; ``raw``,
off by default, which exposes the C handle behind a message for handing it
to another library built on ecCodes.

``eccodes-sys`` exposes the CMake options of ecCodes as features, so a
vendored build can be configured from ``Cargo.toml``. A feature switches on
the ``ENABLE_*`` option of the same name.

.. list-table::
   :widths: 25 15 60
   :header-rows: 1

   * - Feature
     - Default
     - Effect
   * - ``product-grib``, ``product-bufr``
     - on
     - GRIB and BUFR support
   * - ``geography``
     - on
     - Grid iterators and nearest-point search
   * - ``eckit-geo``
     - off
     - The ``eckit::geo`` grid backend
   * - ``jpg``
     - on
     - JPEG 2000 packing
   * - ``aec``
     - on
     - CCSDS packing; builds libaec
   * - ``png``
     - off
     - PNG packing
   * - ``netcdf``
     - off
     - NetCDF conversion; needs a system NetCDF
   * - ``memfs``
     - off
     - Embed the definitions and samples in ``libeccodes``
   * - ``eccodes-threads``
     - off
     - POSIX locking inside the library; ``eccodes`` switches it on for
       vendored builds
   * - ``eccodes-omp-threads``
     - off
     - OpenMP locking inside the library

To switch one on, add ``eccodes-sys`` as a dependency next to ``eccodes``
and enable the feature there:

.. code-block:: bash

   cargo add eccodes
   cargo add eccodes-sys --features memfs
