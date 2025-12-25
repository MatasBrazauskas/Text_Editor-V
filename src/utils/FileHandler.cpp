#include "utils/FileHandler.hpp"

FileHandler::FileHandler(const char* filesPath) {
	file_.open(filesPath, std::ios::in);

	if (!file_.is_open()) {
		throw std::runtime_error("Could not open file");
	}
}

std::vector<std::string> FileHandler::getContent() {
	std::vector<std::string> lines;

	for (std::string line{}; std::getline(file_, line);) {
		lines.emplace_back(line);
	}

	return lines;
}