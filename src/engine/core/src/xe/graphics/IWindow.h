
#ifndef __XE_GRAPHICS_IWINDOW_HPP__
#define __XE_GRAPHICS_IWINDOW_HPP__

#include <xe/math/Vector.h>
#include <string>

namespace XE {
    /**
     * @brief The 'Window' that is beign used as the render target for the graphics device contents
     */
    class IWindow {
    public:
        virtual ~IWindow();

        /**
         * @brief Get the current size of the Window, in Pixels, where the X and Y fields correspond to the Width and Height of the Window, respectively
         */
        virtual Vector2i getSizeInPixels() const = 0;

        virtual void setTitle(const std::string& title) = 0;
    };

} // namespace XE

#endif
