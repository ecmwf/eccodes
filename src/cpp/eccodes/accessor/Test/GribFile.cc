#include "GribFile.h"

#include <fstream>
#include <chrono>

namespace eccodes::accessor {

std::ostream& operator<<(std::ostream& os, GribBuffer const& buffer)
{
    static const int entries_per_line{10};
    int index{0};

    while(index < buffer.size())
    {
        int start_pos{index};
        int end_pos{index+entries_per_line};
        if(end_pos > buffer.size()) {end_pos = buffer.size();}

        os << std::hex << std::left;

        for(int n{start_pos}; n<end_pos; ++n)
        {
            os << std::setfill('0') << std::setw(2) 
               << static_cast<int>(static_cast<unsigned char>(buffer[n])) 
               << ' ';
        }

        int padding = (entries_per_line - (end_pos-start_pos)) * 3;
        if(padding>0) {os << std::setfill(' ') << std::setw(padding) << ' ';}

        os << "  ";
        for(int n{start_pos}; n<end_pos; ++n)
        {
            os << (isprint(buffer[n]) ? buffer[n] : '.') << ' ';
        }

        os << '\n';
        index = end_pos;
    }

    return os;
}

void createGribFile(std::filesystem::path gribFile)
{
    std::ofstream ofs(gribFile, std::ios::binary);

    auto time_now = std::chrono::system_clock::now();
    auto since_epoch = time_now.time_since_epoch();

    // Data to write
    std::string greeting("Hello, world!!!");
    long data1{42};
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(since_epoch).count();
    std::vector<long> data2 = {11,22};
    double pi{3.14159265};
    std::vector<long> data3 = {1,1,2,3};

    ofs.write(reinterpret_cast<const char*>(greeting.data()), greeting.size());
    ofs.write(reinterpret_cast<const char*>(&data1), sizeof(data1));
    ofs.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
    ofs.write(reinterpret_cast<const char*>(data2.data()), data2.size() * sizeof(long));
    ofs.write(reinterpret_cast<const char*>(&pi), sizeof(pi));
    ofs.write(reinterpret_cast<const char*>(data3.data()), data3.size() * sizeof(long));
}

GribBuffer parseGribFile(std::filesystem::path gribFile)
{
    std::ifstream file(gribFile, std::ios::binary | std::ios::ate);
    
    if (!file) { return GribBuffer{}; }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    GribBuffer buffer(fileSize);

    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    return buffer;
}

}
