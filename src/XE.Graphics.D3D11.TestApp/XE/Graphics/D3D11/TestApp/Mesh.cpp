
#include "Mesh.hpp"
#include <XE/Math/Vector.hpp>

namespace XE::Graphics::D3D11::TestApp {
    struct VertexType {
        XE::Math::Vector3f position;
        XE::Math::Vector4f color;
    };

    Mesh::Mesh() {}

    Mesh::~Mesh() {
        this->Shutdown();
    }

    bool Mesh::Initialize(ID3D11Device *device) {
        return this->InitializeBuffers(device);
    }

    void Mesh::Shutdown() {
        this->ShutdownBuffers();
    }

    void Mesh::Render(ID3D11DeviceContext *context) {
        this->RenderBuffers(context);
    }

    int Mesh::GetIndexCount() const {
        return indexCount;
    }
        
    bool Mesh::InitializeBuffers(ID3D11Device *device) {
        VertexType vertices[] = {
            {
                {-1.0f, -1.0f, 0.0f}, 
                {1.0f, 0.0f, 0.0f, 1.0f}
            },
            {
                {0.0f, 1.0f, 0.0f}, 
                {1.0f, 0.0f, 0.0f, 1.0f}
            },
            {
                {1.0f, -1.0f, 0.0f}, 
                {1.0f, 0.0f, 0.0f, 1.0f}
            }
        };

        int indices[] = {0, 1, 2};

        // setup vertex data
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(VertexType) * 3;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexData = {};
        vertexData.pSysMem = vertices;
        vertexData.SysMemPitch = 0;
        vertexData.SysMemSlicePitch = 0;

        //setup index data
        D3D11_BUFFER_DESC indexBufferDesc = {};
        
        D3D11_SUBRESOURCE_DATA indexData;
    }

    void Mesh::ShutdownBuffers() {

    }

    void Mesh::RenderBuffers(ID3D11DeviceContext *context) {

    }
}
