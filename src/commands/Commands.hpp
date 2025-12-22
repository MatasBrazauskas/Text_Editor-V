#pragma once
#include <cstdint>
#include <string>
#include <SDL.h>

//Commands: get keystroke -> convert to some action
class Commands {
public:
    enum class Modes : std::uint8_t {Normal, Insert};
    Commands();
    ~Commands() = default;

    std::string input;
    Modes mode;
};
