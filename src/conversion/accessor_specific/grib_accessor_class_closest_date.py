from accessor_specific.default import AccessorSpecific
import arg

class ClosestDateDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
    def custom_includes(self):
        return ["<float.h>"]