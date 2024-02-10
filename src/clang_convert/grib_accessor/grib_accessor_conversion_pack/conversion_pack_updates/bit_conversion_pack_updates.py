
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
from code_object.arg import Arg

class BitConversionPackUpdates(base_conversion_pack_updates.BaseConversionPackUpdates):
    def __init__(self) -> None:
        super().__init__()

        self._update_funcs.update({
            "pack_long": self.apply_updates_for_pack_long
        })

    def apply_updates_for_pack_long(self, conversion_pack):
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("unsigned char*","mdata"), Arg("AccessorDataPointer","mdata"))
