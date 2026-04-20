#include "utils/FileHandler.hpp"

#include "core/EditorCore.hpp"

#include <fstream>

std::vector<std::string> FileHandler::readFile(std::filesystem::path filesPath) const {
	std::ifstream file_(filesPath, std::ios::in);

	std::vector<std::string> lines;

	for (std::string line{}; std::getline(file_, line);) {
		lines.emplace_back(line);
	}

	return lines;
}

void FileHandler::writeToFile(const File& doc) const {
	auto& [text, stack, path, id] = doc;
	const auto paths = doc.filesPath_.string() + ".tmp";

	{
		std::ofstream tempFile_(paths, std::ios::binary | std::ios::trunc);
		if (!tempFile_.is_open()) {
			throw std::runtime_error("Failed to open file for writing");
		}

		for (int i = 0; i < text.getLinesCount(); i++) {
			tempFile_.write(text.getLine(i).data(), text.getLineLength(i));
			tempFile_.put('\n');
		}

		tempFile_.flush();
	}

	std::error_code errorCode;
	std::filesystem::rename(paths, doc.filesPath_, errorCode);

	if (errorCode) {
		std::filesystem::remove(paths);
		throw std::runtime_error("Failed to rename file");
	}
}

std::vector<std::string> FileHandler::readDirectory(const std::filesystem::path t_path) const {
	std::vector<std::string> dirContent = {};

	for (const auto& file : std::filesystem::directory_iterator{t_path}) {
		const auto filePath = file.path().string();
		const auto index = filePath.find_last_of('/');
		auto fileName = std::string(filePath.substr(index + 1));

		if (file.is_directory()) {
			fileName.push_back('/');
		}
		dirContent.push_back(fileName);
	}

	return dirContent;
}
