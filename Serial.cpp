#include "Serial.hpp"
#include <errno.h>
#include <fstream>
#include <iostream>

char Serial::getChar()
{
    std::scoped_lock const guard(access_mutex);
    while(!serialDataAvail(file))
        ;
    return serialGetchar(file);
}

std::string Serial::getLine(char delimiter)
{
    std::scoped_lock const guard(access_mutex);

    std::string msg;
    while(1)
    {
        char read_val = 0;
        if(int exitcode = serialDataAvail(file); exitcode > 0)
        {
            msg += read_val = serialGetchar(file);
        }
        if(read_val == delimiter)
            return msg;
    }

    /*
    std::string data;
    while(serialDataAvail(file) > 0 && *std::prev(std::end(data)) != delimiter)
        data += serialGetchar(file);
    return data;
    // */
}

void Serial::write(std::string_view sv)
{
    std::scoped_lock const guard(access_mutex);
    std::ofstream file { directory };
    file << sv;
    //for(auto & data : sv)
    //    serialPutchar(file, data);
}

bool Serial::connected()
{
    std::cerr << "file: " << file << " errorno: " << errno << '\n';
    return file != -1;
}

void Serial::flush()
{
    serialFlush(file);
}
