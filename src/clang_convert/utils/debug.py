
from datetime import datetime
from code_object.code_interface import CodeInterface

func_pad = 30
debug_enabled = True
debug_filter_include = [] #["convert_global_declarations", "dump_function"]
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
    
    global start_time 
    elapsed_time_str = ""
    if show_time:
        current_time = datetime.now()
        elapsed_time = current_time - start_time
        elapsed_time_str = f",{elapsed_time.microseconds},"

    # List support
    if isinstance(text, list):
        if len(text) == 0:
            print(f"{func:{func_pad}}:{elapsed_time_str} *** ERROR - CAN'T PRINT DEBUG TEXT, SUPPLIED LIST IS EMPTY ***")
            return

        print(f"{func:{func_pad}}:{elapsed_time_str} {text[0]}")

        func = ""
        for line in text[1:]:
            print(f"{func:{func_pad}}  {line}")
    else:
        print(f"{func:{func_pad}}:{elapsed_time_str} {text}")

# Safe way to print a value even if it is None - useful for
# CodeInstance classes etc
def as_debug_string(value):
    if value is not None:
        if isinstance(value, str):
            return value
        elif isinstance(value, CodeInterface):
            return value.as_string()
        else:
            return f"[{value}]"
    else:
        return "None"
