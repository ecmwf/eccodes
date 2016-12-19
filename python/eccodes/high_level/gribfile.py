"""
``GribFile`` class that implements a GRIB file that closes itself and its
messages when it is no longer needed.

Author: Daniel Lee, DWD, 2014
"""

from .. import eccodes
from .gribmessage import GribMessage


class GribFile(file):
    """
    A GRIB file handle meant for use in a context manager.

    Individual messages can be accessed using the ``next`` method. Of course,
    it is also possible to iterate over each message in the file::

        >>> with GribFile(filename) as grib:
        ...     # Print number of messages in file
        ...     len(grib)
        ...     # Open all messages in file
        ...     for msg in grib:
        ...         print(msg["shortName"])
        ...     len(grib.open_messages)
        >>> # When the file is closed, any open messages are closed
        >>> len(grib.open_messages)
    """

    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        """Close all open messages, release GRIB file handle and close file."""
        while self.open_messages:
            self.open_messages.pop().close()
        self.file_handle.close()

    def close(self):
        """Possibility to manually close file."""
        self.__exit__(None, None, None)

    def __len__(self):
        """Return total messages in GRIB file."""
        return eccodes.codes_count_in_file(self.file_handle)

    def __iter__(self):
        return self

    def next(self):
        try:
            return GribMessage(self)
        except IOError:
            raise StopIteration()

    def __init__(self, filename, mode="r"):
        """Open file and receive GRIB file handle."""
        #: File handle for working with actual file on disc
        #: The class holds the file it works with because the GRIB API's
        #: typechecking does not allow using inherited classes.
        self.file_handle = open(filename, mode)
        #: Number of message in GRIB file currently being read
        self.message = 0
        #: Open messages
        self.open_messages = []
