
#ifndef __XE_GRAPHICS_PROGRAM_HPP__
#define __XE_GRAPHICS_PROGRAM_HPP__

namespace XE::Graphics {
    enum ShaderLang {
        GLSL,
        HLSL
    };

    struct Structure;

    class Shader;
    class Program {
    public:
        virtual ~Program();

        virtual int GetShaderCount() const = 0;

        virtual Shader* GetShader(const int index) = 0;
    };
}

#endif
