"""
Classes for handling BUFR with a high level interface.

``BufrFiles`` can be treated mostly as regular files and used as context
managers, as can ``BufrMessages``. Each of these classes destructs itself and
any child instances appropriately.

Author: Daniel Lee, DWD, 2016
"""

from .. import eccodes
from .codesmessage import CodesMessage
from .codesfile import CodesFile


# TODO: Docstring is mostly redundant, perhaps move into base class?
class BufrMessage(CodesMessage):

    """
    A BUFR message.

    Each ``BufrMessage`` is stored as a key/value pair in a dictionary-like
    structure. It can be used in a context manager or by itself. When the
    ``BufrFile`` it belongs to is closed, it closes any open ``BufrMessage``s
    that belong to it. If a ``BufrMessage`` is closed before its ``BufrFile``
    is closed, it informs the ``BufrFile`` of its closure.

    Scalar and vector values are set appropriately through the same method.

    Usage::

        >>> with BufrFile(filename) as bufr:
        ...     # Access shortNames of all messages
        ...     for msg in bufr:
        ...         print(msg["shortName"])
        ...     # Report number of keys in message
        ...     len(msg)
        ...     # Report message size in bytes
        ...     msg.size
        ...     # Report keys in message
        ...     msg.keys()
        ...     # Check if value is missing
        ...     msg.missing("scaleFactorOfSecondFixedSurface")
        ...     # Set scalar value
        ...     msg["scaleFactorOfSecondFixedSurface"] = 5
        ...     # Check key's value
        ...     msg["scaleFactorOfSecondFixedSurface"]
        ...     # Set value to missing
        ...     msg.set_missing("scaleFactorOfSecondFixedSurface")
        ...     # Missing values raise exception when read with dict notation
        ...     msg["scaleFactorOfSecondFixedSurface"]
        ...     # Array values are set transparently
        ...     msg["values"] = [1, 2, 3]
        ...     # Messages can be written to file
        ...     with open(testfile, "w") as test:
        ...         msg.write(test)
        ...     # Messages can be cloned from other messages
        ...     msg2 = BufrMessage(clone=msg)
        ...     # If desired, messages can be closed manually or used in with
        ...     msg.close()
    """

    product_kind = eccodes.CODES_PRODUCT_BUFR

    # Arguments included explicitly to support introspection
    # TODO: Can we get this to work with an index?
    def __init__(self, codes_file=None, clone=None, sample=None,
                 headers_only=False):
        """
        Open a message and inform the GRIB file that it's been incremented.

        The message is taken from ``codes_file``, cloned from ``clone`` or
        ``sample``, or taken from ``index``, in that order of precedence.
        """
        super(self.__class__, self).__init__(codes_file, clone, sample,
                                             headers_only)
        self._unpacked = False

    def get(self, key, ktype=None):
        """Return requested value, unpacking data values if necessary."""
        # TODO: Only do this if accessing arrays that need unpacking
        if not self._unpacked:
            self.unpacked = True
        return super(self.__class__, self).get(key, ktype)

    def missing(self, key):
        """
        Report if key is missing.

        Overloaded due to confusing behavior in ``codes_is_missing`` (SUP-1874).
        """
        return not bool(eccodes.codes_is_defined(self.codes_id, key))

    def keys(self, namespace=None):
        self.unpacked = True
        return super(self.__class__, self).keys(namespace)

    @property
    def unpacked(self):
        return self._unpacked

    @unpacked.setter
    def unpacked(self, val):
        eccodes.codes_set(self.codes_id, "unpack", val)
        self._unpacked = val

    def __setitem__(self, key, value):
        """Set item and pack BUFR."""
        if not self._unpacked:
            self.unpacked = True
        super(self.__class__, self).__setitem__(key, value)
        eccodes.codes_set(self.codes_id, "pack", True)


class BufrFile(CodesFile):

    """
    A BUFR file handle meant for use in a context manager.

    Individual messages can be accessed using the ``next`` method. Of course,
    it is also possible to iterate over each message in the file::

        >>> with BufrFile(filename) as bufr:
        ...     # Print number of messages in file
        ...     len(bufr)
        ...     # Open all messages in file
        ...     for msg in bufr:
        ...         print(msg["shortName"])
        ...     len(bufr.open_messages)
        >>> # When the file is closed, any open messages are closed
        >>> len(bufr.open_messages)
    """

    MessageClass = BufrMessage
