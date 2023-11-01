
from datetime import datetime

func_pad = 40
debug_enabled = False
debug_filter_include = [] #["to_accessor_data"]
debug_filter_exclude = []

show_time = False
start_time = datetime.now()

def enable_debug():
    global debug_enabled
    debug_enabled = True

def disable_debug():
    global debug_enabled
    debug_enabled = False

def reset_timer():
    global start_time 
    start_time = datetime.now()

def line(func, text):
    if not debug_enabled:
        return
    
    if debug_filter_include and not func in debug_filter_include:
        return
    
    if debug_filter_exclude and func in debug_filter_exclude:
        return
    
    if len(func) > func_pad:
        print(f">>>>>")
        print(f">>>>> PADDING ({func_pad}) TOO SMALL FOR FUNC NAME: {func} - size={len(func)}")
        print(f">>>>>")

    global start_time 
    elapsed_time_str = ""
    if show_time:
        current_time = datetime.now()
        elapsed_time = current_time - start_time
        elapsed_time_str = f",{elapsed_time.microseconds},"

    # Multiline support
    if text[0] == "\n":
        print()
        text = text[1:]

    lines = text.split("\n")
   
    print(f"{func:{func_pad}}:{elapsed_time_str} {lines[0]}")

    func = ""
    for line in lines[1:]:
        print(f"{func:{func_pad}}  {line}")
