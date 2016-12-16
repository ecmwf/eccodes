"""
``CodesFile`` class that implements a file that is readable by ecCodes and
closes itself and its messages when it is no longer needed.

Author: Daniel Lee, DWD, 2016
"""

from .. import eccodes


class CodesFile(file):

    """
    An abstract class to specify and/or implement common behavior that files
    read by ecCodes should implement.
    """

    #: Type of messages belonging to this file
    MessageClass = None

    def __init__(self, filename, mode="r"):
        """Open file and receive codes file handle."""
        #: File handle for working with actual file on disc
        #: The class holds the file it works with because ecCodes'
        # typechecking does not allow using inherited classes.
        self.file_handle = open(filename, mode)
        #: Number of message in file currently being read
        self.message = 0
        #: Open messages
        self.open_messages = []

    def __exit__(self, type, value, traceback):
        """Close all open messages, release GRIB file handle and close file."""
        while self.open_messages:
            self.open_messages.pop().close()
        self.file_handle.close()

    def __len__(self):
        """Return total number of messages in file."""
        return eccodes.codes_count_in_file(self.file_handle)

    def __enter__(self):
        return self

    def close(self):
        """Possibility to manually close file."""
        self.__exit__(None, None, None)

    def __iter__(self):
        return self

    def next(self):
        try:
            return self.MessageClass(self)
        except IOError:
            raise StopIteration()
