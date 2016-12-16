"""
``GribFile`` class that implements a GRIB file that closes itself and its
messages when it is no longer needed.

Author: Daniel Lee, DWD, 2014
"""

from .codesfile import CodesFile
from .gribmessage import GribMessage


class GribFile(CodesFile):

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

    MessageClass = GribMessage
