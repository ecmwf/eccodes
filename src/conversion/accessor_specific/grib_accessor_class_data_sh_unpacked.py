from accessor_specific.default import AccessorSpecific
import arg

class DataShUnpackedDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
        self._custom_arg_transforms = {
            arg.Arg("unsigned char*","buf") : arg.Arg("DataPointer","buf"),
            }

