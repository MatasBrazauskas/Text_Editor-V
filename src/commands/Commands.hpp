#pragma once
#include <cstdint>
#include "buffer/Cursor.hpp"
#include <string>
#include <SDL.h>


class Commands {
public:
    enum class Modes : std::uint8_t {Normal, Insert};
    Commands();
    ~Commands() = default;
    void HandleInput();

    std::string input;
    Modes mode;
    //Cursor cursor;
};
