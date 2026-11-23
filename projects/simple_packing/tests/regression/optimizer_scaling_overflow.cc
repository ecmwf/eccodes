// Fields the decimal scale factor search cannot scale without overflowing the double range
// (values near +-DBL_MAX scaled by 10^D, or a decimal scale factor so large that value * 10^D
// overflows) must be rejected with a clear error: the search overflowed to inf/NaN and then
// converted the NaN to integers (undefined behaviour) or failed much later with a misleading
// message. Each case runs in a child process with a time limit; the error must mention the
// range that cannot be scaled.

#include <exception>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "simple_packing/optimizer/BPVOptimizer.h"

static int failures = 0;

// Runs f in a child process: it must throw an error about the unscaled range within 5 seconds
template <typename F>
static void expectScaleError(const std::string& what, F&& f)
{
    const pid_t pid = fork();
    if (pid == 0) {
        alarm(5);
        try {
            f();
        }
        catch (const std::exception& e) {
            const std::string message = e.what();
            _exit(message.find("cannot scale the range") != std::string::npos ? 0 : 2);
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
                      WIFSIGNALED(status) ? "crashes" :
                      WEXITSTATUS(status) == 2 ? "wrong error message" : "no error")
              << '\n';
    failures++;
}

int main()
{
    expectScaleError("BPVOptimizer: value * 10^decimalScaleFactor overflows", [] {
        BPVOptimizer(400).getParams(1.0, 2.0);
    });
    expectScaleError("BPVOptimizer: value * 10^decimalScaleFactor overflows (320)", [] {
        BPVOptimizer(320).getParams(1.0, 2.0);
    });
    expectScaleError("BPVOptimizer: scaling overflows to inf", [] {
        BPVOptimizer(2).getParams(-1.7e308, -1.7e308 + 1e-300);
    });

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
