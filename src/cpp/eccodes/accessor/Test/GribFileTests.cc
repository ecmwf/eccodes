#include "GribFileTests.h"
#include "AccessorFactory.h"
#include "Accessor.h"
#include "AccessorBuffer.h"
#include "AccessorStore.h"
#include "GribLayout.h"
#include "GribFile.h"
#include "grib_api_internal.h"

#include <iostream>
#include <vector>

namespace eccodes::accessor {

std::string getValues(std::string const& values)
{
    return values;
}

template<typename T>
std::string getValues(std::vector<T> const& values)
{
    if constexpr (std::is_same_v<T, char>) { 
        return values.data(); 
    }
    else if constexpr (std::is_same_v<T, long> || std::is_same_v<T, double>) {
        std::stringstream ss;
        for(auto const& entry : values) { ss << entry << ", ";}
        return ss.str();
    }
    
    return "[DATA]";
}

void createAccessors(LayoutEntries const& entries, GribBuffer& buffer)
{
    std::cout << "Creating accessors...\n\n";

    int offset{};
    int n{};

    std::cout << std::left;

    for(auto const& entry : entries)
    {
        auto accessor = AccessorFactory::instance().build(AccessorType(entry.type_), AccessorName(entry.name_), AccessorNameSpace(""), AccessorInitData{});

        AccessorDataView accBuffer{buffer.data() + offset, static_cast<std::size_t>(entry.byteCount_)};
        offset += entry.byteCount_;

        accessor->newBuffer(accBuffer);

        std::cout << " [" << n++ << "] Type: " << std::setw(15) << entry.type_ << " Name: " << std::setw(15) << entry.name_;
        std::cout << " Buffer start: 0x" << std::hex << static_cast<void*>(accBuffer.data()) << " Buffer size: " << std::dec << accBuffer.size_bytes() << '\n';

        AccessorStore::instance().addAccessor(accessor);
    }

}

template<typename T>
std::string typeString()
{
    if constexpr (std::is_same_v<T, long>) { return "long"; }
    if constexpr (std::is_same_v<T, double>) { return "double"; }
    if constexpr (std::is_same_v<T, std::string>) { return "std::string"; }
    if constexpr (std::is_same_v<T, std::vector<long>>) { return "std::vector<long>"; }
    if constexpr (std::is_same_v<T, std::vector<double>>) { return "std::vector<double>"; }

    return "[Unknown]";
}

template<typename GOOD_TYPE, typename BAD_TYPE>
bool unpack_test(AccessorName const& name)
{
    static_assert(isAllowedUnpackType<GOOD_TYPE>::value, "Unsupported unpack() type supplied");
    static_assert(isAllowedUnpackType<BAD_TYPE>::value, "Unsupported unpack() type supplied");

    std::cout << "\nUnpack test: " << name.get() << '\n';
    std::cout << "GOOD type    : " << typeString<GOOD_TYPE>() << '\n';
    std::cout << "BAD type     : " << typeString<BAD_TYPE>() << '\n';

    auto accessor = AccessorStore::instance().getAccessor(name);
    if(!accessor) { 
        std::cout << "ERROR: Failed to get accessor\n";
        return false; 
    }

    // Unpack - supported
    std::cout << "[1]: Supported unpack : ";
    
    auto goodData = accessor->unpack<GOOD_TYPE>();
    if(goodData.empty()) { 
        std::cout << "FAILED\n";
    } else {
        std::cout << "PASSED: Data = " << getValues(goodData) << '\n';
    }

    // Unpack - unsupported
    std::cout << "[2]: Unupported unpack: ";

    auto badData = accessor->unpack<BAD_TYPE>();
    if(!badData.empty()) { 
         std::cout << "FAILED: data returned! Data = " << getValues(badData) << '\n';
    } else {
        std::cout << "PASSED: no data returned\n";
    }

    return true;
}

void runTests(std::filesystem::path gribFile, std::filesystem::path gribLayoutFile)
{
#undef CREATE_GRIB_FILE
#ifdef CREATE_GRIB_FILE
    std::cout << "Creating grib file: " << gribFile << "...\n\n";
    createGribFile(gribFile);
    return;
#endif

    std::cout << "Parsing layout file: " << gribLayoutFile << "\n\n";

    LayoutEntries entries = parse_grib_layout(gribLayoutFile);
    std::cout << entries << "\n\n";

    std::cout << "Parsing GRIB file: " << gribFile << '\n';
    GribBuffer buffer = parseGribFile(gribFile);
    std::cout << "                 : " << buffer.size() << " bytes read\n";

    std::cout << "\nGrib File contents:\n\n" << buffer << "\n\n";

    createAccessors(entries, buffer);

    std::cout << "\n***** Accessor Tests - native unpack *****\n";
    unpack_test<std::string,std::vector<double>>(AccessorName("greeting"));
    unpack_test<std::vector<long>,std::vector<double>>(AccessorName("data1"));
    unpack_test<std::vector<long>,std::vector<double>>(AccessorName("timestamp"));
    unpack_test<std::vector<long>,std::vector<double>>(AccessorName("data2"));
    unpack_test<std::vector<double>,std::vector<long>>(AccessorName("pi"));
    unpack_test<std::vector<long>,std::vector<double>>(AccessorName("data3"));

    std::cout << "\n***** Accessor Tests - non-native unpack *****\n";
    unpack_test<std::string,std::vector<double>>(AccessorName("timestamp"));


}

}
