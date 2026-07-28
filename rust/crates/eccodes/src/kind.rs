//! Compile-time product-kind markers for [`Handle`](crate::Handle).
//!
//! - [`Any`] — any single message (GRIB, BUFR, ...); wraps `codes_handle`
//! - [`Grib`] — a single GRIB message; wraps `codes_handle`, unlocks the
//!   GRIB-only API (geoiterator, nearest, data extraction)
//! - [`GribMulti`] — a multi-field GRIB message under construction; wraps
//!   `codes_multi_handle`
//!
//! The traits are sealed: exactly these three markers exist.

use std::ptr::NonNull;

use eccodes_sys as sys;

mod sealed {
    pub trait Sealed {}
    impl Sealed for super::Any {}
    impl Sealed for super::Grib {}
    impl Sealed for super::GribMulti {}
}

/// A product kind marker, tying a [`Handle`](crate::Handle) to the C struct
/// it wraps and the way it is freed.
pub trait HandleKind: sealed::Sealed {
    /// The underlying C struct (`codes_handle` or `codes_multi_handle`).
    type CType;

    /// Free the underlying C object. Called from `Drop`.
    #[doc(hidden)]
    unsafe fn delete(raw: NonNull<Self::CType>);
}

/// Marker for kinds wrapping a single-message `codes_handle`.
pub trait SingleKind: HandleKind<CType = sys::codes_handle> {}

/// Marker for kinds wrapping a `codes_multi_handle`.
pub trait MultiKind: HandleKind<CType = sys::codes_multi_handle> {}

/// Any single message: GRIB, BUFR, METAR, GTS, TAF.
pub struct Any;

/// A single GRIB message.
pub struct Grib;

/// A multi-field GRIB message built by appending single GRIB messages.
pub struct GribMulti;

impl HandleKind for Any {
    type CType = sys::codes_handle;

    unsafe fn delete(raw: NonNull<Self::CType>) {
        // SAFETY: `raw` is a valid handle owned by the wrapper being dropped;
        // the return code is irrelevant on destruction.
        unsafe { sys::codes_handle_delete(raw.as_ptr()) };
    }
}
impl SingleKind for Any {}

impl HandleKind for Grib {
    type CType = sys::codes_handle;

    unsafe fn delete(raw: NonNull<Self::CType>) {
        // SAFETY: as for `Any` — same C struct, same destructor.
        unsafe { sys::codes_handle_delete(raw.as_ptr()) };
    }
}
impl SingleKind for Grib {}

impl HandleKind for GribMulti {
    type CType = sys::codes_multi_handle;

    unsafe fn delete(raw: NonNull<Self::CType>) {
        // SAFETY: `raw` is a valid multi-handle owned by the wrapper being
        // dropped.
        unsafe { sys::codes_grib_multi_handle_delete(raw.as_ptr()) };
    }
}
impl MultiKind for GribMulti {}
