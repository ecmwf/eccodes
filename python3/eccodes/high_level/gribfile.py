"""
``GribFile`` class that implements a GRIB file that closes itself and its
messages when it is no longer needed.

Author: Daniel Lee, DWD, 2014
"""

from .codesfile import CodesFile
from .gribmessage import GribMessage


class GribFile(CodesFile):

    __doc__ = "\n".join(CodesFile.__doc__.splitlines()[4:]).format(
        prod_type="GRIB", classname="GribFile", alias="grib")

    MessageClass = GribMessage
