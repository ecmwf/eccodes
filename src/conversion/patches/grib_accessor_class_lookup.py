def patch(klass):
    klass.cannot_convert_method("post_init")
    klass.cannot_convert_method("notify_change")

