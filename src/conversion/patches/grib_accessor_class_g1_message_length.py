def patch(klass):
    klass.cannot_convert_method("unpack_long")
    klass.cannot_convert_method("pack_long")

