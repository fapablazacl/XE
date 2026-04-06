
#include "Util.h"
#include "Common.h"

#include <cstddef>
#include <fstream>

namespace demo {
    xe::ProgramDescriptor makeSimpleProgramDesc(const std::string &vs, const std::string &fs) {
        xe::ProgramDescriptor desc;

        desc.sources = {
            {xe::ShaderType::Vertex, vs},
            {xe::ShaderType::Fragment, fs},
        };

        return desc;
    }

    Mesh makeIndexedCubeMesh(const float width, const float height, const float depth) {
        Mesh mesh;

        const xe::Vector3 n = {0.0f, 0.0f, 1.0};

        const std::vector<Vertex> vertices = {
            {{-0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 0.0f, 0.0f, 1.0f}},
            {{0.5f * width, -0.5f * height, 0.5f * depth}, n, {0.0f, 1.0f, 0.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, 0.5f * depth}, n, {0.0f, 0.0f, 1.0f, 1.0f}},
            {{0.5f * width, 0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 0.0f, 1.0f}},
            {{-0.5f * width, -0.5f * height, -0.5f * depth}, n, {0.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, -0.5f * depth}, n, {0.0f, 0.0f, 0.0f, 1.0f}},
            {{0.5f * width, 0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}}
        };

        mesh.primitive = xe::PrimitiveType::TriangleList;

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

            const xe::Vector3 edge1 = p2.coord - p1.coord;
            const xe::Vector3 edge2 = p3.coord - p1.coord;
            const xe::Vector3 normal = xe::normalize(xe::cross(edge1, edge2));

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
        const xe::Vector3 n = {0.0f, 1.0f, 0.0};

        Mesh mesh;

        mesh.vertices = {
            {{0.0f, 0.0f, 0.0f}, n, {0.5f, 0.5f, 0.5f, 1.0f}},
            {{width, 0.0f, 0.0f}, n, {1.0f, 0.5f, 0.5f, 1.0f}},
            {{0.0f, height, 0.0f}, n, {0.5f, 1.0f, 0.5f, 1.0f}},
            {{0.0f, 0.0f, depth}, n, {0.5f, 0.0f, 1.0f, 1.0f}}
        };
        mesh.primitive = xe::PrimitiveType::LineList;
        mesh.indices = {0, 1, 0, 2, 0, 3};

        return mesh;
    }

    Mesh makeColoredCubeMesh(const float width, const float height, const float depth) {
        const xe::Vector3 n = {0.0f, 0.0f, 1.0};

        Mesh mesh;

        const std::vector<Vertex> vertices = {
            {{-0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, 0.5f * depth}, n, {0.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5f * width, 0.5f * height, 0.5f * depth}, n, {0.0f, 1.0f, 1.0f, 1.0f}},
            {{-0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 0.0f, 0.0f, 1.0f}},
            {{0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 0.0f, 0.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 0.0f, 1.0f}},
            {{0.5f * width, 0.5f * height, -0.5f * depth}, n, {0.0f, 1.0f, 0.0f, 1.0f}}
        };

        mesh.primitive = xe::PrimitiveType::TriangleList;

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

            const xe::Vector3 edge1 = p2.coord - p1.coord;
            const xe::Vector3 edge2 = p3.coord - p1.coord;
            const xe::Vector3 normal = xe::normalize(xe::cross(edge1, edge2));

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
        const xe::Vector3 n = {0.0f, 1.0f, 0.0};

        Mesh mesh;

        const std::vector<Vertex> vertices = {
            {{-0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5f * width, -0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5f * width, 0.5f * height, 0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{-0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5f * width, -0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{-0.5f * width, 0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5f * width, 0.5f * height, -0.5f * depth}, n, {1.0f, 1.0f, 1.0f, 1.0f}}
        };

        mesh.primitive = xe::PrimitiveType::TriangleList;

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

            const xe::Vector3 edge1 = p2.coord - p1.coord;
            const xe::Vector3 edge2 = p3.coord - p1.coord;
            const xe::Vector3 normal = xe::normalize(xe::cross(edge1, edge2));

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

    xe::Subset *createSubset(xe::GraphicsDevice *graphicsDevice, const Mesh &mesh) {
        const xe::BufferDescriptor bufferDesc{
            xe::BufferType::Vertex,
            xe::BufferUsage::Read,
            xe::BufferAccess::Static,
            static_cast<size_t>(sizeof(Vertex) * mesh.vertices.size()),
            reinterpret_cast<const void *>(mesh.vertices.data())
        };

        xe::Buffer *vertexBuffer = graphicsDevice->createBuffer(bufferDesc);

        xe::Buffer *indexBuffer = nullptr;

        if (mesh.indices.size() > 0) {
            const xe::BufferDescriptor indexBufferDesc{
                xe::BufferType::Index,
                xe::BufferUsage::Read,
                xe::BufferAccess::Static,
                static_cast<size_t>(sizeof(uint32_t) * mesh.indices.size()),
                reinterpret_cast<const void *>(mesh.indices.data())
            };

            indexBuffer = graphicsDevice->createBuffer(indexBufferDesc);
        }

        const std::vector<xe::SubsetVertexAttrib> attribs{
            {0, xe::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, coord)},
            {1, xe::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, normal)},
            {2, xe::DataType::Float32, 4, false, sizeof(Vertex), 0, OFFSETOF(Vertex, color)}
        };

        xe::SubsetDescriptor subsetDesc{{vertexBuffer}, attribs, indexBuffer};
        xe::Subset *subset = graphicsDevice->createSubset(subsetDesc);

        return subset;
    }

    xe::Subset *createCubeSubset2(xe::GraphicsDevice *graphicsDevice, const Mesh &mesh) {
        const xe::BufferDescriptor bufferDesc{
            xe::BufferType::Vertex,
            xe::BufferUsage::Read,
            xe::BufferAccess::Static,
            static_cast<size_t>(sizeof(Vertex) * mesh.vertices.size()),
            reinterpret_cast<const void *>(mesh.vertices.data())
        };

        xe::Buffer *vertexBuffer = graphicsDevice->createBuffer(bufferDesc);

        const std::vector<xe::SubsetVertexAttrib> attribs{
            {0, xe::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, coord)},
            {1, xe::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, normal)},
            {2, xe::DataType::Float32, 4, false, sizeof(Vertex), 0, OFFSETOF(Vertex, color)}
        };

