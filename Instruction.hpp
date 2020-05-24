#pragma once

#include <string>

class Instruction
{
    std::string const instruction;

public:
    Instruction(std::string_view instr)
        : instruction { instr }
    {}
    Instruction(Instruction const&) = default;
    Instruction(Instruction&&)      = default;

    void send();
    operator std::string() const;
};
