#include "src/wrappers/SdlWindow.hpp"
#include "src/wrappers/Config.hpp"

int main() {
    Wrapper::Config config {"config.json"};

    Wrapper::SdlWindow app;
    Wrapper::Run(2);
}