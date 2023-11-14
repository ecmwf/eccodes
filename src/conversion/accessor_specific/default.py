# This is the default interface if a grib_accessor doesn't have any specifics!

class AccessorSpecific:
    def __init__(self) -> None:
        # Transforms should be stored as {cfuncname: {carg, cpparg}}
        # Use ALL as the cfuncname if the transforms apply to any function names
        self._custom_arg_transforms = {}
        self._custom_member_arg_transforms = {}
        self._custom_final_line_transforms = {}
            
    def update_converters(self, converters):
        return converters
    
    # Note - This clears out any previous custom transforms...
    def add_custom_transforms_for(self, cfuncname, transforms):
        transforms.clear_custom_transforms()

        for cname in [cfuncname, "ALL"]:
            if cname in self._custom_arg_transforms.keys():
                for carg, cpparg in self._custom_arg_transforms[cname].items():
                    transforms.add_custom_args(carg, cpparg)

            if cname in self._custom_member_arg_transforms.keys():
                # Note: These represent members, but are stored as ARGS!!!
                for carg, cpparg in self._custom_member_arg_transforms[cname].items():
                    transforms.add_custom_member_args(carg, cpparg)

            if cname in self._custom_final_line_transforms.keys():
                for from_line, to_line in self._custom_final_line_transforms[cname].items():
                    transforms.add_custom_final_line_transforms(from_line, to_line)

        return transforms
    
    def custom_includes(self):
        return []
