#include "Commands.hpp"

#include <iostream>
#include <ostream>

Commands::Commands(): mode(Modes::Normal) {}

void Commands::HandleInput() {
    SDL_Event event;

    SDL_PollEvent(&event);

    if (event.type == SDL_QUIT) {
        abort();
    }

    if (event.type == SDL_TEXTINPUT) {
        std::cout << "Text input ";
        input.append(event.text.text);
        std::cout << input << std::endl;
    }
    else if (event.type == SDL_KEYDOWN) {
        std::cout << "Key down\n";
    }
}
