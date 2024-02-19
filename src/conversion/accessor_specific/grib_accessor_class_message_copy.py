from accessor_specific.default import AccessorSpecific
import arg

class MessageCopyDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
        self._custom_arg_transforms["ALL"] = {
            arg.Arg("unsigned char*","v") : arg.Arg("AccessorDataPointer","v"),
            }
