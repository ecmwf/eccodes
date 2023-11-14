import arg

class Member(arg.Arg):
    def __init__(self, type, name="",*, mutable=False):
        super().__init__(type, name)
        self.default_value = ""
        self._mutable = mutable

    def as_declaration(self):
        if self._mutable:
            return "mutable " + super().as_declaration()
        else:
            return super().as_declaration()