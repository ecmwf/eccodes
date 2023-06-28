def patch(klass):
    klass.mark_mutable("lons_")
    klass.mark_mutable("size_")
    klass.mark_mutable("save_")
