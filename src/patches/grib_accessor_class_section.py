def patch(klass):
    klass.cannot_convert_method("init")
    klass.cannot_convert_method("destroy")
    klass.cannot_convert_method("update_size")
    klass.cannot_convert_method("next")
    klass.cannot_convert_method("byte_count")
    klass.cannot_convert_method("sub_section")
