
from func_conv import *
import method
import inherited_method_funcsig_conv

# Define base class member mapping
base_members_map = {
    arg.Arg("long","length") : arg.Arg("long","length_"),
    arg.Arg("long","offset") : arg.Arg("long","offset_"),
    arg.Arg("unsigned long","flags") : arg.Arg("unsigned long","flags_"),
    arg.Arg("int","dirty") : arg.Arg("int","dirty_"),
    arg.Arg("grib_virtual_value*","vvalue") : arg.Arg("std::unique_ptr<grib_virtual_value>","vvalue_"),
    arg.Arg("const char*","set") : arg.Arg("std::string","set_")
    }

# Specialisation of FunctionConverter for AccessorData Methods
class MethodConverter(FunctionConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return method.Method(cppfuncsig)

    # Overridden to process self->, super-> etc
    def transform_cstruct_arg(self, cstruct_arg):

        if cstruct_arg:
            cppstruct_arg = None
            cstruct_member = cstruct_arg.member

            # Process member access
            if cstruct_arg.name in ["super", "self", "a"]:
                cppstruct_member = None

                # Find member arg
                for member_dict in [base_members_map, self._transforms.members]:
                    for carg, cpparg in member_dict.items():
                        if carg.name == cstruct_member.name:
                            cppstruct_member = self.apply_cstruct_arg_transforms_for_ctype(carg.type, cstruct_member, cpparg.name)
                            if not cppstruct_member:
                                cppstruct_member = struct_arg.StructArg("", cpparg.name, cstruct_member.index)
                                if cstruct_member.member:
                                    # TODO: Additonal members here means that we've not processed something correctly - need to fix!
                                    cppstruct_member_member = self.apply_default_cstruct_arg_transform(cstruct_member.member)
                                    cppstruct_member.member = cppstruct_member_member
                                    debug.line("transform_cstruct_arg", f"WARNING: Unexpected member, so not processed correctly: {cstruct_member.member.as_string()}")
                                break
                
                # Extra processing for a-> structs where we've failed to match a member
                if not cppstruct_member and cstruct_arg.name == "a":
                    if cstruct_arg.member.name == "name":
                        # Special handling: Replace name member with a string literal (it's only used in logging)
                        cppstruct_member = struct_arg.StructArg("",f"\"{self._transforms.types['self']}\"", "")
                    else:
                        # Set name to None to mark it for deletion
                        debug.line("transform_cstruct_arg", f"Marking for deletion: {cstruct_arg.as_string()}")
                        cppstruct_member = struct_arg.StructArg("", None, "")

                # If super-> then replace with the correct AccessorName:: call, else remove top-level (self->, a-> etc) 
                if cstruct_arg.name == "super":
                    if not cppstruct_member and cstruct_arg.name == "super":
                        # special case super->super
                        cppstruct_arg = struct_arg.StructArg("", self._transforms.types['supersuper'], "")
                    else:
                        cppstruct_arg = struct_arg.StructArg("", self._transforms.types['super'], "")
                        cppstruct_arg.member = struct_arg.StructArg("::", cppstruct_member.name, cppstruct_member.index, cppstruct_member.member)
                else:
                    cppstruct_arg = cppstruct_member

                assert cppstruct_arg, f"Could not transform cstruct_arg: [{cstruct_arg.as_string()}]"

            # Extra processing required for grib_handle members that are referenced
            if self._transforms.ctype_of(cstruct_arg.name) == "grib_handle*" or cstruct_arg.name == "grib_handle_of_accessor(a)":
                if cstruct_member.name == "buffer":

                    debug.line("transform_cstruct_arg", f"buffer: cstruct_arg=[{cstruct_arg.as_string()}]")

                    cppstruct_arg = struct_arg.StructArg("", "buffer_", "" )
                    if cstruct_member.member and cstruct_member.member.name == "data":
                        cppstruct_arg.member = struct_arg.StructArg(".", "data()", cstruct_member.member.index)

            if cppstruct_arg:
                return cppstruct_arg

        # Not accessing a member, delegate to super()
        return super().transform_cstruct_arg(cstruct_arg)

    # Overridden to update private member calls
    def custom_cfunction_updates(self, line):
        m = re.search(rf"\b(\w+)\s*\(\s*([^,]+),", line)
        if m:
            for mapping in self._transforms.private_funcsig_mappings:
                if m.group(1) == mapping.cfuncsig.name:
                    line = re.sub(m.re, f"{mapping.cppfuncsig.name}(", line)
                    debug.line("custom_cfunction_updates", f"private_methods [after]: {line}")

        return super().custom_cfunction_updates(line)

    # Convert grib_(un)pack_TYPE calls to the equivalent member function
    # Note: We don't convert the vars here, but we do remove those not needed in the C++ function
    #       We also add a call to explicitly set the size var as it may be used after this call...
    def convert_grib_un_pack_functions(self, line):
        m = re.search(rf"\bgrib_((?:un)?pack_\w+)\((.*)\)", line)
        if m:
            mapping = self._transforms.funcsig_mapping_for(m.group(1))
            if mapping and mapping.arg_indexes and mapping.cppfuncsig:
                buffer_arg_index = mapping.arg_indexes.cbuffer
                buffer_len_arg_index = mapping.arg_indexes.clength

                if buffer_arg_index and buffer_len_arg_index:
                    func_name = mapping.cppfuncsig.name
                    vars = m.group(2).split(",")
                    line = re.sub(m.re, rf"{func_name}({vars[buffer_arg_index]}); {vars[buffer_len_arg_index]} = {vars[buffer_arg_index]}.size()", line)
                    debug.line("convert_grib_un_pack_functions", f"Converted grib_{m.group(1)} function: [after ]: {line}")

        return line
    
    # Overridden to apply member function substitutions
    #
    # Note: Group 2 matches self-> and super->
    def update_cfunction_names(self, line):
     
        line = self.convert_grib_un_pack_functions(line)

        # Note: (?=\() ensures group(3) is followed by a "(" without capturing it - ensuring it's a function name!
        m = re.search(rf"(&)?\b(\w+->)?(\w+)(?=\()", line)
        if m:
            prefix = m.group(1) if m.group(1) is not None else ""
            struct_name = m.group(2)
            cfunction_name = m.group(3)

            if struct_name in ["self->", "super->"]:
                # Must be calling an inherited function
                mapping = inherited_method_funcsig_conv.cpp_inherited_method_mapping_for(cfunction_name)
                assert mapping, f"Expected virtual function call, but could not find mapping for function [{cfunction_name}]"
                if struct_name == "super->":
                    struct_name = self._transforms.types['super'] + "::"
                else:
                    struct_name = ""
                line = re.sub(m.re, rf"{prefix}{struct_name}{mapping.cppfuncsig.name}", line)
                debug.line("update_cfunction_names", f"Updating inherited class method {m.group(0)} [after ]: {line}")
                return line

        return super().update_cfunction_names(line)

    # Overridden to process private functions
    def transform_cfunction_name(self, cfunction_name):
        for mapping in self._transforms.private_funcsig_mappings:
            if cfunction_name == mapping.cfuncsig.name:
                return mapping.cppfuncsig.name
        
        return super().transform_cfunction_name(cfunction_name)

    # Overridden to handle member types e.g. AccessorName x = NULL
    def custom_transform_cppvariable_access(self, cppvariable, match_token, post_match_string):
        accessor_arg = None

        for cpparg in self._transforms.all_args.values():
            if cpparg and cpparg.name == cppvariable.name and cpparg.type == "AccessorPtr":
                accessor_arg = cpparg
                break

        if not accessor_arg:
            return None
        
        if match_token.is_assignment:
            m = re.match(r"\s*(NULL)", post_match_string)
            if m:
                post_match_string = re.sub(m.re, "nullptr", post_match_string)
                return cppvariable.as_string() + match_token.as_string() + post_match_string

        return None
