// Optimiser parameters outside their range must be rejected with std::invalid_argument when the
// optimiser is created: negative or huge bits per value and scale factors made the parameter
// search loop forever or for a very long time.

#include <csignal>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "simple_packing/optimizer/BPVOptimizer.h"
#include "simple_packing/optimizer/BinDecOptimizer.h"
#include "simple_packing/optimizer/Dec64Optimizer.h"
#include "simple_packing/optimizer/DecOptimizer.h"

static int failures = 0;

// Runs f in a child process: it must throw std::invalid_argument within 5 seconds
template <typename F>
static void expectInvalid(const std::string& what, F&& f)
{
    const pid_t pid = fork();
    if (pid == 0) {
        alarm(5);
        try {
            f();
        }
        catch (const std::invalid_argument&) {
            _exit(0);
        }
        catch (...) {
            _exit(2);
        }
        _exit(1);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return;
    }
    std::cerr << "FAIL: " << what << ": "
              << (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM ? "hangs" : WIFSIGNALED(status)    ? "crashes"
                                                                             : WEXITSTATUS(status) == 2 ? "other exception"
                                                                                                        : "accepted")
              << '\n';
    failures++;
}

int main()
{
    for (long bpv : { -1L, 0L, 64L, 65L, 100000000000L }) {
        const std::string b = std::to_string(bpv);
        expectInvalid("DecOptimizer with " + b + " bits per value", [&] { DecOptimizer(bpv, 127).getParams(-1, 1); });
        expectInvalid("Dec64Optimizer with " + b + " bits per value", [&] { Dec64Optimizer(bpv, 127).getParams(-1, 1); });
        expectInvalid("BinDecOptimizer with " + b + " bits per value", [&] { BinDecOptimizer(0, 1, static_cast<int>(bpv)).getParams(-1, 1); });
    }
    for (long last : { -1L, 0L, 100000L }) {
        expectInvalid("DecOptimizer with last " + std::to_string(last), [&] { DecOptimizer(16, last).getParams(-1, 1); });
    }
    for (int dsf : { -1000000, 1000000 }) {
        expectInvalid("BPVOptimizer with decimal scale factor " + std::to_string(dsf), [&] { BPVOptimizer(dsf).getParams(-1, 1); });
    }

    // Supported parameters
    try {
        DecOptimizer(63, 127).getParams(-1, 1);
        DecOptimizer(1, 99).getParams(-1, 1);
        BinDecOptimizer(1, 1, 63).getParams(0, 1);
        BinDecOptimizer(0, 1, 32).getParams(320.63377474930917, 581.49648981621397);
        BPVOptimizer(-10).getParams(-1e10, 1e10);
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: supported parameters: " << e.what() << '\n';
        failures++;
    }

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
