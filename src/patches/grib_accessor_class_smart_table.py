def patch(klass):
    # Redefined :-(
    klass.mark_mutable("dirty_")
    klass.mark_mutable("tableCodesSize_")
    klass.mark_mutable("table_")
    klass.mark_mutable("tableCodes_")
    klass.cannot_convert_method("init")
    klass.cannot_convert_method("destroy")
