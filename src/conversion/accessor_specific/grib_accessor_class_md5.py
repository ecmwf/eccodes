from accessor_specific.default import AccessorSpecific
import arg

class Md5DataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
    custom_arg_transforms = {
        arg.Arg("unsigned char*","mess") : arg.Arg("DataPointer","mess"),
        }

    def add_custom_transforms(self, transforms):
        for carg, cpparg in self.custom_arg_transforms.items():
            transforms.add_custom_args(carg, cpparg)

        return transforms
