def patch(klass):
    klass.cannot_convert_top_level("init")
    klass.cannot_convert_method("destroy")
    klass.cannot_convert_method("make_clone")
