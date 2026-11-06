.. SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Rust Interface
==============

The Rust interface lives in the ``rust/`` directory of the ecCodes repository
and calls the ecCodes C library. ``eccodes`` is the crate applications depend
on; ``eccodes-sys`` builds or links the C library underneath it.

.. todo: Coverage table Rust vs C, quick start, approach, usage, building.

The per-item API reference is on docs.rs: `eccodes <https://docs.rs/eccodes>`_
and `eccodes-sys <https://docs.rs/eccodes-sys>`_.
