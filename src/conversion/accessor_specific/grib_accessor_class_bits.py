from accessor_specific.default import AccessorSpecific
import arg

class BitsDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
    custom_arg_transforms = {
        arg.Arg("unsigned char*","p") : arg.Arg("DataPointer","p"),
        arg.Arg("int","type") : arg.Arg("GribType","type"),
        }

    def add_custom_transforms(self, transforms):
        for carg, cpparg in self.custom_arg_transforms.items():
            transforms.add_custom_args(carg, cpparg)

        return transforms
