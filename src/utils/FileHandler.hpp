#pragma once

#include <filesystem>
#include <string>
#include <vector>

class File;

class FileHandler final {
  public:
	FileHandler() = default;
	~FileHandler() noexcept = default;

	std::vector<std::string> readFile(std::filesystem::path) const;
	std::vector<std::string> readDirectory(std::filesystem::path = ".") const;
	void writeToFile(const File&) const;
};