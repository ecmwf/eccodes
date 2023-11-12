# This is the default interface if a grib_accessor doesn't have any specifics!

class AccessorSpecific:
    def __init__(self) -> None:
        self._custom_arg_transforms = {}
        self._custom_member_arg_transforms = {}
            
    def update_converters(self, converters):
        return converters
    
    def add_custom_transforms(self, transforms):
        for carg, cpparg in self._custom_arg_transforms.items():
            transforms.add_custom_args(carg, cpparg)

        # Note: These represent members, but are stored as ARGS!!!
        for carg, cpparg in self._custom_member_arg_transforms.items():
            transforms.add_custom_member_args(carg, cpparg)

        return transforms
    
    def custom_includes(self):
        return []
