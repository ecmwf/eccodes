def patch(klass):
    klass.mark_mutable("numericValuesAccessor_")
    klass.mark_mutable("packAccessor_")
