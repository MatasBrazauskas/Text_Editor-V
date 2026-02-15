#include "utils/FileHandler.hpp"

#include <fstream>

#include "core/EditorCore.hpp"

std::vector<std::string> FileHandler::getContent(std::filesystem::path filesPath) const {
	std::ifstream file_(filesPath, std::ios::in);

	std::vector<std::string> lines;

	for (std::string line{}; std::getline(file_, line);) {
		lines.emplace_back(line);
	}

	return lines;
}

void FileHandler::writeToFile(const File& doc) const {
    auto& [text, stack, cursor, path, id] = doc;
    auto tempFilesPath = doc.filesPath_;
    tempFilesPath += ".tmp";

    {
        std::ofstream tempFile_(tempFilesPath, std::ios::binary | std::ios::trunc);
        if (!tempFile_.is_open()) {
            throw std::runtime_error("Failed to open file for writing");
        }

        for (int i = 0; i < text->getLinesCount(); i++) {
            tempFile_.write(text->getLine(i).data(), text->getLineLength(i));
            tempFile_.put('\n');
        }

        tempFile_.flush();
    }

    std::error_code errorCode;
    std::filesystem::rename(tempFilesPath, doc.filesPath_, errorCode);

    if (errorCode) {
        std::filesystem::remove(tempFilesPath);
        throw std::runtime_error("Failed to rename file");
    }
}
