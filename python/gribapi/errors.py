"""
Exception class hierarchy
"""

import gribapi_swig as _internal


class GribInternalError(Exception):
    """
    @brief Wrap errors coming from the C API in a Python exception object.

    Base class for all exceptions
    """

    def __init__(self, value):
        # Call the base class constructor with the parameters it needs
        Exception.__init__(self, value)
        if type(value) is int:
            err, self.msg = _internal.grib_c_get_error_string(value, 1024)
            assert err == 0
        else:
            self.msg = value

    def __str__(self):
        return self.msg


class EndOfFileError(GribInternalError):
    """End of resource reached."""
class InternalError(GribInternalError):
    """Internal error."""
class BufferTooSmallError(GribInternalError):
    """Passed buffer is too small."""
class FunctionNotImplementedError(GribInternalError):
    """Function not yet implemented."""
class MessageEndNotFoundError(GribInternalError):
    """Missing 7777 at end of message."""
class ArrayTooSmallError(GribInternalError):
    """Passed array is too small."""
class FileNotFoundError(GribInternalError):
    """File not found."""
class CodeNotFoundInTableError(GribInternalError):
    """Code not found in code table."""
class WrongArraySizeError(GribInternalError):
    """Array size mismatch."""
class KeyValueNotFoundError(GribInternalError):
    """Key/value not found."""
class IOProblemError(GribInternalError):
    """Input output problem."""
class MessageInvalidError(GribInternalError):
    """Message invalid."""
class DecodingError(GribInternalError):
    """Decoding invalid."""
class EncodingError(GribInternalError):
    """Encoding invalid."""
class NoMoreInSetError(GribInternalError):
    """Code cannot unpack because of string too small."""
class GeocalculusError(GribInternalError):
    """Problem with calculation of geographic attributes."""
class OutOfMemoryError(GribInternalError):
    """Out of memory."""
class ReadOnlyError(GribInternalError):
    """Value is read only."""
class InvalidArgumentError(GribInternalError):
    """Invalid argument."""
class NullHandleError(GribInternalError):
    """Null handle."""
class InvalidSectionNumberError(GribInternalError):
    """Invalid section number."""
class ValueCannotBeMissingError(GribInternalError):
    """Value cannot be missing."""
class WrongLengthError(GribInternalError):
    """Wrong message length."""
class InvalidTypeError(GribInternalError):
    """Invalid key type."""
class WrongStepError(GribInternalError):
    """Unable to set step."""
class WrongStepUnitError(GribInternalError):
    """Wrong units for step (step must be integer)."""
class InvalidFileError(GribInternalError):
    """Invalid file id."""
class InvalidGribError(GribInternalError):
    """Invalid grib id."""
class InvalidIndexError(GribInternalError):
    """Invalid index id."""
class InvalidIteratorError(GribInternalError):
    """Invalid iterator id."""
class InvalidKeysIteratorError(GribInternalError):
    """Invalid keys iterator id."""
class InvalidNearestError(GribInternalError):
    """Invalid nearest id."""
class InvalidOrderByError(GribInternalError):
    """Invalid order by."""
class MissingKeyError(GribInternalError):
    """Missing a key from the fieldset."""
class OutOfAreaError(GribInternalError):
    """The point is out of the grid area."""
class ConceptNoMatchError(GribInternalError):
    """Concept no match."""
class HashArrayNoMatchError(GribInternalError):
    """Hash array no match."""
class NoDefinitionsError(GribInternalError):
    """Definitions files not found."""
class WrongTypeError(GribInternalError):
    """Wrong type while packing."""
class EndError(GribInternalError):
    """End of resource."""
class NoValuesError(GribInternalError):
    """Unable to code a field without values."""
class WrongGridError(GribInternalError):
    """Grid description is wrong or inconsistent."""
class EndOfIndexError(GribInternalError):
    """End of index reached."""
class NullIndexError(GribInternalError):
    """Null index."""
class PrematureEndOfFileError(GribInternalError):
    """End of resource reached when reading message."""
class InternalArrayTooSmallError(GribInternalError):
    """An internal array is too small."""
class MessageTooLargeError(GribInternalError):
    """Message is too large for the current architecture."""
class ConstantFieldError(GribInternalError):
    """Constant field."""
class SwitchNoMatchError(GribInternalError):
    """Switch unable to find a matching case."""
class UnderflowError(GribInternalError):
    """Underflow."""
class MessageMalformedError(GribInternalError):
    """Message malformed."""
class CorruptedIndexError(GribInternalError):
    """Index is corrupted."""
class InvalidBitsPerValueError(GribInternalError):
    """Invalid number of bits per value."""
class DifferentEditionError(GribInternalError):
    """Edition of two messages is different."""
class ValueDifferentError(GribInternalError):
    """Value is different."""
class InvalidKeyValueError(GribInternalError):
    """Invalid key value."""
class StringTooSmallError(GribInternalError):
    """String is smaller than requested."""
class WrongConversionError(GribInternalError):
    """Wrong type conversion."""
