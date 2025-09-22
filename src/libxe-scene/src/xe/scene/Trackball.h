
#ifndef __XE_SCENE_TRACKBALL_HPP__
#define XE_SCENE_TRACKBALL_HPP_

#include <xe/math/Quaternion.h>
#include <xe/math/Rotation.h>
#include <xe/math/Vector.h>
#include <xe/scene/VirtualSphere.h>

namespace XE {
    /**
     * @brief Generate rotation information
     */
    class Trackball {
    public:
        Trackball(const Vector2i &size);

        void beginDrag(const Vector2i &position);
        void drag(const Vector2i &position);
        void endDrag(const Vector2i &position);

        void resize(const Vector2i &size);

        static Rotation<float> computeRotation() ;

        Vector2i getDragBegin() const {
            return dragBegin;
        }

        Vector2i getDragEnd() const {
            return dragEnd;
        }

        Vector2i getDragCurrent() const {
            return dragCurrent;
        }

        VirtualSphere getVirtualSphere() const {
            return vsphere;
        }

    private:
        Vector2 scalePosition(const Vector2i &position) const;

        static Vector3 computeSpherePosition(const Vector2 &scaledPosition) ;

    
        VirtualSphere vsphere;
        Vector2i dragBegin;
        Vector2i dragEnd;
        Vector2i dragCurrent;
    };
} // namespace XE

#endif
