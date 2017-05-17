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


class BufrMessage(CodesMessage):

    __doc__ = "\n".join(CodesMessage.__doc__.splitlines()[4:]).format(
        prod_type="BUFR", classname="BufrMessage", parent="BufrFile",
        alias="bufr")

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
        #self._unpacked = False

    #def get(self, key, ktype=None):
    #    """Return requested value, unpacking data values if necessary."""
    #    # TODO: Only do this if accessing arrays that need unpacking
    #    if not self._unpacked:
    #        self.unpacked = True
    #    return super(self.__class__, self).get(key, ktype)

    #def missing(self, key):
    #    """
    #    Report if key is missing.#
    #
    #    Overloaded due to confusing behaviour in ``codes_is_missing`` (SUP-1874).
    #    """
    #    return not bool(eccodes.codes_is_defined(self.codes_id, key))

    def unpack(self):
        """Decode data section"""
        eccodes.codes_set(self.codes_id, 'unpack', 1)

    def pack(self):
        """Encode data section"""
        eccodes.codes_set(self.codes_id, 'pack', 1)

    def keys(self, namespace=None):
        #self.unpack()
        #return super(self.__class__, self).keys(namespace)
        iterator = eccodes.codes_bufr_keys_iterator_new(self.codes_id)
        keys = []
        while eccodes.codes_bufr_keys_iterator_next(iterator):
            key = eccodes.codes_bufr_keys_iterator_get_name(iterator)
            keys.append(key)
        eccodes.codes_bufr_keys_iterator_delete(iterator)
        return keys

    #@property
    #def unpacked(self):
    #    return self._unpacked

    #@unpacked.setter
    #def unpacked(self, val):
    #    eccodes.codes_set(self.codes_id, "unpack", val)
    #    self._unpacked = val

    #def __setitem__(self, key, value):
    #    """Set item and pack BUFR."""
    #    if not self._unpacked:
    #        self.unpacked = True
    #    super(self.__class__, self).__setitem__(key, value)
    #    eccodes.codes_set(self.codes_id, "pack", True)

    def copy_data(self, destMsg):
        """Copy data values from this message to another message"""
        return eccodes.codes_bufr_copy_data(self.codes_id, destMsg.codes_id)

class BufrFile(CodesFile):

    __doc__ = "\n".join(CodesFile.__doc__.splitlines()[4:]).format(
        prod_type="BUFR", classname="BufrFile", alias="bufr")

    MessageClass = BufrMessage