        xe::SubsetDescriptor subsetDesc{{vertexBuffer}, attribs, nullptr};

        xe::Subset *subset = graphicsDevice->createSubset(subsetDesc);

        return subset;
    }

    Mesh makeGridMesh(const float tileSize, const int tilesInX, const int tilesInZ) {
        const float half = tileSize * 0.5f;

        const xe::Vector3 normal = {0.0f, 0.0f, 1.0};

        const xe::Vector4 white = {0.8f, 0.8f, 0.8f, 1.0f};
        const xe::Vector4 black = {0.2f, 0.2f, 0.2f, 1.0f};

        const xe::Vector3 p1 = {-half, 0.0f, -half};
        const xe::Vector3 p2 = {half, 0.0f, -half};
        const xe::Vector3 p3 = {-half, 0.0f, half};
        const xe::Vector3 p4 = {half, 0.0f, half};

        Mesh mesh;

        bool colorSide = false;

        for (int i = 0; i < tilesInX; i++) {
            for (int k = 0; k < tilesInZ; k++) {
                const xe::Vector3 centroid = {i * tileSize - half * tilesInX, 0.0f, k * tileSize - half * tilesInZ};

                const auto color = colorSide ? white : black;

                mesh.vertices.push_back({p1 + centroid, normal, color});
                mesh.vertices.push_back({p2 + centroid, normal, color});
                mesh.vertices.push_back({p3 + centroid, normal, color});

                mesh.vertices.push_back({p2 + centroid, normal, color});
                mesh.vertices.push_back({p4 + centroid, normal, color});
                mesh.vertices.push_back({p3 + centroid, normal, color});

                const xe::Vector3 pp1 = mesh.vertices[mesh.vertices.size() - 3].coord;
                const xe::Vector3 pp2 = mesh.vertices[mesh.vertices.size() - 2].coord;
                const xe::Vector3 pp3 = mesh.vertices[mesh.vertices.size() - 1].coord;

                const xe::Vector3 edge1 = pp2 - pp1;
                const xe::Vector3 edge2 = pp3 - pp1;
                const xe::Vector3 nnormal = xe::normalize(xe::cross(edge1, edge2));

                for (int ii = 0; ii < 6; ii++) {
                    mesh.vertices[mesh.vertices.size() - ii - 1].normal = nnormal;
                }

                colorSide = !colorSide;
            }
            colorSide = !colorSide;
        }

        mesh.primitive = xe::PrimitiveType::TriangleList;

        return mesh;
    }
} // namespace demo
