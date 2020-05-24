#include "Instruction.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

extern std::filesystem::path serial_dir;

void Instruction::send()
{
    if(std::filesystem::exists(serial_dir))
    {
        std::ofstream serial { serial_dir };
        serial << instruction;
    }
    else
        std::cerr << "Serial directory " << serial_dir << " does not exist\n";
    std::cout << instruction;
}

Instruction::operator std::string() const
{
    return instruction;
}
