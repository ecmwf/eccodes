from accessor_specific.default import AccessorSpecific
import arg

class G1HalfByteCodeflagDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
        self._custom_arg_transforms = {
            arg.Arg("unsigned char*","dat") : arg.Arg("DataPointer","dat"),
            }

