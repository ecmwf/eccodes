
import utils.debug as debug

# Holds the conversion data, conversion validation and any other useful conversion objects

class ConversionPack:
    def __init__(self, conversion_data, conversion_validation) -> None:
        self._conversion_data = conversion_data
        self._conversion_validation = conversion_validation

    @property
    def conversion_data(self):
        return self._conversion_data

    @property
    def conversion_validation(self):
        return self._conversion_validation

    # Allow the validation to be updated (for example to provide function-specific validators)
    @conversion_validation.setter
    def conversion_validation(self, new_conversion_validation):
        self._conversion_validation = new_conversion_validation