#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <string_view>

class ITextBuffer {
public :
    virtual ~ITextBuffer() = default;
    [[nodiscard]] virtual std::string_view rowView(size_t row) = 0;
    [[nodiscard]] virtual std::string& rowRef(size_t row) = 0;
    [[nodiscard]] virtual size_t size() const = 0;
    virtual void erase(size_t row) = 0;
    virtual void init(std::vector<std::string> matrix) = 0;
};