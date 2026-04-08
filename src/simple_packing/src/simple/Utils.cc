#include "Utils.h"

size_t nSplits(size_t totalSize, size_t splitSize)
{
    size_t nSplits = totalSize / splitSize;
    size_t remainder = totalSize % splitSize;
    return nSplits + (remainder > 0 ? 1 : 0);
}
