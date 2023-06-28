def patch(klass):
    klass.cannot_convert_method("destroy")
    klass.cannot_convert_top_level("dump")
    klass.cannot_convert_method("make_clone")
