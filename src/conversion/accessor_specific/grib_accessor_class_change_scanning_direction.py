from accessor_specific.default import AccessorSpecific
import arg

class ChangeScanningDirectionDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
        self._custom_member_arg_transforms["ALL"] = {
            arg.Arg("const char*","axis") : arg.Arg("std::string","axis"),
            }
