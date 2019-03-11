
#ifndef __XE_SCENE_TRACKBALL_HPP__
#define __XE_SCENE_TRACKBALL_HPP__

#include <XE/Math/Vector.hpp>
#include <XE/Math/Rotation.hpp>

namespace XE {
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
        Vector2i size;
        Vector2i dragBegin;
        Vector2i dragEnd;
        Vector2i dragCurrent;
    };
}

#endif
