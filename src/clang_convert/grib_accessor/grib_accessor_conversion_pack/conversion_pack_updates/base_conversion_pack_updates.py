
import utils.debug as debug

class BaseConversionPackUpdates:
    def __init__(self) -> None:
        self._update_funcs = {}

    # Use this entry point to call the appropriate derived function
    def apply_updates_for_cfunction(self, cfuncname, conversion_pack):

        self.apply_updates_for_all_functions()

        for name, update_func in self._update_funcs.items():
            if name == cfuncname:
                update_func(conversion_pack)
                break

    # Override to set the same update for all functions
    # NOTE: Doesn't apply to global definition
    def apply_updates_for_all_functions(self):
        pass