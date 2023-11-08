from accessor_specific.default import AccessorSpecific
import arg

class MessageCopyDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
   
    custom_arg_transforms = {
        arg.Arg("unsigned char*","v") : arg.Arg("DataPointer","v"),
        }

    def add_custom_transforms(self, transforms):
        for carg, cpparg in self.custom_arg_transforms.items():
            transforms.add_custom_args(carg, cpparg)

        return transforms
