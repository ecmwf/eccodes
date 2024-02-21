#include "GribStatus.h"

namespace eccodes::accessor {

// Helper for the conversion process
bool GribStatusSuccess(GribStatus stat) 
{ 
    return stat == GribStatus::SUCCESS; 
}

}
