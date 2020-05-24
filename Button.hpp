#pragma once

#include <iostream>

class Button
{
    int const pin_;

public:
    Button(int pin);
    bool get() const;
    void waitForNext(bool state) const;
};