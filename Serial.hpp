#pragma once

#include <filesystem>
#include <mutex>
#include <string_view>

#if __has_include(<wiringSerial.h>)
#    include <wiringSerial.h>
#else
#    include "wiringSerialStandin.hpp"
#endif

class Serial
{
    static std::filesystem::path directory;
    static inline int file = serialOpen(directory.c_str(), 115200);
    static inline std::mutex access_mutex;

public:
    static char getChar();
    static std::string getLine(char delimiter = '\n');
    static void write(std::string_view sv);
    static bool connected();
    static void flush();
};
