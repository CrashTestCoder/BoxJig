#pragma once

#include <functional>

class Button
{
    int const pin_;

public:
    Button(int pin);
    bool get() const;
    void waitForNext(bool state, bool *timeout = nullptr) const;
    void onState(bool state, std::function<void()> func, bool &timeout);
};
