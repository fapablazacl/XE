
#ifndef __XE_GRAPHICS_SUBSET_HPP__
#define __XE_GRAPHICS_SUBSET_HPP__

#include <cstddef>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <optional>

#include <XE/Predef.h>
#include <XE/DataType.h>

namespace XE {
    class Buffer;
}

namespace XE {
    /**
     * @brief Geometric Primitive Shape basis for rendering
     */
    enum class PrimitiveType {
        PointList,
        LineList,
        LineStrip,
        TriangleStrip,
        TriangleList,
        TriangleFan
    };
    
    /**
     * @brief Interpretation information for a given subset. Used in rendering operations.
     */
    struct SubsetEnvelope {
        //! Geometric Primitive Shape basis for rendering
        PrimitiveType Primitive = PrimitiveType::PointList;

        //! From wich vertex we will render
        int VertexStart = 0;
        
        //! How many vertices we must use for render
        int VertexCount = 0;
    };
    
    /**
     * @brief Describes a Vertex Attribute for use in the vertex shader
     */
    struct VertexAttribute {
        //! attribute Name in the Vertex Shader
        std::string name;

        //! Basic data type
        DataType type = DataType::Unknown;

        //! Attribute dimension (1, 2, 3 or 4)
        int size = 0;
    };

    /**
     * @brief Describes a geometry subset
     */
    struct [[deprecated("Use SubsetDescriptor2. It's more flexible, and less error-prone")]] SubsetDescriptor {
        //! The vertex structure
        std::vector<VertexAttribute> attributes;

        //! The index buffer datatype
        DataType indexType = DataType::Unknown;
    };

    /**
     * @brief Describes a Vertex Attribute for use in the vertex shader
     */
    struct SubsetVertexAttrib {
        //! attribute layout location in the vertex shader
        int shaderLocation = 0;

        //! Basic data type
        DataType type = DataType::Unknown;

        //! attribute dimension (1, 2, 3 or 4)
        int size = 0;

        bool normalized = false;

        size_t stride = 0;

        //!
        size_t bufferIndex = 0;

        //! 
        size_t bufferOffset = 0;
    };

    /**
     * @brief Describes a geometry subset.
     */
    struct SubsetDescriptor2 {
        const Buffer **buffers = nullptr;

        size_t bufferCount = 0;

        const SubsetVertexAttrib *attribs = nullptr;

        size_t attribCount = 0;

        const Buffer* indexBuffer = nullptr;
    };

    /**
     * @brief Geometry Subset
     *
     * This class abstracts a (possibly indexed) Geometry Subset, independent of the Vertex Layout.
     * The Vertex Layout just specifies the different attributes supported by each vertex stored in one or many
     * buffer(s).
     */
    class Subset {
    public:
        virtual ~Subset();

        /**
         * @brief Get the count of buffers attached to the geometric subset.
         */
        virtual int getBufferCount() const = 0;

        /**
         * @brief Get a mutable pointer to a buffer allocated in GPU memory. This buffer stores geometric data
         */
        virtual Buffer* getBuffer(const int index) = 0;

        /**
         * @brief Get a mutable pointer to a buffer allocated in GPU memory. This buffer stores indexation data
         */
        virtual Buffer* getIndexBuffer() = 0;

        /**
         * @brief Get a constant pointer to a buffer allocated in GPU memory. This buffer stores geometric data
         */
        virtual const Buffer* getBuffer(const int index) const = 0;

        /**
         * @brief Get a constant pointer to a buffer allocated in GPU memory. This buffer stores indexation data
         */
        virtual const Buffer* getIndexBuffer() const = 0;

        /**
         * @brief Get the current description structure that leads to the creation of the subset
         */
        [[deprecated]]
        virtual SubsetDescriptor GetDescriptor() const = 0;
    };
}

#endif
