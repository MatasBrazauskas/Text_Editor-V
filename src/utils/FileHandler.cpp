#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace IO {
    class FileHandler {
    public :
        FileHandler() = delete;
        FileHandler(const char* filesPath) {
            file_.open(filesPath, std::ios::in);

            if (!file_.is_open()) {
                throw std::runtime_error("Could not open file");
            }
        }

        std::vector<std::string> getLine() {
            std::vector<std::string> lines;

            for (std::string line{}; std::getline(file_, line); ) {
                lines.emplace_back(line);
            }

            return lines;
        }

        virtual ~FileHandler() = default;
    private:
        std::ifstream file_;
    };
}