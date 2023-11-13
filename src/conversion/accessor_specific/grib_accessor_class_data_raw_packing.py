from accessor_specific.default import AccessorSpecific
import arg

class DataRawPackingDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
        self._custom_arg_transforms = {
            arg.Arg("unsigned char*","buf") : arg.Arg("AccessorDataPointer","buf"),
            }

