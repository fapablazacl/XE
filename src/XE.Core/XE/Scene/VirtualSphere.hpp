
#ifndef __XE_SCENE_VIRTUALSPHERE_HPP__
#define __XE_SCENE_VIRTUALSPHERE_HPP__

#include <XE/Math/Vector.hpp>
#include <XE/Math/Rotation.hpp>

namespace XE {
    /**
     * @brief Virtual sphere tied into a screen. Used as an utility in the Trackball class.
     */
    class VirtualSphere {
    public:
        VirtualSphere();

        VirtualSphere(const Vector2i &screenSize);

        Vector2i getScreenSize() const {
            return screenSize;
        }

        void setScreenSize(const Vector2i &value);

        Vector3f computePointAt(const Vector2i &screenPosition) const;

    private:
        Vector2i screenSize;
    };
}

#endif
