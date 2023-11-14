
from func_conv import *
import method
import inherited_method_funcsig_conv
import private_method_funcsig_conv

# Specialisation of FunctionConverter for AccessorData Methods
class MethodConverter(FunctionConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return method.Method(cppfuncsig, self._transforms.types["self"])

    # overridden to short-circuit well-knwon transforms!
    def convert_cfunction_calls(self, line):

        # Transform the argument getters
        m = re.search(r"((self->)?(\w+)\s+=\s+)?\bgrib_arguments_get_(\w+)\([^,]+, ([^,]+), ([^\)]+)\)", line)
        if m:
            ctype = assignment_text = ""
            if m.group(1): 
                assignment_text = m.group(1)
                member_name = m.group(3)
                for cmember, cppmember in self._transforms.members.items():
                    if cmember.name == member_name:
                        ctype = cppmember.type
                        break

            get_what = m.group(4)
            if get_what == "expression":
                get_what = "GribExpressionPtr"
            elif get_what in ["string", "name"]:
                get_what = "std::string"
                if not ctype:
                    ctype = "AccessorName"

            if m.group(5).startswith("self->"):
                grib_arguments_arg_name = m.group(5)
            else:
                grib_arguments_arg_name = "initData"

            if ctype == "AccessorName":
                line = re.sub(m.re, f"{assignment_text}AccessorName(std::get<{get_what}>({grib_arguments_arg_name}.args[{m.group(6)}].second))", line)
            else:
                line = re.sub(m.re, f"{assignment_text}std::get<{get_what}>({grib_arguments_arg_name}.args[{m.group(6)}].second)", line)
            
            debug.line("convert_cfunction_calls", f"Updated [bgrib_arguments_get_expression] line=[{line}]")

        return super().convert_cfunction_calls(line)

    # WARNING: CAN CAUSE CONFLICTS WITH e.g. "self->owner" member and "owner" local variable
    # The former is detected via self-> struct access instead
    # We only validate vars that end in "_" are indeed members, or return None
    def cppvariable_for(self, var):

        if var.name.endswith("_"):
            for cppmember in self._transforms.members.values():
                if cppmember and var.name == cppmember.name:
                        cppvariable = variable.Variable("", cppmember.name, var.index)
                        cppvariable.type = cppmember.type
                        return cppvariable

        return super().cppvariable_for(var)

    # Overridden to check if we're assigning to a member var and therefore need to remove const from
    # the function signature
    def update_cppstruct_arg_assignment(self, cppstruct_arg, remainder):

        if self._cppfunction.const:
            for cppmember in self._transforms.members.values():
                if cppmember.name == cppstruct_arg.name:

                    if cppmember._mutable:
                        debug.line("update_cppstruct_arg_assignment", f"Assignment to mutable member [{cppmember.name}] in const function, so leaving const")
                    else:
                        self._cppfunction.const = ""
                        debug.line("update_cppstruct_arg_assignment", f"Assignment to member [{cppmember.name}] in const function, so removing constness")

        return super().update_cppstruct_arg_assignment(cppstruct_arg, remainder)

    # transform_cstruct_arg helpers - return cppstruct_arg or None
    def transform_cstruct_arg_member(self, cstruct_arg):
        cppstruct_arg = None
        cstruct_member = cstruct_arg.member

        if cstruct_arg.name in ["super", "self", "a"]:
            cppstruct_member = None

            # Find member arg
            for cmember, cppmember in self._transforms.members.items():
                if cmember.name == cstruct_member.name:
                    cppstruct_member = self.apply_cstruct_arg_transforms_for_ctype(cmember.type, cstruct_member, cppmember.name)
                    if not cppstruct_member:
                        cppstruct_member = struct_arg.StructArg("", cppmember.name, cstruct_member.index)
                        if cstruct_member.member:
                            if cstruct_member.name == "vvalue":
                                # This is a direct mapping
                                cppstruct_member.member = cstruct_member.member
                            elif cppmember and cppmember.type == "AccessorInitData":
                                # This is a direct mapping
                                cppstruct_member.member = cstruct_member.member
                            else:
                                # TODO: Additonal members here means that we've not processed something correctly - need to fix!
                                cppstruct_member_member = self.apply_default_cstruct_arg_transform(cstruct_member.member)
                                cppstruct_member.member = cppstruct_member_member
                                debug.line("transform_cstruct_arg_member", f"WARNING: Unexpected member, so not processed correctly: {cstruct_member.member.as_string()}")
                        break

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

        debug.line("transform_cstruct_arg_member", f"[{cstruct_arg.as_string()}] -> [{cppstruct_arg.as_string() if cppstruct_arg else None}]")

        return cppstruct_arg

    # transform_cstruct_arg helpers - return cppstruct_arg or None
    # Tidy up any stucts that have been updated to call a member function 
    # (for example remove a-> from a->unpack(...))
    def transform_cstruct_arg_member_functions(self, cstruct_arg):
        cppstruct_arg = None
        cstruct_member = cstruct_arg.member

        if cstruct_arg.name in ["super", "self", "a"]:
            # Find member functions
            conversions_list = self.method_conversions_list()

            for conversions in conversions_list:
                for mapping in conversions:
                    if mapping.cppfuncsig.name == cstruct_member.name:
                            cppstruct_arg = struct_arg.StructArg("", cstruct_member.name, cstruct_member.index)
                            break

        debug.line("transform_cstruct_arg_member_functions", f"[{cstruct_arg.as_string()}] -> [{cppstruct_arg.as_string() if cppstruct_arg else None}]")

        return cppstruct_arg

    # transform any a->foo where foo is a non-member
    def transform_cstruct_arg_a_nonmember(self, cstruct_arg):
        cppstruct_arg = None

        if cstruct_arg.name == "a":
            a_member = cstruct_arg.member
            if a_member.name == "name":
                # Special handling: Replace name member with a string literal (it's only used in logging)
                cppstruct_arg = struct_arg.StructArg("",f"\"{self._transforms.types['self']}\"", "")
            elif a_member.name == "cclass":
                if a_member.member and a_member.member.name == "name":
                    # Special handling: Replace name member with a string literal (it's only used in logging)
                    cppstruct_arg = struct_arg.StructArg("",f"\"{self._transforms.types['self']}\"", "")
            elif a_member.name == "context":
                context_member = a_member.member
                if context_member and context_member.name == "debug":
                    # Special handling: Replace with AccessorDebug flag
                    cppstruct_arg = struct_arg.StructArg("", "ACCESSOR_DEBUG", "")

            if not cppstruct_arg:
                # Set name to None to mark it for deletion
                debug.line("transform_cstruct_arg_a_nonmember", f"Marking for deletion: {cstruct_arg.as_string()}")
                cppstruct_arg = struct_arg.StructArg("", None, "")

        return cppstruct_arg

    def transform_cstruct_arg_grib_handle_member(self, cstruct_arg):
        cppstruct_arg = None
        cstruct_member = cstruct_arg.member

        if self._transforms.ctype_of(cstruct_arg.name) == "grib_handle*" or cstruct_arg.name == "grib_handle_of_accessor(a)":
            if cstruct_member.name == "buffer":
                debug.line("transform_cstruct_arg_grib_handle_member", f"buffer: cstruct_arg=[{cstruct_arg.as_string()}]")
                cppstruct_arg = struct_arg.StructArg("", "buffer_", "" )
                if cstruct_member.member and cstruct_member.member.name == "data":
                    cppstruct_arg.member = struct_arg.StructArg(".", "data()", cstruct_member.member.index)
            elif cstruct_member.name == "loader":
                debug.line("transform_cstruct_arg_grib_handle_member", f"loader: cstruct_arg=[{cstruct_arg.as_string()}]")
                cppstruct_arg = struct_arg.StructArg("", "loader()", "" )

        return cppstruct_arg

    def transform_cstruct_arg_accessor_ptr(self, cstruct_arg):
        cppstruct_arg = None

        # If AccessorPtr, keep access as "->"
        cpparg = self._transforms.cpparg_for_cname(cstruct_arg.name)
        if cpparg and cpparg.type == "AccessorPtr":
            if cstruct_arg.member.name == "name":
                # Special handling: Replace name member with a string literal (it's only used in logging)
                cppstruct_arg = struct_arg.StructArg("", f"\"{self._transforms.types['self']}\"", "" )
            elif cstruct_arg.member.name == "parent":
                # Special handling: Replace parent member a call...
                cppstruct_arg = struct_arg.StructArg("", f"parentOf({cstruct_arg.name})", "" )
            else:
                cppstruct_arg = struct_arg.StructArg("", cpparg.name, cstruct_arg.index )
                cppstruct_arg.member = cstruct_arg.member

            debug.line("transform_cstruct_arg_accessor_ptr", f"[{cstruct_arg.as_string()}] -> [{cppstruct_arg.as_string()}]")


        return cppstruct_arg

    # Overridden to process self->, super-> etc
    def transform_cstruct_arg(self, cstruct_arg):
        assert cstruct_arg, f"Unexpected cstruct_arg with None value"

        transform_funcs = [
            self.transform_cstruct_arg_member,
            self.transform_cstruct_arg_member_functions,
            self.transform_cstruct_arg_a_nonmember,
            self.transform_cstruct_arg_grib_handle_member,
            self.transform_cstruct_arg_accessor_ptr,
        ]

        cppstruct_arg = None

        for transform_func in transform_funcs:
            cppstruct_arg = transform_func(cstruct_arg)            

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
    def custom_transform_cppvariable_access(self, cppvariable, original_var, match_token, post_match_string):

        access_arg = None

        for cpparg_list in [self._transforms.all_args.values(),
                            self._transforms.members.values()]:
            for cpparg in cpparg_list:
                if cpparg and cpparg.name == cppvariable.name:
                    access_arg = cpparg
                    break

        if not access_arg:
            return super().custom_transform_cppvariable_access(cppvariable, original_var, match_token, post_match_string)

        if match_token.is_assignment:
            if access_arg.type == "AccessorPtr":
                m = re.match(r"\s*(NULL)", post_match_string)
                if m:
                    post_match_string = re.sub(m.re, "nullptr", post_match_string)
                    return cppvariable.as_string() + match_token.as_string() + post_match_string
        # Check for && or ||
        elif match_token.is_boolean_test:
            if access_arg.type == "AccessorName":
                return cppvariable.as_string() + ".get().size()" + match_token.as_string() + post_match_string

        return super().custom_transform_cppvariable_access(cppvariable, original_var, match_token, post_match_string)

    # Find any overloaded virtual function calls that are not implemented in this class, and add to the 
    # "using" list so we can add a "using base::func" directive in the header
    def update_using_list(self, line):
        m = re.search(r"\b([^\s\(]*pack[^\s\<\(]*)\(([^,\(]*)[,\)]", line)
        if m:
            funcname = m.group(1)

            # Check it is a virtual function first!
            is_virtual_function = False
            for mapping in inherited_method_funcsig_conv.InheritedMethodFuncSigConverter.inherited_method_conversions:
                if mapping.cppfuncsig.name == funcname:
                    is_virtual_function = True
                    break

            if is_virtual_function:
                if funcname not in self._cppfunction.using:
                    arg_type = self._transforms.cpptype_of(m.group(2))

                    for mappings in [self._transforms.inherited_funcsig_mappings,
                                    self._transforms.private_funcsig_mappings]:
                        for mapping in mappings:
                            if mapping.cppfuncsig.name == funcname:
                                if mapping.cppfuncsig.template:
                                    # Ignore templates - special case!
                                    return line
                                for arg_entry in mapping.cppfuncsig.args:
                                    if arg_entry and arg_entry.type == arg_type:
                                        # Correct override already implemented
                                        return line

                    self._cppfunction.using.append(funcname)
                    debug.line("update_using_list", f"Added [{funcname}] to using list to support type [{arg_type}]")

        return line

    # Override to check we can "see" all function calls...
    def final_updates(self, line):
        line = self.update_using_list(line)

        return super().final_updates(line)

    # Overridden for inherited methods
    def transform_cfunction_call(self, cfuncname, cparams):

        # Strip VAR-> and grib_ from funcname so we match e.g. super->unpack as unpack and grib_value_count as value_count
        grib_re   = r"(grib_)?"
        struct_re = r"([^\>]+->)?"

        m = re.match(rf"{struct_re}{grib_re}(\w*)", cfuncname)
        if m:
            struct_prefix = m.group(1)
            cmethod_name = m.group(3)
        else:
            struct_prefix = ""
            cmethod_name = cfuncname

        debug.line("transform_cfunction_call", f"cfuncname=[{cfuncname}] cmethod_name=[{cmethod_name}]")

        cppfuncname = transformed_cparams = None
        
        conversions_list = self.method_conversions_list()

        for conversions in conversions_list:
            cppfuncname, transformed_cparams = self.transform_cfunction_call_from_conversions(cmethod_name, cparams, conversions)
            if cppfuncname:
                # Put back the super-> etc prefix (it will be stripped later)
                if struct_prefix:
                    cppfuncname = struct_prefix + cppfuncname
                break

        debug.line("transform_cfunction_call", f"[METHOD] cppfuncname=[{cppfuncname}] cfuncname=[{cfuncname}] cmethod_name=[{cmethod_name}]")

        if cppfuncname:
            return cppfuncname, transformed_cparams

        return super().transform_cfunction_call(cfuncname, cparams)

    # Overridden to handle AccessorDataBuffer...
    def transform_cpp_container_non_assignment(self, cpp_container_arg, original_var, match_token, post_match_string):
        if cpp_container_arg.underlying_type == "AccessorDataBuffer":
            if match_token.value == ";" or match_token.is_maths_operator:
                transformed_line = cpp_container_arg.name + ".data()" + match_token.as_string() + post_match_string
                debug.line("transform_cpp_container_non_assignment", f"[METHOD] updated AccessorDataBuffer access for [{cpp_container_arg.name}]: Transformed line = [{transformed_line}]")
                return transformed_line
        
        return super().transform_cpp_container_non_assignment(cpp_container_arg, original_var, match_token, post_match_string)

    # Overridden for container members
    def transform_container_cppvariable_access(self, cppvariable, original_var, match_token, post_match_string):
        cppcontainer_arg = None

        for cppmember in self._transforms.members.values():
            if cppmember.name == cppvariable.name and arg.is_container(cppmember):
                cppcontainer_arg = cppmember
                break

        if cppcontainer_arg:
            for transform_func in [self.transform_cpp_container_assignment,
                                   self.transform_cpp_container_non_assignment]:
                transformed_string = transform_func(cppcontainer_arg, original_var, match_token, post_match_string)
                if transformed_string:
                    return transformed_string
                
            return None

        return super().transform_container_cppvariable_access(cppvariable, original_var, match_token, post_match_string)

    # Overridden for member checks
    def process_boolean_test(self, line):
        m = re.search(r"\b(\w+)(\s*\(!?)([^\)]+)\)", line)

        if m and m.group(1) in ["if", "Assert"]:
            transformed_call = None

            arg_name = m.group(3)
            test_arg = None
            for cppmember in self._transforms.members.values():
                if cppmember.name == arg_name:
                    test_arg = cppmember
                    break

            if test_arg:
                if arg.is_container(test_arg):
                    container_func_call = self.container_func_call_for(test_arg, "size")
                    transformed_call = f"{test_arg.name}.{container_func_call}"
                elif test_arg.type == "AccessorName":
                    transformed_call = f"{test_arg.name}.hasValue()"
                elif test_arg.type == "AccessorInitData":
                    transformed_call = f"{test_arg.name}.args.size()"

            #elif "context->debug" in m.group(3):
            elif "context->" in m.group(3):
                transformed_call = "false"
                
            if transformed_call:
                line = re.sub(re.escape(m.group(3)), f"{transformed_call}", line)
                debug.line("process_boolean_test", f"Replaced [{m.group(0)}] with [{transformed_call}] line:[{line}]")
                return line
            
        return super().process_boolean_test(line)

    # Return a list of all pre-defined conversions
    def method_conversions_list(self):
        # Need to use the full inherited list in case the current method's local mappings don't support it!
        return [inherited_method_funcsig_conv.InheritedMethodFuncSigConverter.inherited_method_conversions,
                self._transforms.private_funcsig_mappings,
                self._transforms.static_funcsig_mappings]