
from func_conv import *
import method

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

    # Extra processing required for grib_handle members that are referenced
    def update_grib_handle_members(self, line):

        for k in self._transforms.all_args.keys():
            if k.type == "grib_handle*":
                m = re.search(rf"\b{k.name}->buffer", line)
                if m:
                    line = re.sub(rf"{m.group(0)}->data", "buffer_.data()", line)
                    debug.line("update_grib_handle_members", f"Updated buffer ref [after ]: {line}")     

        return line

    def update_class_members(self, line):
        line,count = re.subn(r"\bsuper->\b", f"{self._transforms.class_types['super'].cppvalue}::", line)
        if count:
            debug.line("update_class_members", f"begin [after ]: {line}")

        accessor_variable_name = arg_conv.transform_variable_name(self._transforms.class_types["self"].cppvalue)

        # Replace name member with a string literal (it's only used in logging)
        line,count = re.subn(r"\ba->name", f"\"{self._transforms.class_types['super'].cppvalue}\"", line)
        if count:
            debug.line("update_class_members", f"Changed a->name to string literal [after ]: {line}")

        for n in [r"\bself\b", r"\ba\b"]:
            line,count = re.subn(n, f"{accessor_variable_name}", line)
            if count:
                debug.line("update_class_members", f"this [after ]: {line}")

        if re.match(rf".*\b{accessor_variable_name}\s+=", line):
            debug.line("update_class_members", f"deleting: {line}")
            line = ""

        if re.match(rf".*\bsuper\s+=\s+\*\({accessor_variable_name}->cclass->super\)", line):
            debug.line("update_class_members", f"deleting: {line}")
            line = ""

        for k,v in base_members_map.items():
            line,count = re.subn(rf"\b{accessor_variable_name}->{k.name}\b", rf"{v.name}", line)
            if(count):
                debug.line("update_class_members", f"base_members_map [after ]: {line}")

        # Process all members in the hierarchy
        m = re.search(rf"\b{accessor_variable_name}->(\w+)\b", line)
        if m:
            for k,v in self._transforms.members.items():
                assert v is not None, f"v is None for k={arg.arg_string(k)}"
                if m.group(1) == k.name:
                    line = re.sub(m.re, rf"{v.name}", line)
                    debug.line("update_class_members", f"members_in_hierarchy [after ]: {line}")

        m = re.search(rf"\b(\w+)\s*\(\s*([^,]+),", line)
        if m:
            for mapping in self._transforms.private_funcsig_mappings:
                if m.group(1) == mapping.cfuncsig.name:
                    line = re.sub(m.re, f"{accessor_variable_name}.{mapping.cfuncsig.name}(", line)
                    debug.line("update_class_members", f"private_methods [after ]: {line}")

        return line


    # Override this to provide any initial conversions before the main update_line runs
    def process_variables_initial_pass(self, line):
        line = self.update_class_members(line)
        line = self.update_grib_handle_members(line)
        return super().process_variables_initial_pass(line)

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
    def apply_function_transforms(self, line):
     
        line = self.convert_grib_un_pack_functions(line)

        m = re.search(rf"(?<!\")(&)?\b(\w+)\b(?!\")", line)
        if m:
            for mapping in self._transforms.inherited_funcsig_mappings:
                if m.group(2) == mapping.cfuncsig.name:
                    prefix = m.group(1) if m.group(1) is not None else ""
                    line = re.sub(m.re, rf"{prefix}{mapping.cppfuncsig.name}", line)
                    debug.line("apply_function_transforms", f"Updating inherited method {m.group(0)} [after ]: {line}")

            for mapping in self._transforms.private_funcsig_mappings:
                if m.group(2) == mapping.cfuncsig.name:
                    prefix = m.group(1) if m.group(1) is not None else ""
                    line = re.sub(m.re, rf"{prefix}{mapping.cppfuncsig.name}", line)
                    debug.line("apply_function_transforms", f"Updating private method {m.group(0)} [after ]: {line}")

        return super().apply_function_transforms(line)

