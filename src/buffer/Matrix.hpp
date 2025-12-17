#pragma once

import <vector>;
import <string>;

using Vec = std::vector<std::string>;

class Matrix : public ITextBuffer {
    public :
        public Matrix() {
            vec = new Vec();
        }
        virtual void insert(char character);
    private :
        Vec vec;
}