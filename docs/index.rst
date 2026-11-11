.. SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

ecCodes Documentation
=====================

ecCodes is ECMWF's library for decoding and encoding the WMO formats its data
is distributed in: GRIB editions 1 and 2 and BUFR editions 3 and 4. It ships
a C library, Fortran 90 bindings, command line tools, and Rust crates.

.. admonition:: Work in progress
   :class: warning

   This site is being assembled. The reference documentation remains on
   `Confluence <https://confluence.ecmwf.int/display/ECC/ecCodes+Home>`_
   until each chapter here is complete.

.. toctree::
   :maxdepth: 1
   :caption: Contents

   content/c-api
   content/rust/index

:ref:`The Rust interface <Rust_Introduction>` is the ``eccodes`` crate, a
safe Rust API over the same C library.
