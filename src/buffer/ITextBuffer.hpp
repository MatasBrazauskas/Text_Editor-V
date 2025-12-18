#pragma once

#include <cstddef>
#include <vector>
#include <string>

class ITextBuffer {
    public :
        virtual ~ITextBuffer() = default;
        [[nodiscard]] virtual char at(size_t row, size_t column) const = 0;
        virtual void init(std::vector<std::string>& matrix) = 0;
};