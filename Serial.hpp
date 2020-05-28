#pragma once

#include <filesystem>
#include <mutex>
#include <string_view>

class Serial
{
    static std::filesystem::path directory;
    static inline std::mutex access_mutex;

public:
    static char getChar();
    static std::string getLine(char delimiter = '\n');
    static void write(std::string_view sv);
};