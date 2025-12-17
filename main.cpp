#include "src/wrappers/SdlWrapper.hpp"
#include "src/wrappers/Config.hpp"

int main() {
    Wrapper::Config config {"config.json"};

    Wrapper::SdlWrapper app;
    Wrapper::Run(100);
}