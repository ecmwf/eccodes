# This is the default interface if a grib_accessor doesn't have any specifics!

class AccessorSpecific:
    def __init__(self) -> None:
        pass
            
    def update_converters(self, converters):
        return converters
    
