#include "ITextBuffer.hpp"
#include <vector>
#include <string>
#include <utility>

using Vec = std::vector<std::string>;

namespace Buffer {
    class Matrix : public ITextBuffer {
    public :
        Matrix() = default;

        [[nodiscard]] char at(const size_t row, const size_t column) const override {
            return vec.at(row).at(column);
        }
        void init(Vec& matrix) override {
            vec = std::move(matrix);
        }
    private :
        Vec vec;
    };
}