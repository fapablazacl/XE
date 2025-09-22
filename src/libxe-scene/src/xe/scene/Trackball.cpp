
#include "xe/math/Rotation.h"
#include "xe/math/Vector.h"
#include <cmath>
#include <xe/scene/Trackball.h>

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
        vsphere.setScreenSize(size);
    }

    Rotation<float> Trackball::computeRotation() {
        return {0.0F, {0.0F, 0.0F, 0.0F}};
    }

    Vector2 Trackball::scalePosition(const Vector2i &position) const {
        // map position to the [-1, 1]^2 range domain
        const Vector2 position_f = position;
        const Vector2 size_f = vsphere.getScreenSize();
        const Vector2 mappedPosition = (position_f / (0.5F * size_f)) - Vector2{1.0F};

        return mappedPosition;
    }

    Vector3 Trackball::computeSpherePosition(const Vector2 &scaledPosition) {
        const auto sp = scaledPosition;
        const auto sp_length_squared = norm2(sp);

        if (sp_length_squared > 1.0F) {
            return normalize(Vector3{sp, 0.0F});
        }

        return {sp, std::sqrt(1.0F - sp_length_squared)};
    }
} // namespace XE
