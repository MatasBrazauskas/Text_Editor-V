#pragma once
#include "ITextBuffer.hpp"
#include <vector>
#include <string>

class Matrix final : public ITextBuffer {
public:
    Matrix() = default;

    void init(std::vector<std::string> lines) override;

    [[nodiscard]] std::string_view rowView(size_t row) override;
    [[nodiscard]] virtual std::string& rowRef(size_t row) override;
    [[nodiscard]] size_t size() const override;

private:
    std::vector<std::string> lines_;
};