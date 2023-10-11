import arg

class Member(arg.Arg):
    def __init__(self, type, name=""):
        super().__init__(type, name)
        self.default_value = ""
        self._mutable = False
