// The thread pool of the parallel decoder must have one thread per chunk (never more than
// there are RSIs): an oversized pool (nThreads up to 255) used to be created for the whole
// decoder, and many concurrent decodes could exhaust the process/thread limit of the
// system. A watcher thread samples the thread count of the process while decoding.

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include <exception>
#include "ccsds/CodeOption.h"
#include "ccsds/Span.h"
#include "ccsds/SpAec.h"

static int failures = 0;

// Peak number of threads of this process while the watcher runs
static std::atomic<size_t> peakThreads{ 0 };

static void watcherLoop(const std::atomic<bool>* stop)
{
    while (!stop->load()) {
        FILE* f = fopen("/proc/self/status", "r");
        if (f) {
            char line[256];
            while (fgets(line, sizeof(line), f)) {
                if (strncmp(line, "Threads:", 8) == 0) {
                    const size_t n = static_cast<size_t>(strtol(line + 8, nullptr, 10));
                    size_t seen = peakThreads.load();
                    while (seen < n && !peakThreads.compare_exchange_weak(seen, n)) {
                    }
                    break;
                }
            }
            fclose(f);
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

static void decodeMany(const char* what, uint8_t bpv, uint8_t blockSize, uint16_t rsi,
                       size_t nValues, uint8_t nThreads, size_t repetitions, size_t maxPeak)
{
    // One RSI of zeros
    std::vector<double> data(nValues, 0.0);
    encoder::SpAec<double> enc{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0 };
    std::vector<uint8_t> encoded(enc.maxEncodedSize(nValues, bpv, blockSize));
    const size_t size = enc.encode(Span<double>(data), Span<uint8_t>(encoded));

    peakThreads = 0;
    std::atomic<bool> stop{ false };
    std::thread watcher(watcherLoop, &stop);
    {
        encoder::SpAec<double> dec{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0, 0, 0, nThreads };
        std::vector<double> out(nValues, -1.0);
        for (size_t i = 0; i < repetitions; ++i) {
            try {
                std::fill(out.begin(), out.end(), -1.0);
                dec.decode(Span<uint8_t>(encoded.data(), size), Span<double>(out));
                for (size_t j = 0; j < nValues; ++j) {
                    if (out[j] != 0.0) {
                        std::cerr << "FAIL: " << what << ": value " << j << " is " << out[j] << '\n';
                        failures++;
                        break;
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "FAIL: " << what << ": " << e.what() << '\n';
                failures++;
                break;
            }
        }
    }
    stop = true;
    watcher.join();

    if (peakThreads > maxPeak) {
        std::cerr << "FAIL: " << what << ": peak thread count " << peakThreads << " exceeds " << maxPeak << '\n';
        failures++;
    }
    else {
        std::cout << "OK: " << what << ": peak threads " << peakThreads << '\n';
    }
}

int main()
{
    try {
        // 1 RSI: the pool must have a single worker although 255 are requested
        decodeMany("1 RSI, 255 threads", 8, 8, 2, 16, 255, 3000, 8);
        // 4 RSIs, 255 threads: 4 workers
        decodeMany("4 RSIs, 255 threads", 8, 8, 2, 64, 255, 2000, 16);
        // 2 RSIs, 8 threads: 2 workers
        decodeMany("2 RSIs, 8 threads", 8, 8, 2, 32, 8, 3000, 8);
        // fewer values than one RSI: still a single worker
        decodeMany("partial RSI, 255 threads", 8, 8, 2, 9, 255, 3000, 8);

        if (failures == 0) {
            std::cout << "OK\n";
        }
        return failures == 0 ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
}
