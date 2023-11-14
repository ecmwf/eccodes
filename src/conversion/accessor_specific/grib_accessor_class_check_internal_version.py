from accessor_specific.default import AccessorSpecific
import arg

class CheckInternalVersionDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()

        self._custom_arg_transforms["ALL"] = {
            arg.Arg("const char*","s_defn_version") : arg.Arg("AccessorName","sDefnVersion"),
            }
