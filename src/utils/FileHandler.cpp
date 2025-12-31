#include "utils/FileHandler.hpp"

std::vector<std::string> FileHandler::getContent(const char* filesPath) {
	std::ifstream file_(filesPath, std::ios::in);

	std::vector<std::string> lines;

	for (std::string line{}; std::getline(file_, line);) {
		lines.emplace_back(line);
	}

	return lines;
}