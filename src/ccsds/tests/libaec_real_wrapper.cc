// Wrapper to access the real libaec library functions via dlopen.
// This avoids symbol conflicts with our ccsds compatibility layer
// which exports the same function names.

#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>

// We include our own libaec.h since struct aec_stream is binary-compatible
#include "libaec.h"

static void *libaec_handle = nullptr;

typedef int (*aec_init_fn)(struct aec_stream *);
typedef int (*aec_process_fn)(struct aec_stream *, int);
typedef int (*aec_end_fn)(struct aec_stream *);

static aec_init_fn    fn_encode_init = nullptr;
static aec_process_fn fn_encode      = nullptr;
static aec_end_fn     fn_encode_end  = nullptr;
static aec_init_fn    fn_decode_init = nullptr;
static aec_process_fn fn_decode      = nullptr;
static aec_end_fn     fn_decode_end  = nullptr;

static void ensure_loaded() {
    if (libaec_handle) return;

    // Try the installed libaec shared library
    libaec_handle = dlopen("libaec.so", RTLD_NOW | RTLD_LOCAL);
    if (!libaec_handle) {
        // Try with full path from the known install location
        libaec_handle = dlopen("/home/joobog/install/libaec/lib/libaec.so", RTLD_NOW | RTLD_LOCAL);
    }
    if (!libaec_handle) {
        std::fprintf(stderr, "Failed to load real libaec: %s\n", dlerror());
        std::abort();
    }

    fn_encode_init = (aec_init_fn)dlsym(libaec_handle, "aec_encode_init");
    fn_encode      = (aec_process_fn)dlsym(libaec_handle, "aec_encode");
    fn_encode_end  = (aec_end_fn)dlsym(libaec_handle, "aec_encode_end");
    fn_decode_init = (aec_init_fn)dlsym(libaec_handle, "aec_decode_init");
    fn_decode      = (aec_process_fn)dlsym(libaec_handle, "aec_decode");
    fn_decode_end  = (aec_end_fn)dlsym(libaec_handle, "aec_decode_end");

    if (!fn_encode_init || !fn_encode || !fn_encode_end ||
        !fn_decode_init || !fn_decode || !fn_decode_end) {
        std::fprintf(stderr, "Failed to resolve libaec symbols: %s\n", dlerror());
        std::abort();
    }
}

extern "C" {

int real_aec_encode_init(struct aec_stream *strm) {
    ensure_loaded();
    return fn_encode_init(strm);
}

int real_aec_encode(struct aec_stream *strm, int flush) {
    ensure_loaded();
    return fn_encode(strm, flush);
}

int real_aec_encode_end(struct aec_stream *strm) {
    ensure_loaded();
    return fn_encode_end(strm);
}

int real_aec_decode_init(struct aec_stream *strm) {
    ensure_loaded();
    return fn_decode_init(strm);
}

int real_aec_decode(struct aec_stream *strm, int flush) {
    ensure_loaded();
    return fn_decode(strm, flush);
}

int real_aec_decode_end(struct aec_stream *strm) {
    ensure_loaded();
    return fn_decode_end(strm);
}

} // extern "C"
