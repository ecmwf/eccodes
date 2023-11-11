from accessor_specific.default import AccessorSpecific
import arg

class BitsDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
        self._custom_arg_transforms = {
            arg.Arg("unsigned char*","p") : arg.Arg("DataPointer","p"),
            arg.Arg("int","type") : arg.Arg("GribType","type"),
            }