class MissingBufrEntryError(GribInternalError):
    """Missing BUFR table entry for descriptor."""
class NullPointerError(GribInternalError):
    """Null pointer."""
class AttributeClashError(GribInternalError):
    """Attribute is already present, cannot add."""
class TooManyAttributesError(GribInternalError):
    """Too many attributes. Increase MAX_ACCESSOR_ATTRIBUTES."""
class AttributeNotFoundError(GribInternalError):
    """Attribute not found.."""
class UnsupportedEditionError(GribInternalError):
    """Edition not supported.."""
class OutOfRangeError(GribInternalError):
    """Value out of coding range."""
class WrongBitmapSizeError(GribInternalError):
    """Size of bitmap is incorrect."""
class ValueMismatchError(GribInternalError):
    """Value mismatch."""
class DoubleValueMismatchError(GribInternalError):
    """double values are different."""
class LongValueMismatchError(GribInternalError):
    """long values are different."""
class ByteValueMismatchError(GribInternalError):
    """byte values are different."""
class StringValueMismatchError(GribInternalError):
    """string values are different."""
class OffsetMismatchError(GribInternalError):
    """Offset mismatch."""
class CountMismatchError(GribInternalError):
    """Count mismatch."""
class NameMismatchError(GribInternalError):
    """Name mismatch."""
class TypeMismatchError(GribInternalError):
    """Type mismatch."""
class TypeAndValueMismatchError(GribInternalError):
    """Type and value mismatch."""
class UnableToCompareAccessorsError(GribInternalError):
    """Unable to compare accessors."""
class UnableToResetIteratorError(GribInternalError):
    """Unable to reset iterator."""
class AssertionFailureError(GribInternalError):
    """Assertion failure."""

ERROR_MAP = {
    -66 : WrongBitmapSizeError,
    -65 : OutOfRangeError,
    -64 : UnsupportedEditionError,
    -63 : AttributeNotFoundError,
    -62 : TooManyAttributesError,
    -61 : AttributeClashError,
    -60 : NullPointerError,
    -59 : MissingBufrEntryError,
    -58 : WrongConversionError,
    -57 : StringTooSmallError,
    -56 : InvalidKeyValueError,
    -55 : ValueDifferentError,
    -54 : DifferentEditionError,
    -53 : InvalidBitsPerValueError,
    -52 : CorruptedIndexError,
    -51 : MessageMalformedError,
    -50 : UnderflowError,
    -49 : SwitchNoMatchError,
    -48 : ConstantFieldError,
    -47 : MessageTooLargeError,
    -46 : InternalArrayTooSmallError,
    -45 : PrematureEndOfFileError,
    -44 : NullIndexError,
    -43 : EndOfIndexError,
    -42 : WrongGridError,
    -41 : NoValuesError,
    -40 : EndError,
    -39 : WrongTypeError,
    -38 : NoDefinitionsError,
    -37 : HashArrayNoMatchError,
    -36 : ConceptNoMatchError,
    -35 : OutOfAreaError,
    -34 : MissingKeyError,
    -33 : InvalidOrderByError,
    -32 : InvalidNearestError,
    -31 : InvalidKeysIteratorError,
    -30 : InvalidIteratorError,
    -29 : InvalidIndexError,
    -28 : InvalidGribError,
    -27 : InvalidFileError,
    -26 : WrongStepUnitError,
    -25 : WrongStepError,
    -24 : InvalidTypeError,
    -23 : WrongLengthError,
    -22 : ValueCannotBeMissingError,
    -21 : InvalidSectionNumberError,
    -20 : NullHandleError,
    -19 : InvalidArgumentError,
    -18 : ReadOnlyError,
    -17 : OutOfMemoryError,
    -16 : GeocalculusError,
    -15 : NoMoreInSetError,
    -14 : EncodingError,
    -13 : DecodingError,
    -12 : MessageInvalidError,
    -11 : IOProblemError,
    -10 : KeyValueNotFoundError,
    -9  : WrongArraySizeError,
    -8  : CodeNotFoundInTableError,
    -7  : FileNotFoundError,
    -6  : ArrayTooSmallError,
    -5  : MessageEndNotFoundError,
    -4  : FunctionNotImplementedError,
    -3  : BufferTooSmallError,
    -2  : InternalError,
    -1  : EndOfFileError,
    1   : ValueMismatchError,
    2   : DoubleValueMismatchError,
    3   : LongValueMismatchError,
    4   : ByteValueMismatchError,
    5   : StringValueMismatchError,
    6   : OffsetMismatchError,
    7   : CountMismatchError,
    8   : NameMismatchError,
    9   : TypeMismatchError,
    10  : TypeAndValueMismatchError,
    11  : UnableToCompareAccessorsError,
    12  : UnableToResetIteratorError,
    13  : AssertionFailureError
}


def raise_grib_error(errid):
    """
    Raise the GribInternalError corresponding to ``errid``.
    """
    raise ERROR_MAP[errid](errid)
