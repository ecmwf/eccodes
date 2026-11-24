// Unit tests of Span and SplitBlock:
// - SplitBlock splits n values into full blocks and a part for every n up to 300 and every block size
// - Span: begin/end/data/size/drop
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "spaec/Span.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

static void splitBlock()
{
    for (size_t block : { 1, 8, 32, 64 }) {
        for (size_t n = 0; n <= 300; ++n) {
            std::vector<int> v(n);
            for (size_t i = 0; i < n; ++i)
                v[i] = static_cast<int>(i);
            SplitBlock<int> s(Span<int>(v), block);
            const std::string name = "SplitBlock(" + std::to_string(n) + ", " + std::to_string(block) + ")";
            check(s.nFull() == n / block, name + ": nFull");
            check(s.hasPart() == (n % block != 0) && s.nPart() == (n % block != 0 ? 1u : 0u), name + ": part");
            bool ok = true;
            for (size_t b = 0; b < s.nFull(); ++b) {
                ok = ok && s.full(b).size() == block && s.full(b)[0] == static_cast<int>(b * block);
            }
            if (s.hasPart()) {
                ok = ok && s.part().size() == n % block && s.part()[0] == static_cast<int>(s.nFull() * block);
            }
            check(ok, name + ": contents");
        }
    }
}

static void span()
{
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    Span<int> s(v);
    int sum = 0;
    for (int x : s)
        sum += x;
    check(s.size() == 5 && s.data() == v.data() && sum == 15 && s.end() - s.begin() == 5, "Span over 5 values");
    Span<int> d = s.drop(2);
    check(d.size() == 3 && d[0] == 3 && d.data() == v.data() + 2, "Span::drop(2)");
    check(s.drop(5).size() == 0, "Span::drop of all values");
}

int main()
{
    splitBlock();
    span();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
