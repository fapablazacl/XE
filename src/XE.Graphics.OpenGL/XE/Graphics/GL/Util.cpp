
#include "Util.hpp"

#include <cassert>
#include <iostream>
#include <glad/glad.h>

namespace XE::Graphics::GL {
    void __checkErrorImpl(const char *file, const int line) {
        GLenum errorGL = ::glGetError();

        if (errorGL != GL_NO_ERROR) {
            std::cout << "[GL] " << errorGL << " error at " << file << "(" << line << ")" << std::endl;
            assert(false);
        }
    }
}
