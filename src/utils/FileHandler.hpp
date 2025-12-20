#pragma once

#include <fstream>
#include <string>
#include <vector>

class FileHandler {
public :
    FileHandler() = delete;
    FileHandler(const char* filesPath);
    std::vector<std::string> getContent();

    ~FileHandler() = default;
private:
    std::ifstream file_;
};