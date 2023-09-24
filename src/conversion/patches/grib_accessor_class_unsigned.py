def patch(klass):
    # Global function
    klass.cannot_convert_top_level("unpack_long")
    klass.cannot_convert_method("destroy")
    klass.cannot_convert_method("init")
