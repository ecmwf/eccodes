// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

#include "simple_packing/detail/optimizer/BPVOptimizer.h"
#include "simple_packing/detail/optimizer/BinDecOptimizer.h"
#include "simple_packing/detail/optimizer/DecOptimizer.h"

using namespace sp::detail;

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

// Runs f in a child process: it must return constant-field parameters that decode to 'value'
// within 5 seconds
template <typename F>
static void expectConstant(const std::string& what, double value, F&& f)
{
    const pid_t pid = fork();
    if (pid == 0) {
        alarm(5);
        try {
            const Parameters params = f();
            const double decoded = params.referenceValue() / std::pow(10.0, static_cast<double>(params.decimalScaleFactor()));
            const bool ok = params.bitsPerValue() == 0 && params.isConstantField()
                            && std::fabs(decoded - value) <= 1e-6 * value;
            _exit(ok ? 0 : 2);
        }
        catch (const std::exception&) {
            _exit(3);
        }
    }
    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return;
    }
    std::cerr << "FAIL: " << what << ": "
              << (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM ?
                      "hangs" :
                      WIFSIGNALED(status) ? "crashes" :
                      WEXITSTATUS(status) == 2 ? "wrong parameters" : "threw")
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

    // A constant field: the reference value must represent the field, not a default
    expectConstant("BinDecOptimizer: constant field", 5.0, [] {
        return BinDecOptimizer(0, 1, 16).getParams(5.0, 5.0);
    });
    expectConstant("DecOptimizer: constant field", 5.0, [] {
        return DecOptimizer(24, 127).getParams(5.0, 5.0);
    });

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
