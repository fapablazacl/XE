
#ifndef __XE_UI_FRAME_HPP__
#define __XE_UI_FRAME_HPP__

#include <cstddef>
#include <string>
#include <functional>
#include <XE/Predef.hpp>
// #include <XE/Math/Vector.hpp>

namespace XE {
    class XE_API InputManager;
    /**
     * @brief 
     */
    typedef void* FrameHandle;

    struct FrameProperties {
        std::string caption;
        // Vector2i size;
        // Vector2i position;
    };

    /**
     * @brief Abstract a (possible) native OS window frame, for presenting rendering content to the user.
     */
    class Frame {
    public:
        virtual ~Frame();

        virtual bool getCloseRequested() const = 0;

        virtual FrameHandle getHandle() const = 0;

        virtual FrameProperties getProperties() const = 0;

        virtual void setProperties(const FrameProperties &properties) = 0;
    };
}

#endif
