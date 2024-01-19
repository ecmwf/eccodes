
import code_object.member_function as member_function
import utils.debug as debug
import re

class VirtualMemberFunction(member_function.MemberFunction):
    def __init__(self, funcsig, body, class_name="") -> None:
        super().__init__(funcsig, body, class_name)

    # Overridden to add "override"
    @property
    def funcsig_as_declaration(self):
        return super().funcsig_as_declaration + " override"
