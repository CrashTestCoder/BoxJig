#include "Button.hpp"
#include <thread>

//#if __has_include(<wiringPi.h>)
#    include <wiringPi.h>
//#else
//#    include "wiringPiStandin.hpp"
//#endif /* WIRING_PI */

Button::Button(int pin)
    : pin_ { pin }
{
    pinMode(pin, INPUT);
    pullUpDnControl(pin, PUD_UP);
}

bool Button::get() const
{
    return digitalRead(pin_);
}

void Button::waitForNext(bool state, bool *timeout) const
{
    bool const init_state = get();
    while(init_state == get() && ((timeout && *timeout) || !timeout))
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    while(get() != state && ((timeout && *timeout) || !timeout))
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Button::onState(bool state, std::function<void()> func, bool &timeout)
{
    bool start_state = get();
    while(start_state == get() && !timeout);
    while(get() != state && !timeout);
    if(!timeout)
        func();
}
