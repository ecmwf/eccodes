"""
``GribMessage`` class that implements a GRIB message that allows access to
the message's key-value pairs in a dictionary-like manner and closes the
message when it is no longer needed, coordinating this with its host file.

Author: Daniel Lee, DWD, 2014
"""

import collections

import eccodes


class IndexNotSelectedError(Exception):
    """GRIB index was requested before selecting key/value pairs."""


class GribMessage(object):
    """
    A GRIB message.

    Each ``GribMessage`` is stored as a key/value pair in a dictionary-like
    structure. It can be used in a context manager or by itself. When the
    ``GribFile`` it belongs to is closed, it closes any open ``GribMessage``s
    that belong to it. If a ``GribMessage`` is closed before its ``GribFile``
    is closed, it informs the ``GribFile`` of its closure.

    Scalar and vector values are set appropriately through the same method.

    Usage::

        >>> with GribFile(filename) as grib:
        ...     # Access shortNames of all messages
        ...     for msg in grib:
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
        ...     msg2 = GribMessage(clone=msg)
        ...     # If desired, messages can be closed manually or used in with
        ...     msg.close()
    """
    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        """Release GRIB message handle and inform file of release."""
        # This assert should never trigger
        # assert self.gid is not None
        eccodes.codes_release(self.gid)
        if self.grib_index:
            self.grib_index.open_messages.remove(self)

    def close(self):
        """Possibility to manually close message."""
        self.__exit__(None, None, None)

    def __contains__(self, key):
        """Check whether a key is present in message."""
        return key in self.keys()

    def __len__(self):
        """Return key count."""
        return len(self.keys())

    def __getitem__(self, key):
        """Return value associated with key as its native type."""
        return self.get(key)

    def __setitem__(self, key, value):
        """
        Set value associated with key.

        If the object is iterable,
        """
        # Alternative implemented (TODO: evaluate)
        # if eccodes.codes_get_size(self.gid, key) > 1:
        #     eccodes.codes_set_array(self.gid, key, value)
        # else:
        #     eccodes.codes_set(self.gid, key, value)

        # Passed value is iterable and not string
        if (isinstance(value, collections.Iterable) and not
                isinstance(value, basestring)):
            eccodes.codes_set_array(self.gid, key, value)
        else:
            eccodes.codes_set(self.gid, key, value)

    def __iter__(self):
        return iter(self.keys())

    # Not yet implemented
    # def itervalues(self):
    #    return self.values()

    def items(self):
        """Return list of tuples of all key/value pairs."""
        return [(key, self[key]) for key in self.keys()]

    def keys(self, namespace=None):
        """Get available keys in message."""
        iterator = eccodes.codes_keys_iterator_new(self.gid, namespace=namespace)
        keys = []
        while eccodes.codes_keys_iterator_next(iterator):
            key = eccodes.codes_keys_iterator_get_name(iterator)
            keys.append(key)
        eccodes.codes_keys_iterator_delete(iterator)
        return keys

    def __init__(self, grib_file=None, clone=None, sample=None, gribindex=None):
        """
        Open a message and inform the GRIB file that it's been incremented.

        If ``grib_file`` is not supplied, the message is cloned from
        ``GribMessage`` ``clone``. If neither is supplied, the ``GribMessage``
        is cloned from ``sample``. If ``index`` is supplied as a GribIndex, the
        message is taken from the index.
        """
        #: Unique GRIB ID, for GRIB API interface
        self.gid = None
        #: File containing message
        self.grib_file = None
        #: GribIndex referencing message
        self.grib_index = None
        if grib_file is not None:
            self.gid = eccodes.codes_grib_new_from_file(grib_file.file_handle)
            if self.gid is None:
                raise IOError("Grib file %s is exhausted" % grib_file.name)
            self.grib_file = grib_file
            self.grib_file.message += 1
            self.grib_file.open_messages.append(self)
        elif clone is not None:
            self.gid = eccodes.codes_clone(clone.gid)
        elif sample is not None:
            self.gid = eccodes.codes_grib_new_from_samples(sample)
        elif gribindex is not None:
            self.gid = eccodes.codes_new_from_index(gribindex.iid)
            if not self.gid:
                raise IndexNotSelectedError("All keys must have selected "
                                            "values before receiving message "
                                            "from index.")
            self.grib_index = gribindex
            gribindex.open_messages.append(self)
        else:
            raise RuntimeError("Either grib_file, clone, sample or gribindex "
                               "must be provided.")

    def size(self):
        """Return size of message in bytes."""
        return eccodes.codes_get_message_size(self.gid)

    def dump(self):
        """Dump message's binary content."""
        return eccodes.codes_get_message(self.gid)

    def get(self, key, ktype=None):
        """Get value of a given key as its native or specified type."""
        if self.missing(key):
            raise KeyError("Key is missing from message.")
        if eccodes.codes_get_size(self.gid, key) > 1:
            ret = eccodes.codes_get_array(self.gid, key, ktype)
        else:
            ret = eccodes.codes_get(self.gid, key, ktype)
        return ret

    def missing(self, key):
        """Report if key is missing."""
        return bool(eccodes.codes_is_missing(self.gid, key))

    def set_missing(self, key):
        """Set a key to missing."""
        eccodes.codes_set_missing(self.gid, key)

    def write(self, outfile=None):
        """Write message to file."""
        if not outfile:
            # This is a hack because the API does not accept inheritance
            outfile = self.grib_file.file_handle
        eccodes.codes_write(self.gid, outfile)
