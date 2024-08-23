
#include "Logger.h"

#include <iostream>

Logger::Logger(const std::string &parentName) : mParentName(parentName) {}

Logger::~Logger() {}

void Logger::log(const std::string &info) {
    std::cout << mParentName << ": " << info << std::endl;
}
