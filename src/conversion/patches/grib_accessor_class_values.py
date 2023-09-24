def patch(klass):
    # It's actually redefined here :-(
    klass.mark_mutable("dirty_")
