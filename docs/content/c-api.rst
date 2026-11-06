.. SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

C API
=====

The C API is declared in ``eccodes.h``. The reference below is generated from
that header with Doxygen and Breathe.

.. todo: Group the header by section (handles, keys, iterators, index, nearest, multi-field) instead of one flat dump.

Handles
-------

.. doxygenfunction:: codes_handle_new_from_file

.. doxygenfunction:: codes_handle_delete

Keys
----

.. doxygenfunction:: codes_get_long

.. doxygenfunction:: codes_get_double

.. doxygenfunction:: codes_get_string
