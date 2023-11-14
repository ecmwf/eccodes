from accessor_specific.default import AccessorSpecific
import arg

class Md5DataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
        self._custom_arg_transforms["ALL"] = {
            arg.Arg("unsigned char*","mess") : arg.Arg("AccessorDataPointer","mess"),
            }

