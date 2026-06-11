//! Raw FFI bindings to the ECMWF eccodes C library.
//!
//! Unsafe, low-level. See the high-level `eccodes` crate for a safe API.

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(clippy::missing_safety_doc)]
#![allow(clippy::too_many_arguments)]
#![allow(clippy::upper_case_acronyms)]
#![allow(clippy::doc_markdown)]
#![allow(clippy::unreadable_literal)]
#![allow(clippy::pub_underscore_fields)]
#![allow(clippy::items_after_statements)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
