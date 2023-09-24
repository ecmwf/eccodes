def patch(klass):
    klass.cannot_convert_top_level("unpack_string")
    klass.cannot_convert_top_level("pack_string")
    klass.cannot_convert_method("pack_string")
