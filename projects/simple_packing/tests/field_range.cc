// Field ranges the optimisers cannot handle (minimum > maximum, non-finite or overflowing range,
// NaN, values beyond the float range) must be rejected with an exception: several of them made
// the parameter search loop forever. Each case runs in a child process with a time limit.

#include <cmath>
#include <csignal>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "simple_packing/optimizer/BPVOptimizer.h"
#include "simple_packing/optimizer/BinDecOptimizer.h"
#include "simple_packing/optimizer/DecOptimizer.h"
#include "simple_packing/simple/Binary.h"

static int failures = 0;

// Runs f in a child process: it must throw within 5 seconds
template <typename F>
static void expectError(const std::string& what, F&& f)
{
    const pid_t pid = fork();
    if (pid == 0) {
        alarm(5);
        try {
            f();
        }
        catch (const std::exception&) {
            _exit(0);
        }
        _exit(1);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return;
    }
    std::cerr << "FAIL: " << what << ": " << (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM ? "hangs" : WIFSIGNALED(status) ? "crashes"
                                                                                                                                 : "no error")
              << '\n';
    failures++;
}

int main()
{
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::nan("");

    struct Range
    {
        const char* name;
        double min, max;
    };
    for (const Range& r : { Range{ "minimum > maximum", 5.0, 1.0 }, Range{ "overflowing range", -1e308, 1e308 },
                            Range{ "negative beyond the float range", -1e50, -9.99505e49 }, Range{ "NaN", nan, 1.0 },
                            Range{ "infinity", 0.0, inf } }) {
        expectError(std::string(r.name) + " (DecOptimizer)", [&] {
            SimplePackingBinary<double> sp(std::make_unique<DecOptimizer>(16, 127));
            sp.getParameters(r.min, r.max);
        });
        expectError(std::string(r.name) + " (BinDecOptimizer)", [&] {
            SimplePackingBinary<double> sp(std::make_unique<BinDecOptimizer>(0, 1, 16));
            sp.getParameters(r.min, r.max);
        });
        expectError(std::string(r.name) + " (BPVOptimizer)", [&] {
            SimplePackingBinary<double> sp(std::make_unique<BPVOptimizer>(2));
            sp.getParameters(r.min, r.max);
        });
    }

    // The scale factor search directly
    expectError("get_binary_scale_factor with maximum < minimum", [&] {
        get_binary_scale_factor(1.0, 5.0, 24);
    });
    expectError("get_binary_scale_factor with an infinite range", [&] {
        get_binary_scale_factor(1e308, -1e308, 24);
    });

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
