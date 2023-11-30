#!/usr/bin/env python3

import debug
import arg

# Various test cases for the convert code

def test_arg():
    test_data = [
        ("int", "i"),
        ("const int", "i"),
        ("static int", "i"),
        ("static const int", "i"),
        ("int*", "pi"),
        ("int *", "pi"),
        ("const int*", "pi"),
        ("const int *", "pi"),
        ("int* const", "pi"),
        ("int *const", "pi"),
        ("const int* const", "pi"),
        ("const int *const", "pi"),
        ("const int* *const", "pi"),
        ("static const int* *const", "pi"),
        ("int", "i[10]"),
    ]
    debug.line("test_arg", f"[IN]")

    for entry in test_data:
        debug.line("test_arg", f"test data: [{entry[0]}] [{entry[1]}]")
        new_arg = arg.Arg(entry[0], entry[1])
        debug.line("test_arg", f"new_arg  : storage_class=[{new_arg.decl_spec.storage_class}] const_qualifier=[{new_arg.decl_spec.const_qualifier}] type=[{new_arg.decl_spec.type}] pointer=[{new_arg.decl_spec.pointer}] name=[{new_arg.name}]")
        debug.line("test_arg", f"new_arg  : as_string=[{new_arg.as_string()}]")

    debug.line("test_arg", f"[OUT]")

def main():
    test_arg()

if __name__ == "__main__":
    main()
