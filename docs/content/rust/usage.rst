.. SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Using the ``eccodes`` Crate
===========================

The code on this page is quoted from the examples of the ``eccodes`` crate;
:ref:`content/rust/building:Running the Examples` says how to run them. The
methods of each type are listed on `docs.rs <https://docs.rs/eccodes>`_.

Reading Files
-------------

``GribFile::open`` fails at once when the path is not a readable file.
Messages come in file order. ``messages_from`` reads from any
``std::io::Read``; wrap a socket or a raw file in a ``BufReader``, since the
library scans for the start of a message a byte at a time:

.. literalinclude:: ../../../rust/crates/eccodes/examples/messages.rs
   :language: rust
   :start-after: // [read]
   :end-before: // [end read]
   :dedent:

``BufrFile`` reads BUFR the same way and ``AnyFile`` reads a file of mixed
products as ``Message``, whose ``kind`` reports what each one is.

Keys
----

The type the value is read into selects the ecCodes accessor. ``key``
inspects one key without reading it and ``keys`` lists the keys of a
namespace:

.. literalinclude:: ../../../rust/crates/eccodes/examples/messages.rs
   :language: rust
   :start-after: // [keys]
   :end-before: // [end keys]
   :dedent:

Writing Messages
----------------

A new message starts from one of the library's samples, ``GRIB2`` or
``GRIB1``. ``set`` writes a key, ``set_values`` the data section and
``write_to`` the encoded bytes to any ``std::io::Write``:

.. literalinclude:: ../../../rust/crates/eccodes/examples/write.rs
   :language: rust
   :start-after: // [write]
   :end-before: // [end write]
   :dedent:

``try_clone`` copies a message so the copy can be edited on its own:

.. literalinclude:: ../../../rust/crates/eccodes/examples/write.rs
   :language: rust
   :start-after: // [bytes]
   :end-before: // [end bytes]
   :dedent:

A point without data is coded through the bitmap: write the sentinel
``missing::F64`` as its value, with ``missingValue`` set to the sentinel and
``bitmapPresent`` on. Reading the values back gives the sentinel at those
points, which ``missing::is_f64`` detects:

.. literalinclude:: ../../../rust/crates/eccodes/examples/write.rs
   :language: rust
   :start-after: // [missing]
   :end-before: // [end missing]
   :dedent:

Grids
-----

``values`` returns the data section in grid order. ``grid_points`` walks the
grid with the value at each point and ``grid_positions`` walks the
coordinates alone, leaving the data section undecoded:

.. literalinclude:: ../../../rust/crates/eccodes/examples/grid.rs
   :language: rust
   :start-after: // [grid]
   :end-before: // [end grid]
   :dedent:

Nearest Points
--------------

``nearest`` prepares a search over the message's geometry; keep it for
several queries on the same message. ``find`` returns the four grid points
around a position, with the value and the distance in kilometres;
``find_each`` returns the single nearest point to each of several positions:

.. literalinclude:: ../../../rust/crates/eccodes/examples/grid.rs
   :language: rust
   :start-after: // [nearest]
   :end-before: // [end nearest]
   :dedent:

Indexes
-------

An ``Index`` records, for a set of keys, which messages of one or more files
hold each value. ``values`` lists the distinct values of a key, ``select``
narrows the selection one key at a time and ``messages`` reads the matching
messages. Every key of the index needs a selected value before ``messages``:

.. literalinclude:: ../../../rust/crates/eccodes/examples/index.rs
   :language: rust
   :start-after: // [index]
   :end-before: // [end index]
   :dedent:

Multi-field Messages
--------------------

``GribMultiField`` collects GRIB2 fields into one message that repeats
sections 4 to 8 per field. Reading it back, a file yields one message by
default and one message per field with ``multi_field`` on:

.. literalinclude:: ../../../rust/crates/eccodes/examples/multi.rs
   :language: rust
   :start-after: // [multi]
   :end-before: // [end multi]
   :dedent:

BUFR
----

A BUFR message is built from the ``BUFR4`` sample by setting the number of
subsets, the descriptors and the data keys, then the ``pack`` key:

.. literalinclude:: ../../../rust/crates/eccodes/examples/bufr.rs
   :language: rust
   :start-after: // [encode]
   :end-before: // [end encode]
   :dedent:

Reading one back, the header keys are available at once and the data keys
only after ``unpack``. An attribute of a data key is read with ``->``:

.. literalinclude:: ../../../rust/crates/eccodes/examples/bufr.rs
   :language: rust
   :start-after: // [decode]
   :end-before: // [end decode]
   :dedent:

Logging
-------

The C library writes diagnostics of its own, to stderr unless told
otherwise. ``Library::set_logging(Logging::Rust)`` routes them through the
``log`` crate under the target ``eccodes``; ``set_log_handler`` takes a
closure instead:

.. literalinclude:: ../../../rust/crates/eccodes/examples/errors.rs
   :language: rust
   :start-after: // [logging]
   :end-before: // [end logging]
   :dedent:

Errors
------

``code`` gives the ``Code`` the C library returned, ``key`` and ``path`` what
the call was about and ``io_error`` the I/O error when ``code`` is
``None``:

.. literalinclude:: ../../../rust/crates/eccodes/examples/errors.rs
   :language: rust
   :start-after: // [errors]
   :end-before: // [end errors]
   :dedent:

Threads
-------

``GribFile``, ``BufrFile`` and ``AnyFile`` are ``Send`` and ``Sync``.

``Message``, ``Index`` and ``GribMultiField`` are ``Send`` but not ``Sync``.

The iterators, ``Messages``, ``Keys``, ``GridPoints``, ``GridPositions``
and ``Nearest``, are neither.
