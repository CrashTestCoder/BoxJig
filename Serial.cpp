#include "Serial.hpp"
#include <fstream>
#if __has_include(<wiringSerial.h>)
#    include <wiringSerial.h>
#else
#    include <wiringSerialStandin.h>
#endif

char Serial::getChar()
{
    std::scoped_lock const guard(access_mutex);
    while(! serialDataAvail(file))
        ;
    return serialGetchar(file);
}

std::string Serial::getLine(char delimiter)
{
    std::scoped_lock const guard(access_mutex);
    std::string data;
    while(serialDataAvail(file) > 0 && *std::prev(std::end(data)) != delimiter)
        data += serialGetchar(file);
    return data;
}

void Serial::write(std::string_view sv)
{
    std::scoped_lock const guard(access_mutex);
    std::ofstream file { directory };
    file << sv;
    //for(auto & data : sv)
    //    serialPutchar(file, data);
}
