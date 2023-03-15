
#include "Util.h"
#include "Common.h"

#include <cstddef>
#include <fstream>

namespace demo {
    XE::ProgramDescriptor makeSimpleProgramDesc(const std::string &vs, const std::string &fs) {
        XE::ProgramDescriptor desc;

        desc.sources = {
            {XE::ShaderType::Vertex, vs},
            {XE::ShaderType::Fragment, fs},
        };

        return desc;
    }

    Mesh makeIndexedCubeMesh(const float width, const float height, const float depth) {
        Mesh mesh;

        const XE::Vector3f n = {0.0f, 0.0f, 1.0};

        const std::vector<Vertex> vertices = {
            {{-0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 0.0f, 0.0f, 1.0f}},  {{0.5f * width, -0.5f * height, 0.5f * depth}, n, {0.0f, 1.0f, 0.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, 0.5f * depth}, n, {0.0f, 0.0f, 1.0f, 1.0f}},   {{0.5f * width, 0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 0.0f, 1.0f}},
            {{-0.5f * width, -0.5f * height, -0.5f * depth}, n, {0.0f, 1.0f, 1.0f, 1.0f}}, {{0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, -0.5f * depth}, n, {0.0f, 0.0f, 0.0f, 1.0f}},  {{0.5f * width, 0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}}};

        mesh.primitive = XE::PrimitiveType::TriangleList;

        const std::vector<int> indices = {0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1};

        // convertir primitiva a lista de triangulos
        bool order = true;

        for (std::size_t i = 0; i < indices.size() - 2; i++) {
            Vertex p1, p2, p3;

            if (order) {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 1]];
                p3 = vertices[indices[i + 2]];
            } else {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 2]];
                p3 = vertices[indices[i + 1]];
            }

            const XE::Vector3f edge1 = p2.coord - p1.coord;
            const XE::Vector3f edge2 = p3.coord - p1.coord;
            const XE::Vector3f normal = XE::normalize(XE::cross(edge1, edge2));

            p1.normal = normal;
            p2.normal = normal;
            p3.normal = normal;

            mesh.vertices.push_back(p1);
            mesh.vertices.push_back(p2);
            mesh.vertices.push_back(p3);

            order = !order;
        }

        return mesh;
    }

    Mesh makeAxisMesh(const float width, const float height, const float depth) {
        const XE::Vector3f n = {0.0f, 1.0f, 0.0};

        Mesh mesh;

        mesh.vertices = {{{0.0f, 0.0f, 0.0f}, n, {0.5f, 0.5f, 0.5f, 1.0f}},
                         {{width, 0.0f, 0.0f}, n, {1.0f, 0.5f, 0.5f, 1.0f}},
                         {{0.0f, height, 0.0f}, n, {0.5f, 1.0f, 0.5f, 1.0f}},
                         {{0.0f, 0.0f, depth}, n, {0.5f, 0.0f, 1.0f, 1.0f}}};
        mesh.primitive = XE::PrimitiveType::LineList;
        mesh.indices = {0, 1, 0, 2, 0, 3};

        return mesh;
    }

    Mesh makeColoredCubeMesh(const float width, const float height, const float depth) {
        const XE::Vector3f n = {0.0f, 0.0f, 1.0};

        Mesh mesh;

        const std::vector<Vertex> vertices = {
            {{-0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 0.0f, 1.0f, 1.0f}},  {{0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, 0.5f * depth}, n, {0.0f, 1.0f, 1.0f, 1.0f}},   {{0.5f * width, 0.5f * height, 0.5f * depth}, n, {0.0f, 1.0f, 1.0f, 1.0f}},
            {{-0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 0.0f, 0.0f, 1.0f}}, {{0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 0.0f, 0.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 0.0f, 1.0f}},  {{0.5f * width, 0.5f * height, -0.5f * depth}, n, {0.0f, 1.0f, 0.0f, 1.0f}}};

        mesh.primitive = XE::PrimitiveType::TriangleList;

        const std::vector<int> indices = {0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1};

        // convertir primitiva a lista de triangulos
        bool order = true;

        for (std::size_t i = 0; i < indices.size() - 2; i++) {
            Vertex p1, p2, p3;

            if (order) {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 2]];
                p3 = vertices[indices[i + 1]];
            } else {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 1]];
                p3 = vertices[indices[i + 2]];
            }

            const XE::Vector3f edge1 = p2.coord - p1.coord;
            const XE::Vector3f edge2 = p3.coord - p1.coord;
            const XE::Vector3f normal = XE::normalize(XE::cross(edge1, edge2));

            p1.normal = normal;
            p2.normal = normal;
            p3.normal = normal;

            mesh.vertices.push_back(p1);
            mesh.vertices.push_back(p2);
            mesh.vertices.push_back(p3);

            order = !order;
        }

        return mesh;
    }

    Mesh makeCubeMesh(const float width, const float height, const float depth) {
        const XE::Vector3f n = {0.0f, 1.0f, 0.0};

        Mesh mesh;

        const std::vector<Vertex> vertices = {
            {{-0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},  {{0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},   {{0.5f * width, 0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{-0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}}, {{0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},  {{0.5f * width, 0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}}};

        mesh.primitive = XE::PrimitiveType::TriangleList;

        const std::vector<int> indices = {0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1};

        // convertir primitiva a lista de triangulos
        bool order = true;

        for (std::size_t i = 0; i < indices.size() - 2; i++) {
            Vertex p1, p2, p3;

            if (order) {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 2]];
                p3 = vertices[indices[i + 1]];
            } else {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 1]];
                p3 = vertices[indices[i + 2]];
            }

            const XE::Vector3f edge1 = p2.coord - p1.coord;
            const XE::Vector3f edge2 = p3.coord - p1.coord;
            const XE::Vector3f normal = XE::normalize(XE::cross(edge1, edge2));

            p1.normal = normal;
            p2.normal = normal;
            p3.normal = normal;

            mesh.vertices.push_back(p1);
            mesh.vertices.push_back(p2);
            mesh.vertices.push_back(p3);

            order = !order;
        }

        return mesh;
    }

    std::string loadTextFile(const std::string &filePath) {
        assert(filePath != "");

        std::fstream fs;
        fs.open(filePath.c_str());

        assert(fs.is_open());

        std::string content;
        std::string line;

        while (std::getline(fs, line)) {
            content += line + "\n";
        }

        return content;
    }

    XE::Subset *createSubset(XE::GraphicsDevice *graphicsDevice, const Mesh &mesh) {
        const XE::BufferDescriptor bufferDesc{XE::BufferType::Vertex, XE::BufferUsage::Read, XE::BufferAccess::Static, static_cast<size_t>(sizeof(Vertex) * mesh.vertices.size()),
                                              reinterpret_cast<const void *>(mesh.vertices.data())};

        XE::Buffer *vertexBuffer = graphicsDevice->createBuffer(bufferDesc);

        XE::Buffer *indexBuffer = nullptr;

        if (mesh.indices.size() > 0) {
            const XE::BufferDescriptor indexBufferDesc{XE::BufferType::Index, XE::BufferUsage::Read, XE::BufferAccess::Static,
                                                       static_cast<size_t>(sizeof(uint32_t) * mesh.indices.size()), reinterpret_cast<const void *>(mesh.indices.data())};

            indexBuffer = graphicsDevice->createBuffer(indexBufferDesc);
        }

        const std::vector<XE::SubsetVertexAttrib> attribs{{0, XE::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, coord)},
                                                          {1, XE::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, normal)},
                                                          {2, XE::DataType::Float32, 4, false, sizeof(Vertex), 0, OFFSETOF(Vertex, color)}};

        XE::SubsetDescriptor subsetDesc{{vertexBuffer}, attribs, indexBuffer};
        XE::Subset *subset = graphicsDevice->createSubset(subsetDesc);

        return subset;
    }

    XE::Subset *createCubeSubset2(XE::GraphicsDevice *graphicsDevice, const Mesh &mesh) {
        const XE::BufferDescriptor bufferDesc{XE::BufferType::Vertex, XE::BufferUsage::Read, XE::BufferAccess::Static, static_cast<size_t>(sizeof(Vertex) * mesh.vertices.size()),
                                              reinterpret_cast<const void *>(mesh.vertices.data())};

        XE::Buffer *vertexBuffer = graphicsDevice->createBuffer(bufferDesc);

        const std::vector<XE::SubsetVertexAttrib> attribs{{0, XE::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, coord)},
                                                          {1, XE::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, normal)},
                                                          {2, XE::DataType::Float32, 4, false, sizeof(Vertex), 0, OFFSETOF(Vertex, color)}};

        XE::SubsetDescriptor subsetDesc{{vertexBuffer}, attribs, nullptr};

        XE::Subset *subset = graphicsDevice->createSubset(subsetDesc);

        return subset;
    }

    Mesh makeGridMesh(const float tileSize, const int tilesInX, const int tilesInZ) {
        const float half = tileSize * 0.5f;

        const XE::Vector3f normal = {0.0f, 0.0f, 1.0};

        const XE::Vector4f white = {0.8f, 0.8f, 0.8f, 1.0f};
        const XE::Vector4f black = {0.2f, 0.2f, 0.2f, 1.0f};

        const XE::Vector3f p1 = {-half, 0.0f, -half};
        const XE::Vector3f p2 = {half, 0.0f, -half};
        const XE::Vector3f p3 = {-half, 0.0f, half};
        const XE::Vector3f p4 = {half, 0.0f, half};

        Mesh mesh;

        bool colorSide = false;

        for (int i = 0; i < tilesInX; i++) {
            for (int k = 0; k < tilesInZ; k++) {
                const XE::Vector3f centroid = {i * tileSize - half * tilesInX, 0.0f, k * tileSize - half * tilesInZ};

                const auto color = colorSide ? white : black;

                mesh.vertices.push_back({p1 + centroid, normal, color});
                mesh.vertices.push_back({p2 + centroid, normal, color});
                mesh.vertices.push_back({p3 + centroid, normal, color});

                mesh.vertices.push_back({p2 + centroid, normal, color});
                mesh.vertices.push_back({p4 + centroid, normal, color});
                mesh.vertices.push_back({p3 + centroid, normal, color});

                const XE::Vector3f p1 = mesh.vertices[mesh.vertices.size() - 3].coord;
                const XE::Vector3f p2 = mesh.vertices[mesh.vertices.size() - 2].coord;
                const XE::Vector3f p3 = mesh.vertices[mesh.vertices.size() - 1].coord;

                const XE::Vector3f edge1 = p2 - p1;
                const XE::Vector3f edge2 = p3 - p1;
                const XE::Vector3f normal = XE::normalize(XE::cross(edge1, edge2));

                for (int ii = 0; ii < 6; ii++) {
                    mesh.vertices[mesh.vertices.size() - ii - 1].normal = normal;
                }

                colorSide = !colorSide;
            }
            colorSide = !colorSide;
        }

        mesh.primitive = XE::PrimitiveType::TriangleList;

        return mesh;
    }
} // namespace demo
