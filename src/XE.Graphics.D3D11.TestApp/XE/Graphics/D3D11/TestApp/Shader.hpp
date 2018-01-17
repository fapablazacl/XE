
#ifndef __XE_GRAPHICS_D3D11_TESTAPP_SHADER_HPP__
#define __XE_GRAPHICS_D3D11_TESTAPP_SHADER_HPP__

#include <d3d11.h>
#include <XE/Math/Matrix.hpp>

namespace XE::Graphics::D3D11::TestApp {
    class Shader {
    public:
        Shader();

        ~Shader();

        bool Initialize(ID3D11Device *device, HWND hWnd);

        void Shutdown();

        bool Render(ID3D11DeviceContext *context, int indexCount, const XE::Math::Matrix4f &world, const XE::Math::Matrix4f &view, const XE::Math::Matrix4f &projection);

    private:
        bool InitializeShader(ID3D11Device *device, HWND hWnd, LPCWSTR vertexShaderSrc, LPCWSTR pixelShaderSrc);

        void ShutdownShader();

        void DisplayShaderErrorMessage(ID3D10Blob *blob, HWND hWnd, LPCWSTR msg);

        bool SetShaderParameters(ID3D11DeviceContext *context, const XE::Math::Matrix4f &world, const XE::Math::Matrix4f &view, const XE::Math::Matrix4f &projection);

        void RenderShader(ID3D11DeviceContext *context, int indexCount);

    private:
        struct MatrixBufferType {
            XE::Math::Matrix4f world;
            XE::Math::Matrix4f view;
            XE::Math::Matrix4f projection;
        };

        ID3D11VertexShader *vertexShader = nullptr;
        ID3D11PixelShader *pixelShader = nullptr;
        ID3D11InputLayout *layout = nullptr;
        ID3D11Buffer *matrixBuffer = nullptr;
    };
}

#endif
