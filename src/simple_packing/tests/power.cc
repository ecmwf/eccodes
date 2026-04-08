#include <iostream>
#include <cmath>

#include <Utils.h>

int main(int argc, char** argv) {

    for (int bpv = 0; bpv <= 64; bpv++) {
        std::cout << std::fixed << sp_power<double>(bpv, 2) << std::endl;
        std::cout << pow(2, bpv) << std::endl;
    }

}
