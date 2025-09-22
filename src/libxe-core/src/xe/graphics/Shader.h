
#ifndef __XE_GRAPHICS_SHADER_HPP__
#define XE_GRAPHICS_SHADER_HPP_

#include <string>

namespace XE {
    enum class ShaderType {
        Vertex,
        Fragment,
        Geometry,
    };

    class Shader {
    public:
        virtual ~Shader();

        virtual ShaderType getType() const = 0;

        virtual std::string getSource() const = 0;
    };
} // namespace XE

#endif
