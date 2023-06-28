def patch(klass):
    # Redefined at this level :-(
    klass.mark_mutable("dirty_")

