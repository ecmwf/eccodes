#!/usr/bin/env python3

import re
import glob
from collections import defaultdict


class Header:
    ''' Parse a header file and extract class name and base class name '''
    def __init__(self, filename):
        self.class_name = None
        self.base_class_name = None

        for line in open(filename):
            if m := re.match(r'\s*class (?P<name>[\w_]+) : public (?P<base>[\w_]+)', line):
                self.class_name = m.group('name')
                self.base_class_name = m.group('base')
                continue

            if m := re.match(r'\s*class (?P<name>[\w_]+)', line):
                self.class_name = m.group('name')
                self.base_class_name = None
                continue

class Hierarchy:
    ''' Build a hierarchy of classes '''
    def __init__(self):
        self.base_class_names = set()
        self.children = defaultdict(list)


    def add(self, h):
        if h.base_class_name is None and h.class_name is not None:
            self.base_class_names.add(h.class_name)
        self.children[h.base_class_name].append(h.class_name)


    def get_children_(self, base_class_name):
        children_list = self.children[base_class_name]
        if not children_list:
            return [[base_class_name]]
        else:
            result = []
            for child in children_list:
                children2 = self.get_children_(child)
                for child2 in children2:
                    result.append([base_class_name] + child2)
            return result


    def make_hierarchy(self, base_class_name):
        hierarchy = []
        children_ = self.children[base_class_name]
        if not children_:
            hierarchy.append([base_class_name])
        else:
            for child in children_:
                chains = self.get_children_(child)
                for chain in chains:
                    hierarchy.append([base_class_name] + chain)

        return hierarchy


    def print(self):
        for base_class_name in self.base_class_names:
            hierarchy = self.make_hierarchy(base_class_name)
            print("====================================")
            print(f"Hierarchy: {base_class_name}")
            print("====================================")
            for chain in hierarchy:
                print(" <- ".join(chain))



if __name__ == "__main__":
    dirs = [
        "src/accessor/*.h",
        "src/action/*.h",
        "src/expression/*.h",
        "src/dumper/*.h",
        "src/geo/iterator/*.h",
        "src/geo/nearest/*.h",
        "src/eccodes/accessor/*.h",
        "src/eccodes/action/*.h",
        "src/eccodes/expression/*.h",
        "src/eccodes/dumper/*.h",
        "src/eccodes/geo/iterator/*.h",
        "src/eccodes/geo/nearest/*.h",
    ]

    for dir in dirs:
        hierarchy = Hierarchy()
        files = glob.glob(dir)
        for fn in files:
            hierarchy.add(Header(fn))

        hierarchy.print()
