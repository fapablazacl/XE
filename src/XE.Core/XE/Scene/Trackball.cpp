
#include "Trackball.hpp"

namespace XE {
    Trackball::Trackball(const Vector2i &size) {
        this->resize(size);
    }

    void Trackball::beginDrag(const Vector2i &position) {
        dragBegin = position;
    }

    void Trackball::drag(const Vector2i &position) {
        dragCurrent = position;
    }

    void Trackball::endDrag(const Vector2i &position) {
        dragEnd = position;
    }

    void Trackball::resize(const Vector2i &size) {
        this->size = size;
    }

    Rotation<float> Trackball::computeRotation() const {
        return {0.0f, {0.0f, 0.0f, 0.0f}};
    }
}
