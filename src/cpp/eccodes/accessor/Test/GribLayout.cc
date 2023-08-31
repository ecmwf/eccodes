#include "GribLayout.h"

#include <fstream>

namespace eccodes::accessor {


std::ostream& operator<<(std::ostream& os, LayoutEntries entries)
{
    static int nameWidth{15};
    static int typeWidth{8};
    static int bytesWidth{8};
    os << std::left;
    os << std::setw(nameWidth) << "Name" << std::setw(typeWidth) << "Type" << std::setw(bytesWidth) << "Bytes" << '\n';
    os << std::setw(nameWidth) << "----" << std::setw(typeWidth) << "----" << std::setw(bytesWidth) << "-----" << '\n';

    int totalBytes{0};

    for(auto const& entry : entries)
    {
        os << std::setw(nameWidth) << entry.name_ << std::setw(typeWidth) << entry.type_ << std::setw(bytesWidth) << entry.byteCount_ << '\n';
        totalBytes += entry.byteCount_;
    }
    os << std::setw(nameWidth) << "----" << std::setw(typeWidth) << "----" << std::setw(bytesWidth) << "-----" << '\n';
    os << std::setw(nameWidth) << "Total bytes" << std::setw(typeWidth) << ' ' << std::setw(bytesWidth) << totalBytes << '\n';

    return os;
}

LayoutEntries parse_grib_layout(std::filesystem::path layoutFile)
{
    LayoutEntries entries;

    std::ifstream ifs(layoutFile);

    while(ifs)
    {
        LayoutEntry entry{};
        ifs >> entry.name_ >> entry.type_ >> entry.byteCount_;
        
        if(!entry.name_.empty()) { entries.push_back(entry); }
    }

    return entries;
}

}
