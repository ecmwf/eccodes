def patch(klass):
    for m in klass._members:
        if m.name in ("numericValuesAccessor_", "packAccessor_"):
            m._mutable = True
