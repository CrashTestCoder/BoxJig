#include "Button.hpp"
#include <thread>

#if __has_include(<wiringPi.h>)
#    include <wiringPi.h>
#else
#    include "wiringPiStandin.hpp"
#endif /* WIRING_PI */

Button::Button(int pin)
    : pin_ { pin }
{
    pinMode(pin, INPUT);
    pullUpDnControl(pin, PUpDn::PUD_UP);
}

bool Button::get() const
{
    return digitalRead(pin_);
}

void Button::waitForNext(bool state) const
{
    bool const init_state = get();
    while(init_state == get())
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    while(get() != state)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
