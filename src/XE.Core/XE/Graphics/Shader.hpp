
#ifndef __XE_GRAPHICS_SHADER_HPP__
#define __XE_GRAPHICS_SHADER_HPP__

#include <string>

namespace XE::Graphics {
    enum class ShaderType {
        Vertex,
        Fragment,
        Geometry,
    };

    class Shader {
    public:
        virtual ~Shader();

        virtual ShaderType GetType() const = 0;

        virtual std::string GetSource() const = 0;
    };
}

#endif
