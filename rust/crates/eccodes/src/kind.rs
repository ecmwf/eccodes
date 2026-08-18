//! Product kinds — what a message *is*, at compile time and at run time.
//!
//! At compile time a [`Message`](crate::Message) is tagged with a marker:
//!
//! - [`Any`] — any message; the default, and what a mixed file yields
//! - [`Grib`] — a GRIB message, unlocking the grid API
//! - [`Bufr`] — a BUFR message, unlocking the observation API
//!
//! The markers are uninhabited: they exist to parameterise types, never to be
//! constructed. [`MessageKind`] is sealed — these three are all there are.
//!
//! At run time [`Kind`] names the product a message actually holds, decoded
//! from the WMO framing rather than from what the reader was asked for:
//!
//! ```no_run
//! use eccodes::{AnyFile, Kind};
//!
//! # fn main() -> eccodes::Result<()> {
//! for message in &AnyFile::open("mixed.bin")? {
//!     let message = message?;
//!     match message.kind()? {
//!         Kind::Grib => println!("grib"),
//!         Kind::Bufr => println!("bufr"),
//!         other => println!("{other}"),
//!     }
//! }
//! # Ok(())
//! # }
//! ```

use std::fmt;

use eccodes_sys as sys;

/// The product a message holds.
///
/// Determined by the message's own framing — see [`Kind::from_bytes`].
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
#[non_exhaustive]
pub enum Kind {
    /// GRIB, editions 1 and 2.
    Grib,
    /// BUFR.
    Bufr,
    /// METAR.
    Metar,
    /// TAF.
    Taf,
    /// GTS bulletin.
    Gts,
    /// Framing this crate does not recognise.
    Unknown,
}

impl Kind {
    /// The product a message starts with, from its first four bytes.
    ///
    /// Every WMO product this crate knows is identified by its leading
    /// octets — `GRIB`, `BUFR`, `META`, `TAF ` or the GTS control sequence.
    /// Anything shorter than four bytes, or with framing outside that set, is
    /// [`Kind::Unknown`].
    ///
    /// This is deliberately not `codes_get_product_kind`: the C call reports
    /// the product the *reader* was opened with, so a GRIB message read from
    /// a mixed file reports "any".
    #[must_use]
    pub const fn from_bytes(bytes: &[u8]) -> Self {
        if bytes.len() < 4 {
            return Self::Unknown;
        }
        match [bytes[0], bytes[1], bytes[2], bytes[3]] {
            [b'G', b'R', b'I', b'B'] => Self::Grib,
            [b'B', b'U', b'F', b'R'] => Self::Bufr,
            [b'M', b'E', b'T', b'A'] => Self::Metar,
            // The trailing space is part of the identifier.
            [b'T', b'A', b'F', b' '] => Self::Taf,
            // GTS bulletins open with SOH CR CR LF, not with a name.
            [0x01, 0x0D, 0x0D, 0x0A] => Self::Gts,
            _ => Self::Unknown,
        }
    }
}

impl fmt::Display for Kind {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.write_str(match self {
            Self::Grib => "GRIB",
            Self::Bufr => "BUFR",
            Self::Metar => "METAR",
            Self::Taf => "TAF",
            Self::Gts => "GTS",
            Self::Unknown => "unknown",
        })
    }
}

mod sealed {
    /// Implemented only for the markers in this module.
    pub trait Sealed {}
}

/// A compile-time product marker: [`Any`], [`Grib`] or [`Bufr`].
///
/// Sealed — the three markers in this module are the only implementors, and
/// the trait carries no method a caller could need.
pub trait MessageKind: sealed::Sealed + 'static {
    /// The marker's name, as it appears in `Debug` output.
    #[doc(hidden)]
    const NAME: &'static str;

    /// The product a message must hold to wear this marker, or `None` where
    /// any product will do.
    #[doc(hidden)]
    const EXPECTED: Option<Kind>;
}

/// Marker for a message of any product.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Any {}

/// Marker for a GRIB message.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Grib {}

/// Marker for a BUFR message.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Bufr {}

impl sealed::Sealed for Any {}
impl MessageKind for Any {
    const NAME: &'static str = "Any";
    const EXPECTED: Option<Kind> = None;
}

impl sealed::Sealed for Grib {}
impl MessageKind for Grib {
    const NAME: &'static str = "Grib";
    const EXPECTED: Option<Kind> = Some(Kind::Grib);
}

impl sealed::Sealed for Bufr {}
impl MessageKind for Bufr {
    const NAME: &'static str = "Bufr";
    const EXPECTED: Option<Kind> = Some(Kind::Bufr);
}

/// The `ProductKind` the C reader entry points expect for a marker.
///
/// Crate-internal: the raw product constant never reaches a caller, who
/// selects the product by naming a type instead.
pub(crate) const fn product_of<K: MessageKind>() -> sys::ProductKind {
    match K::EXPECTED {
        Some(Kind::Grib) => sys::ProductKind_PRODUCT_GRIB,
        Some(Kind::Bufr) => sys::ProductKind_PRODUCT_BUFR,
        // No marker exists for the remaining products, so they read as "any".
        _ => sys::ProductKind_PRODUCT_ANY,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn truncated_framing_is_not_guessed() {
        // A partial read must never be reported as a product: the first three
        // bytes of a GRIB message are also the first three of nothing else,
        // but we have not seen enough to say so.
        assert_eq!(Kind::from_bytes(b"GRI"), Kind::Unknown);
        assert_eq!(Kind::from_bytes(b""), Kind::Unknown);
    }

    #[test]
    fn taf_needs_its_trailing_space() {
        // "TAF " is four octets including the pad; "TAFS" is a different
        // product's problem, not a TAF.
        assert_eq!(Kind::from_bytes(b"TAF "), Kind::Taf);
        assert_eq!(Kind::from_bytes(b"TAFS"), Kind::Unknown);
    }

    #[test]
    fn gts_is_recognised_by_control_characters() {
        // GTS bulletins are the one product not named in ASCII — the case a
        // string comparison on the identifier would miss.
        assert_eq!(Kind::from_bytes(&[0x01, 0x0D, 0x0D, 0x0A, b'T']), Kind::Gts);
    }

    #[test]
    fn markers_map_onto_the_reader_products() {
        assert_eq!(product_of::<Any>(), sys::ProductKind_PRODUCT_ANY);
        assert_eq!(product_of::<Grib>(), sys::ProductKind_PRODUCT_GRIB);
        assert_eq!(product_of::<Bufr>(), sys::ProductKind_PRODUCT_BUFR);
    }
}
