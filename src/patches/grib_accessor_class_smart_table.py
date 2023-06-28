def patch(klass):
    # Redefined :-(
    klass.mark_mutable("dirty_")
    klass.cannot_convert_method("init")


