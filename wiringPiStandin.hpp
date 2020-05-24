#pragma once

#include <iostream>

#if __has_include(<conio.h>)
#    include <conio.h>
#else
#    include <termios.h>
#    include <unistd.h>

inline static char getch(void)
{
    char buf           = 0;
    struct termios old = { 0 };
    fflush(stdout);
    if(tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN]  = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if(read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    printf("%c\n", buf);
    return buf;
}
#endif

enum Mode { INPUT,
            OUTPUT,
            PWM_OUTPUT,
            GPIO_CLOCK };

enum PUpDn { PUD_OFF,
             PUD_DOWN,
             PUD_UP };

inline static void wiringPiSetup()
{
    std::cerr << "Warning: wiringPi.h was not found, using debugging alternatives\n";
}

inline static void pinMode(int pin, int mode)
{
    std::cout << "Setup Pin " << pin << " to mode " << mode << "\n";
}
inline static void pullUpDnControl(int pin, int pud)
{
    std::cout << "Setup Pin " << pin << " bias to " << pud << "\n";
}
inline static int digitalRead(int pin)
{
    pin = pin;
    return getch() != ' ';
}