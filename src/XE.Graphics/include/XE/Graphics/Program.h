
#ifndef __XE_GRAPHICS_PROGRAM_HPP__
#define __XE_GRAPHICS_PROGRAM_HPP__

#include <string>
#include <vector>

namespace XE {
    enum class ShaderType;

    enum ShaderLang {
        GLSL,
        HLSL
    };
    
    struct ProgramSource {
        ShaderType type;
        std::string text;
    };

    struct ProgramDescriptor {
        std::vector<ProgramSource> sources;
    };

    class Shader;
    class Program {
    public:
        virtual ~Program();

        virtual int getShaderCount() const = 0;

        virtual Shader* getShader(const int index) = 0;
    };
}

#endif
