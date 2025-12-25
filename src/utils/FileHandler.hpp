#pragma once

#include <fstream>
#include <string>
#include <vector>

class FileHandler final {
public :
	FileHandler() = delete;

	FileHandler(const char*);

	std::vector<std::string> getContent();

	~FileHandler() = default;

private:
	std::ifstream file_;
};