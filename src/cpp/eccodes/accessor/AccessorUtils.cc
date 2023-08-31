#include "AccessorUtils.h"
#include <filesystem>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace eccodes::accessor {

// Note - these are NOT thread safe!

auto createLogFile = [](std::string const& fileName){
    const auto clockNow = std::chrono::system_clock::now();
    const std::time_t timeNow = std::chrono::system_clock::to_time_t(clockNow);

    std::stringstream ssPath;
    ssPath << "/home/kev/grib_handle_logs/" << fileName << '_';
    ssPath << std::put_time(std::localtime(&timeNow), "%Y%m%d_%H%M%S");
    ssPath << ".log";

    std::filesystem::path logFile(ssPath.str());

    return std::ofstream(logFile, std::ios_base::app);
};

std::ofstream& debugLog()
{
    static std::ofstream debugLog_ = createLogFile("debug");
    return debugLog_;
}

std::ofstream& errorLog()
{
    static std::ofstream errorLog_ = createLogFile("error");
    return errorLog_;
}

}

