#pragma once

namespace eccodes::accessor {

enum class GribAccessorFlag{
    READ_ONLY                = (1 << 1),
    DUMP                     = (1 << 2),
    EDITION_SPECIFIC         = (1 << 3),
    CAN_BE_MISSING           = (1 << 4),
    HIDDEN                   = (1 << 5),
    CONSTRAINT               = (1 << 6),
    BUFR_DATA                = (1 << 7),
    NO_COPY                  = (1 << 8),
    COPY_OK                  = (1 << 9),
    FUNCTION                 = (1 << 10),
    DATA                     = (1 << 11),
    NO_FAIL                  = (1 << 12),
    TRANSIENT                = (1 << 13),
    STRING_TYPE              = (1 << 14),
    LONG_TYPE                = (1 << 15),
    DOUBLE_TYPE              = (1 << 16),
    LOWERCASE                = (1 << 17),
    BUFR_COORD               = (1 << 18),
    COPY_IF_CHANGING_EDITION = (1 << 19)
};

constexpr int toInt(GribAccessorFlag flag) { return static_cast<int>(flag); }

}
