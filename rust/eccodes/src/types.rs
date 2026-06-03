//! Common types shared across the ecCodes Rust API.

/// The kind of product contained in a message.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ProductKind {
    Any,
    Grib,
    Bufr,
    Metar,
    Gts,
    Taf,
}

impl ProductKind {
    pub(crate) fn to_sys(self) -> eccodes_sys::ProductKind {
        match self {
            ProductKind::Any => eccodes_sys::ProductKind::PRODUCT_ANY,
            ProductKind::Grib => eccodes_sys::ProductKind::PRODUCT_GRIB,
            ProductKind::Bufr => eccodes_sys::ProductKind::PRODUCT_BUFR,
            ProductKind::Metar => eccodes_sys::ProductKind::PRODUCT_METAR,
            ProductKind::Gts => eccodes_sys::ProductKind::PRODUCT_GTS,
            ProductKind::Taf => eccodes_sys::ProductKind::PRODUCT_TAF,
        }
    }

    pub(crate) fn from_sys(pk: eccodes_sys::ProductKind) -> Self {
        match pk {
            eccodes_sys::ProductKind::PRODUCT_GRIB => ProductKind::Grib,
            eccodes_sys::ProductKind::PRODUCT_BUFR => ProductKind::Bufr,
            eccodes_sys::ProductKind::PRODUCT_METAR => ProductKind::Metar,
            eccodes_sys::ProductKind::PRODUCT_GTS => ProductKind::Gts,
            eccodes_sys::ProductKind::PRODUCT_TAF => ProductKind::Taf,
            _ => ProductKind::Any,
        }
    }
}

/// The native type of a key in a message.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum KeyType {
    Undefined,
    Long,
    Double,
    String,
    Bytes,
    Section,
    Label,
    Missing,
}

impl KeyType {
    pub fn name(&self) -> &'static str {
        match self {
            KeyType::Long => "long",
            KeyType::Double => "double",
            KeyType::String => "string",
            KeyType::Bytes => "bytes",
            KeyType::Undefined => "undefined",
            KeyType::Section => "section",
            KeyType::Label => "label",
            KeyType::Missing => "missing",
        }
    }

    #[cfg(feature = "advanced")]
    pub(crate) fn from_c(t: std::os::raw::c_int) -> Self {
        match t {
            eccodes_sys::CODES_TYPE_LONG => KeyType::Long,
            eccodes_sys::CODES_TYPE_DOUBLE => KeyType::Double,
            eccodes_sys::CODES_TYPE_STRING => KeyType::String,
            eccodes_sys::CODES_TYPE_BYTES => KeyType::Bytes,
            eccodes_sys::CODES_TYPE_SECTION => KeyType::Section,
            eccodes_sys::CODES_TYPE_LABEL => KeyType::Label,
            eccodes_sys::CODES_TYPE_MISSING => KeyType::Missing,
            _ => KeyType::Undefined,
        }
    }
}
