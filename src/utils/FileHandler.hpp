#pragma once

#include <fstream>
#include <string>
#include <vector>

class FileHandler final {
public :
	FileHandler() = default;

	std::vector<std::string> getContent(const char* filesPath);

	~FileHandler() = default;
};