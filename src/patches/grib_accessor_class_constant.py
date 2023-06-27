def patch(klass):
    klass._top_level_code["init"] = (
        ["#ifdef CANNOT_CONVERT_CODE"] + klass._top_level_code["init"] + ["#endif"]
    )
