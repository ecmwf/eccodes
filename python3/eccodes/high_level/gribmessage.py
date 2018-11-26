"""
``GribMessage`` class that implements a GRIB message that allows access to
the message's key-value pairs in a dictionary-like manner and closes the
message when it is no longer needed, coordinating this with its host file.

Author: Daniel Lee, DWD, 2014
"""

from .codesmessage import CodesMessage
from .. import eccodes


class IndexNotSelectedError(Exception):
    """GRIB index was requested before selecting key/value pairs."""


class GribMessage(CodesMessage):

    __doc__ = "\n".join(CodesMessage.__doc__.splitlines()[4:]).format(
        prod_type="GRIB", classname="GribMessage", parent="GribFile",
        alias="grib")

    product_kind = eccodes.CODES_PRODUCT_GRIB

    # Arguments included explicitly to support introspection
    def __init__(self, codes_file=None, clone=None, sample=None,
                 headers_only=False, gribindex=None):
        """
        Open a message and inform the GRIB file that it's been incremented.

        The message is taken from ``codes_file``, cloned from ``clone`` or
        ``sample``, or taken from ``index``, in that order of precedence.
        """
        grib_args_present = True
        if gribindex is None:
            grib_args_present = False
        super(self.__class__, self).__init__(codes_file, clone, sample,
                                             headers_only, grib_args_present)
        #: GribIndex referencing message
        self.grib_index = None
        if gribindex is not None:
            self.codes_id = eccodes.codes_new_from_index(gribindex.iid)
            if not self.codes_id:
                raise IndexNotSelectedError("All keys must have selected "
                                            "values before receiving message "
                                            "from index.")
            self.grib_index = gribindex
            gribindex.open_messages.append(self)

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Release GRIB message handle and inform file of release."""
        super(self.__class__, self).__exit__(exc_type, exc_val, exc_tb)
        if self.grib_index:
            self.grib_index.open_messages.remove(self)

    def missing(self, key):
        """Report if the value of a key is MISSING."""
        return bool(eccodes.codes_is_missing(self.codes_id, key))

    def set_missing(self, key):
        """Set the value of key to MISSING."""
        eccodes.codes_set_missing(self.codes_id, key)

    @property
    def gid(self):
        """Provided for backwards compatibility."""
        return self.codes_id

    @property
    def grib_file(self):
        """Provided for backwards compatibility."""
        return self.codes_file

    @gid.setter
    def gid(self, val):
        self.codes_id = val

    @grib_file.setter
    def grib_file(self, val):
        self.codes_file = val
