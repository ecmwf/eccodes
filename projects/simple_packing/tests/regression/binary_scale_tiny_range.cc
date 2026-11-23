// get_binary_scale_factor() with a range so small that scaling it up overflows the double
// range hung forever (the scale search multiplied zs until it became inf and then kept
// halving inf). It must report the range as too small instead. Each case runs in a child
// process with a time limit.

#include <cmath>
#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "simple_packing/Utils.h"

static int failures = 0;

// Runs f in a child process: it must return nullopt within 5 seconds
template <typename F>
static void expectTooSmall(const std::string& what, F&& f)
{
    const pid_t pid = fork();
    if (pid == 0) {
        alarm(5);
        try {
            const bool tooSmall = !f().has_value();
            _exit(tooSmall ? 0 : 2);
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
                      WEXITSTATUS(status) == 2 ? "no nullopt" :
                      WEXITSTATUS(status) == 3 ? "threw" : "unknown")
              << '\n';
    failures++;
}

// Runs f in a child process: it must return the given scale factor within 5 seconds
template <typename F>
static void expectScale(const std::string& what, long expected, F&& f)
{
    const pid_t pid = fork();
    if (pid == 0) {
        alarm(5);
        try {
            const auto scale = f();
            _exit(scale.has_value() && scale.value() == expected ? 0 : 2);
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
                      WEXITSTATUS(status) == 2 ? "wrong scale factor" : "threw")
              << '\n';
    failures++;
}

int main()
{
    const double denorm = std::numeric_limits<double>::denorm_min();

    // These hung forever: the scale search multiplied zs until it overflowed to inf
    expectTooSmall("denormal range, 63 bits", [=] {
        return get_binary_scale_factor(denorm, 0.0, 63);
    });
    expectTooSmall("1e-320 range, 24 bits", [] {
        return get_binary_scale_factor(1e-320, 0.0, 24);
    });
    expectTooSmall("1e-300 range, 63 bits", [] {
        return get_binary_scale_factor(1e-300, 0.0, 63);
    });
    expectTooSmall("1e-280 range, 63 bits (too small without overflow)", [] {
        return get_binary_scale_factor(1e-280, 0.0, 63);
    });

    // The boundaries of the search must keep their results
    expectScale("3e-20 range, 63 bits (smallest valid scale)", -127, [] {
        return get_binary_scale_factor(3e-20, 0.0, 63);
    });
    expectScale("unit range, 24 bits", -23, [] {
        return get_binary_scale_factor(1.0, 0.0, 24);
    });
    expectScale("wide range, 24 bits", 118, [] {
        return get_binary_scale_factor(2e42, -1e42, 24);
    });

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
