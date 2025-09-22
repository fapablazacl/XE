
#ifndef __XE_GRAPHICS_PROGRAM_HPP__
#define XE_GRAPHICS_PROGRAM_HPP_

#include <string>
#include <vector>

namespace XE {
    enum class ShaderType;

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

        virtual Shader *getShader(int index) = 0;
    };
} // namespace XE

#endif
