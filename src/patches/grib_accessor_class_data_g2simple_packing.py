def patch(klass):
    for m in klass._inherited_methods:
        if m.name == "pack_double":
            block = False
            n = 0
            lines = []
            for i, line in enumerate(m._lines):
                if "(super != grib_accessor_class_data_g2simple_packing)" in line:
                    lines.append("#ifdef CANNOT_CONVERT_CODE")
                    block = True

                lines.append(line)

                if block and "}" in line:
                    n += 1
                    if n == 2:
                        lines.append("#endif")
                        block = False

            m.update_lines(lines)
