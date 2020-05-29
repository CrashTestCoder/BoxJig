#include "Instruction.hpp"
#include "Serial.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

void Instruction::send()
{
    Serial::write(instruction);
    std::cout << instruction;
    auto msg = Serial::getLine();
    
    if (msg[0] != 'o')
        std::cerr << "gcode error:" << msg << '\n'
                  << instruction;
}

Instruction::operator std::string() const
{
    return instruction;
}
