#pragma once

#include <filesystem>
#include <string>
#include <vector>

class File;

class FileHandler final {
public :
	FileHandler() = default;
    ~FileHandler() = default;

	std::vector<std::string> getContent(std::filesystem::path) const;

    void writeToFile(const File&) const;

};