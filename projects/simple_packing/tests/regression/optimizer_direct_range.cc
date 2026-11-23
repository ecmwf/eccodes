// Calling an optimiser's getParams() directly with an out-of-range field (minimum > maximum,
// non-finite values) must be rejected with an exception: the parameter search looped forever
// for minimum > maximum and converted NaN or infinity to integers (undefined behaviour)
// Otherwise. SimplePacking::getParameters() validates on its own, the optimisers did not.
// Each case runs in a child process with a five-second time limit.

#include <cmath>
#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "simple_packing/optimizer/BPVOptimizer.h"
#include "simple_packing/optimizer/BinDecOptimizer.h"
#include "simple_packing/optimizer/DecOptimizer.h"

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
    std::cerr << "FAIL: " << what << ": "
              << (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM ?
                      "hangs" :
                      WIFSIGNALED(status) ? "crashes" : "no error")
              << '\n';
    failures++;
}

int main()
{
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::nan("");

    expectError("DecOptimizer: minimum > maximum", [] {
        DecOptimizer(24, 127).getParams(1.0, -1.0);
    });
    expectError("BinDecOptimizer: minimum > maximum", [] {
        BinDecOptimizer(0, 1, 16).getParams(2.0, -1.0);
    });
    expectError("BinDecOptimizer: NaN", [=] {
        BinDecOptimizer(0, 1, 16).getParams(nan, 1.0);
    });
    expectError("BPVOptimizer: minimum > maximum", [] {
        BPVOptimizer(2).getParams(1.0, -1.0);
    });
    expectError("BPVOptimizer: infinity", [=] {
        BPVOptimizer(2).getParams(0.0, inf);
    });

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
