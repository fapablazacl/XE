
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

    Vector2f Trackball::scalePosition(const Vector2i &position) const {
        // map position to the [-1, 1]^2 range domain
        const Vector2f position_f = position;
        const Vector2f size_f = size;
        const Vector2f mappedPosition = (position_f / (0.5f * size_f)) - Vector2f{1.0f};

        return mappedPosition;
    }

     Vector3f Trackball::computeSpherePosition(const Vector2f &scaledPosition) const {
         const auto sp = scaledPosition;
         const auto sp_length_squared = norm2(sp);

         if (sp_length_squared > 1.0f) {
             return normalize(Vector3f{sp, 0.0f});
         }

         return {sp, std::sqrt(1.0f - sp_length_squared)};
     }
}
