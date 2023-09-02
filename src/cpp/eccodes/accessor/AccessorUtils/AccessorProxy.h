#pragma once

#include "AccessorDefs.h"

// Provides a lightweight interface to an Accessor, for example when using as a member variable

namespace eccodes::accessor {

    double getDouble(AccessorName const& name);
    long getLong(AccessorName const& name);

}
