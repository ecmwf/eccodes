#include "GribStatus.h"

namespace eccodes::accessor {

// Helper for the conversion process
bool isError(GribStatus stat) 
{ 
    return stat != GribStatus::SUCCESS; 
}

}
