from __future__ import absolute_import
import sys

from .eccodes import *
from .eccodes import __version__

if sys.version_info >= (2, 6):
    from .high_level.gribfile import GribFile
    from .high_level.gribmessage import GribMessage
    from .high_level.gribindex import GribIndex
    from .high_level.bufr import BufrFile, BufrMessage
