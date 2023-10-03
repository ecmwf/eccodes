
func_pad = 30
debug_line_enabled = True
debug_filter_include = []
debug_filter_exclude = []

def debug_line(func, text):
    if not debug_line_enabled:
        return
    
    if debug_filter_include and not func in debug_filter_include:
        return
    
    if debug_filter_exclude and func in debug_filter_exclude:
        return
    
    if len(func) > func_pad:
        print(f">>>>>")
        print(f">>>>> PADDING ({func_pad}) TOO SMALL FOR FUNC NAME: {func} - size={len(func)}")
        print(f">>>>>")

    print(f"{func:{func_pad}}: {text}")
