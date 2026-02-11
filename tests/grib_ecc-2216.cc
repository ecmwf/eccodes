#include <unistd.h>
#include <omp.h>

#include "grib_api_internal.h"  // for GRIB_OMP_THREADS
#include "eccodes.h"

// The purpose of this test is to detect data races during accessor initialization.
// It only works when used with Valgrind or with the -fsanitize=thread compiler option.

void do_stuff(const char* input_file, int thread_num)
{
    FILE* in = fopen(input_file, "rb");
    grib_handle* source_handle = NULL;
    int err                    = 0;
    const void* buffer         = NULL;
    size_t size                = 0;

    while ((source_handle = grib_handle_new_from_file(0, in, &err)) != NULL) {
        GRIB_CHECK(grib_get_message(source_handle, &buffer, &size), 0);

        source_handle = grib_handle_new_from_message_copy(0, buffer, size);
        if (!source_handle) {
            fprintf(stderr, "Error creating handle from message\n");
            continue;
        }
        grib_handle_delete(source_handle);
    }
    fclose(in);

    fprintf(stderr, "Thread %d finished\n", thread_num);
}


int main(int argc, char** argv)
{

#if GRIB_OMP_THREADS
    if (argc != 3) {
        const char* prog = argv[0];
        fprintf(stderr, "Usage:\n\t%s [options] file numRuns\nOr\n\t%s [options] file numThreads\n", prog, prog);
        return 1;
    }

    const char* input_file = argv[1];
    size_t nThreads = atol(argv[2]);

#pragma omp parallel for schedule(static) num_threads(nThreads)
    for (size_t i = 0; i < nThreads; i++) {
        int thread_num = omp_get_thread_num();
        do_stuff(input_file, thread_num);
    }
#else
    fprintf(stderr, "This test requires GRIB_OMP_THREADS to be set to a non-zero value\n");
#endif

    return 0;
}
