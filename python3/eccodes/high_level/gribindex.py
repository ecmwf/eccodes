"""
``GribIndex`` class that implements a GRIB index that allows access to
ecCodes's index functionality.

Author: Daniel Lee, DWD, 2014
"""

from .. import eccodes
from .gribmessage import GribMessage


class GribIndex(object):
    """
    A GRIB index meant for use in a context manager.

    Usage::

        >>> # Create index from file with keys
        >>> with GribIndex(filename, keys) as idx:
        ...     # Write index to file
        ...     idx.write(index_file)
        >>> # Read index from file
        >>> with GribIndex(file_index=index_file) as idx:
        ...     # Add new file to index
        ...     idx.add(other_filename)
        ...     # Report number of unique values for given key
        ...     idx.size(key)
        ...     # Report unique values indexed by key
        ...     idx.values(key)
        ...     # Request GribMessage matching key, value
        ...     msg = idx.select({key: value})
    """

    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        """Release GRIB message handle and inform file of release."""
        while self.open_messages:
            self.open_messages[0].close()
        eccodes.codes_index_release(self.iid)

    def close(self):
        """Possibility to manually close index."""
        self.__exit__(None, None, None)

    def __init__(self, filename=None, keys=None, file_index=None,
                 grib_index=None):
        """
        Create new GRIB index over ``keys`` from ``filename``.

        ``filename`` should be a string of the desired file's filename.
        ``keys`` should be a sequence of keys to index. ``file_index`` should
        be a string of the file that the index should be loaded from.
        ``grib_index`` should be another ``GribIndex``.

        If ``filename`` and ``keys`` are provided, the ``GribIndex`` is
        initialized over the given keys from the given file. If they are not
        provided, the ``GribIndex`` is read from ``indexfile``. If
        ``grib_index`` is provided, it is cloned from the given ``GribIndex``.
        """
        #: Grib index ID
        self.iid = None
        if filename and keys:
            self.iid = eccodes.codes_index_new_from_file(filename, keys)
        elif file_index:
            self.iid = eccodes.codes_index_read(file_index)
        elif grib_index:
            self.iid = eccodes.codes_new_from_index(grib_index.iid)
        else:
            raise RuntimeError("No source was supplied "
                               "(possibilities: grib_file, clone, sample).")
        #: Indexed keys. Only available if GRIB is initialized from file.
        self.keys = keys
        #: Open GRIB messages
        self.open_messages = []

    def size(self, key):
        """Return number of distinct values for index key."""
        return eccodes.codes_index_get_size(self.iid, key)

    def values(self, key, ktype=str):
        """Return distinct values of index key."""
        return eccodes.codes_index_get(self.iid, key, ktype)

    def add(self, filename):
        """Add ``filename`` to the ``GribIndex``."""
        eccodes.codes_index_add_file(self.iid, filename)

    def write(self, outfile):
        """Write index to filename at ``outfile``."""
        eccodes.codes_index_write(self.iid, outfile)

    def select(self, key_value_pairs):
        """
        Return message associated with given key value pairs.

        ``key_value_pairs`` should be passed as a dictionary.
        """
        for key in key_value_pairs:
            eccodes.codes_index_select(self.iid, key, key_value_pairs[key])
        return GribMessage(gribindex=self)
