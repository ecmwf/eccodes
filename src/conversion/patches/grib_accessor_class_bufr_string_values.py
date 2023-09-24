def patch(klass):
    klass.mark_mutable("dataAccessor_")
    klass.cannot_convert_method('unpack_string_array')

