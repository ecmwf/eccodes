from accessor_specific.default import AccessorSpecific
import arg

class CountMissingDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
        self._custom_arg_transforms = {
            arg.Arg("unsigned char*","p") : arg.Arg("AccessorDataPointer","p"),
            }

        self._custom_final_line_transforms = {
            "longValues[0] += bitsoff[(p) | used[unusedBitsInBitmap]];" : "longValues[0] += bitsoff[(*p) | used[unusedBitsInBitmap]];",
            }
    
