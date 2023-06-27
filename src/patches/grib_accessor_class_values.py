def patch(klass):
    for m in klass._members:
        if m.name == "dirty_":
            m._mutable = True
