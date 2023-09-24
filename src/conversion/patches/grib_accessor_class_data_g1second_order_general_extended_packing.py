def patch(klass):
    klass.cannot_convert_method("destroy")
    klass.mark_mutable("size_")
    klass.mark_mutable("fvalues_")
    klass.mark_mutable("dvalues_")
    klass.mark_mutable("double_dirty_")
    klass.mark_mutable("float_dirty_")

