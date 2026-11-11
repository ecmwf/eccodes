.. SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Approach
========

This page gives the reasons behind the shape of the interface. How to use it
is on :doc:`usage`; the C API it calls is on :doc:`../c-api`.

Two Crates
----------

``eccodes-sys``
   Unsafe bindings to the C library, generated with
   `bindgen <https://rust-lang.github.io/rust-bindgen/>`_.

   * :ref:`Builds libeccodes from source or links an installed one <content/rust/building:Building the Rust Crates>`.
   * :ref:`Exposes the ecCodes CMake options as Cargo features <content/rust/building:Cargo Features>`.

``eccodes``
   Safe Rust API over ``eccodes-sys``.

   * :ref:`eccodes::Result and an Error with a code to match on <content/rust/usage:Errors>`.
   * :ref:`The product as a type <content/rust/approach:Product Types>`.
   * :ref:`Key access typed by the value asked for <content/rust/approach:Keys>`.
   * :ref:`Iterators over messages, keys and grid points <content/rust/usage:Reading Files>`.
   * :ref:`The library's diagnostics through the log crate <content/rust/usage:Logging>`.

Product Types
-------------

A message carries its product as a type parameter: ``GribMessage``,
``BufrMessage`` or plain ``Message`` for one read from a mixed file. Grids,
nearest-point search and multi-field writing exist only on ``GribMessage``;
``unpack`` and ``copy_data_from`` only on ``BufrMessage``. ``from_bytes`` on
a typed message refuses bytes of another product and ``try_into_grib`` hands
a ``Message`` back unchanged when it is not GRIB. ``Kind`` names the product
at run time, read from the message's own framing rather than from what the
reader was opened for.

Keys
----

``get`` and ``set`` take the key name and the type decides the decoding:
``i64``, ``f64``, ``f32``, ``bool``, ``String``, ``Vec`` of the numeric and
string ones, ``Vec<u8>`` for bytes and ``Date`` and ``Time`` from the
``time`` crate for the keys that code them as integers. ``Option<T>`` reads
an absent or missing key as ``None`` instead of an error and ``set`` with
``None`` codes a key as missing.

Files
-----

A ``GribFile`` or ``BufrFile`` is a path, not an open stream. Each call to
``messages`` opens its own stream, so a file can be counted and read twice.
A ``Messages`` iterator borrows nothing.

Reading a multi-field GRIB message as one message per field is a property of
one reader, set with ``multi_field``. Asking one reader for fields changes
nothing for another reader or another thread.

Error Type
----------

``eccodes::Error`` holds the ``Code`` the C library returned and the key or
file the call was about. Match on ``code``; ``Display`` prints both, as in
``eccodes::NotFound: Key/value not found (key "shortNam")``.

Threads
-------

A message is ``Send`` and not ``Sync``: the C library caches decoded state
inside a handle, so a read through ``&self`` mutates it. A file is both,
since it is a path.
