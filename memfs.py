#!/usr/bin/env python
from __future__ import print_function
import os
import re
import sys
import binascii

assert len(sys.argv) > 2

# For now exclude GRIB3 as it is still experimental
EXCLUDED = 'grib3'

dirs = [os.path.realpath(x) for x in sys.argv[1:-1]]
print(dirs)

FILES = {}
NAMES = []

# Binary to ASCII function. Different in Python 2 and 3
try:
    str(b'\x23\x20','ascii')
    ascii = lambda x: str(x, 'ascii')  # Python 3
except:
    ascii = lambda x: str(x)           # Python 2


# The last argument is the path of the generated C file
output_file_path = sys.argv[-1]
g = open(output_file_path, "w")

for directory in dirs:

    # print("---->", directory)
    dname = os.path.basename(directory)
    NAMES.append(dname)

    for dirpath, dirnames, files in os.walk(directory):
        if EXCLUDED in dirnames:
            print('Note: %s/%s will not be included.' % (dirpath,EXCLUDED))

        # Prune the walk by modifying the dirnames in-place
        dirnames[:] = [dirname for dirname in dirnames if dirname != EXCLUDED]
        for name in files:
            full = '%s/%s' % (dirpath, name)
            _, ext = os.path.splitext(full)
            if ext not in ['.def', '.table', '.tmpl']:
                continue

            fname = full[full.find("/%s/" % (dname,)):]
            #print("MEMFS add", fname)
            name = re.sub(r'\W', '_', fname)

            assert name not in FILES
            FILES[name] = fname

            print('static const unsigned char %s[] = {' % (name,), file=g)

            with open(full, 'rb') as f:
                i = 0
                #Python 2
                #fcont = f.read().encode("hex")

                #Python 2 and 3
                fcont = binascii.hexlify(f.read())

                # Read two characters at a time and convert to C hex
                # e.g. 23 -> 0x23
                for n in range(0, len(fcont), 2):
                    twoChars = ascii(fcont[n:n+2])
                    print("0x%s," % (twoChars,), end="", file=g)
                    i += 1
                    if (i % 20) == 0:
                        print("", file=g)

            print('};', file=g)

print("""
#include "eccodes_config.h"
#ifdef ECCODES_HAVE_FMEMOPEN
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct entry {
    const char* path;
    const unsigned char* content;
    size_t length;
} entries[] = { """, file=g)

items = [(v, k) for k, v in FILES.items()]

for k, v in sorted(items):
    print ('{"/MEMFS%s", &%s[0], sizeof(%s) / sizeof(%s[0]) },' % (k, v, v, v), file=g)


print("""};

#if defined(ECCODES_HAVE_FUNOPEN) && !defined(ECCODES_HAVE_FMEMOPEN)

typedef struct mem_file {
    const char* buffer;
    size_t size;
    size_t pos;
} mem_file;

static int read_mem(void *data, char * buf, int len) {
    mem_file* f = (mem_file*)data;
    int n = len;

    if(f->pos + n > f->size) {
        n = f->size - f->pos;
    }

    memcpy(buf, f->buffer + f->pos, n);

    f->pos += n;
    return n;
}

static int write_mem(void* data, const char* buf, int len) {
    mem_file* f = (mem_file*)data;
    return -1;
}

static fpos_t seek_mem(void *data, fpos_t pos, int whence) {
    mem_file* f = (mem_file*)data;
    long newpos = 0;

    switch (whence) {
    case SEEK_SET:
        newpos = (long)pos;
        break;

    case SEEK_CUR:
        newpos = (long)f->pos + (long)pos;
        break;

    case SEEK_END:
        newpos = (long)f->size -  (long)pos;
        break;

    default:
        return -1;
        break;
  }

  if(newpos < 0) { newpos = 0; }
  if(newpos > f->size) { newpos = f->size; }

  f->pos = newpos;
  return newpos;
}

static int close_mem(void *data) {
    mem_file* f = (mem_file*)data;
    free(f);
    return 0;
}

static FILE* fmemopen(const char* buffer, size_t size, const char* mode){
    mem_file* f = (mem_file*)calloc(sizeof(mem_file), 1);
    if(!f) return NULL;

    f->buffer = buffer;
    f->size = size;

    return funopen(f, &read_mem, &write_mem, &seek_mem, &close_mem);
}

#endif

static size_t entries_count = sizeof(entries)/sizeof(entries[0]);

static const unsigned char* find(const char* path, size_t* length) {
    size_t i;

    for(i = 0; i < entries_count; i++) {
        if(strcmp(path, entries[i].path) == 0) {
            /*printf("Found in MEMFS %s\\n", path);*/
            *length = entries[i].length;
            return entries[i].content;
        }
    }

    return NULL;
}

int codes_memfs_exists(const char* path) {
    size_t dummy;
    return find(path, &dummy) != NULL;
}

FILE* codes_memfs_open(const char* path) {
    size_t size;
    const unsigned char* mem = find(path, &size);
    if(!mem) {
        return NULL;
    }
    return fmemopen((void*)mem, size, "r");
}

""", file=g)
print ('Created ',output_file_path)
