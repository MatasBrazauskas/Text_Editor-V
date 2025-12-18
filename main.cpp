#include "SdlTtf.h"
#include "src/wrappers/SdlWindow.hpp"
#include "src/utils/Config.hpp"

int main() {
    const std::filesystem::path filesPath = "config.json";
    Utils::Config config (filesPath);

    const auto title = "Text Editor";
    Wrapper::SdlWindow window{title};

    Wrapper::SdlTtf font{config.font_};
    while (Wrapper::WindowOpen()) {

    }
}