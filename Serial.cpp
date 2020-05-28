#include "Serial.hpp"

#if __has_include(<wiringSerial.h>)
#    include <wiringSerial.h>
#else
#    include <wiringSerialStandin.h>
#endif

char Serial::getChar()
{
    std::scoped_lock const guard(access_mutex);
    char c;
    while((c = std::fgetc(file)) == EOF)
        ;
    return c;
}

std::string Serial::getLine(char delimiter)
{
    std::scoped_lock const guard(access_mutex);
    std::string data;
    char c;
    while(! ((c = std::fgetc(file)) == EOF) || (c == delimiter))
        data += c;
    return data;
}

void Serial::write(std::string_view sv)
{
    std::scoped_lock const guard(access_mutex);
    std::fwrite(sv.data(), sizeof(*sv.begin()), sv.size(), file);
}