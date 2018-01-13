
#ifndef __XE_GRAPHICS_D3D11_TESTAPP_RENDERER_HPP__
#define __XE_GRAPHICS_D3D11_TESTAPP_RENDERER_HPP__

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <atlbase.h>

namespace XE::Graphics::D3D11::TestApp {
    class Renderer {
    public:
        Renderer();

        ~Renderer();

        void Initialize(HWND hWnd);

        void Terminate();

        void Render();

    private:
        HWND m_hWnd;
        D3D_FEATURE_LEVEL m_featureLevel;
        D3D_DRIVER_TYPE m_driverType;

        ID3D11Device *m_device;
        ID3D11DeviceContext *m_context;
        IDXGISwapChain *m_swapChain;
        ID3D11Texture2D *m_backBufferTexture;
        ID3D11RenderTargetView *m_backBufferTarget;
    };
}

#endif
