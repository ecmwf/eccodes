.. SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

.. _Rust_Introduction:

Rust Interface
==============

:Version: |version|

The Rust interface lives in the ``rust/`` directory of the eccodes
repository and calls the ecCodes C library. This chapter is for developers
writing applications against the crates.

``eccodes`` is the crate applications depend on; it gives the C library a
Rust API, with files of messages, keys read and written by name and typed by
the value asked for, indexes and grids. ``eccodes-sys`` builds or links the
C library underneath it.

.. note::

   Use ``eccodes`` in your code. ``eccodes-sys`` goes into a ``Cargo.toml``
   only to switch on a build option of the C library (:doc:`building`);
   calling it directly means raw pointers and status codes.

Coverage
--------

.. list-table::
   :widths: 32 33 35
   :header-rows: 1

   * - Rust
     - C
     - Notes
   * - ``eccodes::GribFile``, ``BufrFile``, ``AnyFile``
     - ``codes_handle_new_from_file``, ``codes_count_in_filename``,
       ``codes_extract_offsets_malloc``
     - Count, index and iterate a file; read any ``std::io::Read``
   * - ``eccodes::Message``, ``GribMessage``, ``BufrMessage``
     - ``codes_handle``, ``codes_get_*``, ``codes_set_*``
     - One message; keys typed by the value asked for
   * - ``eccodes::Key``
     - ``codes_is_defined``, ``codes_is_missing``, ``codes_get_native_type``,
       ``codes_get_size``
     - One key, inspected without reading it
   * - ``eccodes::KeysQuery``, ``Keys``
     - ``codes_keys_iterator_*``
     - The keys of a namespace
   * - ``eccodes::Index``
     - ``codes_index_*``
     - Select messages by key value across files
   * - ``GribMessage::grid_points``, ``grid_positions``, ``data_points``
     - ``codes_grib_iterator_*``, ``codes_grib_get_data``
     - Coordinates and values of a field
   * - ``eccodes::Nearest``
     - ``codes_grib_nearest_*``
     - The grid points nearest to a position
   * - ``eccodes::GribMultiField``
     - ``codes_grib_multi_*``
     - Several fields written as one message
   * - ``BufrMessage::unpack``, ``copy_data_from``
     - The ``unpack`` key, ``codes_bufr_copy_data``
     - The BUFR data section
   * - ``eccodes::Library``
     - ``codes_get_api_version``, ``codes_context_set_*``
     - Version, definitions and samples paths, debug output
   * - ``eccodes::Logging``
     - ``codes_context_set_logging_proc``
     - The library's diagnostics through the ``log`` crate
   * - ``eccodes::Error``, ``Code``
     - The ``CODES_*`` return codes
     - The code plus the key or path the call was about

Quick Start
-----------

Add the crate:

.. code-block:: bash

   cargo add eccodes

The crate version is the version of the ecCodes release it wraps.
:doc:`building` lists the requirements and the alternative of linking an
installed library.

The program builds a message from the library's GRIB2 sample, writes it to a
temporary file and reads the file back:

.. literalinclude:: ../../../rust/crates/eccodes/examples/quickstart.rs
   :language: rust
   :start-after: // [main]
   :end-before: // [end main]

.. toctree::
   :maxdepth: 2
   :caption: Contents
   :hidden:

   approach
   usage
   building

The per-item API reference is on docs.rs: `eccodes <https://docs.rs/eccodes>`_
and `eccodes-sys <https://docs.rs/eccodes-sys>`_.
