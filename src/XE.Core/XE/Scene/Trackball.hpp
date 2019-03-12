
#ifndef __XE_SCENE_TRACKBALL_HPP__
#define __XE_SCENE_TRACKBALL_HPP__

#include <XE/Math/Vector.hpp>
#include <XE/Math/Rotation.hpp>

namespace XE {

    /**
     * @brief Virtual sphere living into a screen. Used as an utility in the Trackball class.
     */
    class VirtualSphere {
    public:
        VirtualSphere(const Vector2i &screenSize, const float radius);

        Vector2i getScreenSize() const;

        void setScreenSize(const Vector2i &value);

        float getRadius() const;

        void setRadius(const float value);

        Vector3f computePointAt(const Vector2i &screenPosition) const;

    private:
        Vector2i screenSize;
        float radius;
    };

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

        Rotation<float> computeRotation() const;

        Vector2i getDragBegin() const {
            return dragBegin;
        }

        Vector2i getDragEnd() const {
            return dragEnd;
        }

        Vector2i getDragCurrent() const {
            return dragCurrent;
        }

        Vector2i getSize() const {
            return size;
        }

    private:
        Vector2f scalePosition(const Vector2i &position) const;

        Vector3f computeSpherePosition(const Vector2f &normalizedPosition) const;

    private:
        Vector2i size;
        Vector2i dragBegin;
        Vector2i dragEnd;
        Vector2i dragCurrent;
    };
}

#endif
