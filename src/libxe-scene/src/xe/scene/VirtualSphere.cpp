
#include "xe/math/Vector.h"
#include <cmath>
#include <xe/scene/VirtualSphere.h>

namespace XE {
    VirtualSphere::VirtualSphere() {
    }

    VirtualSphere::VirtualSphere(const Vector2i &screenSize) {
        this->setScreenSize(screenSize);
    }

    void VirtualSphere::setScreenSize(const Vector2i &value) {
        screenSize = value;
    }

    Vector3 VirtualSphere::computePointAt(const Vector2i &screenPosition) const {
        // map position to the [-1, 1]^2 range domain
        const auto position = Vector2{screenPosition};
        const auto size = Vector2{screenSize};
        const auto sp = (position / (0.5F * size)) - Vector2{1.0F};

        // compute the corresponding point in the surface of the sphere
        const auto sp_length_squared = norm2(sp);

        if (sp_length_squared > 1.0F) {
            return normalize(Vector3{sp, 0.0F});
        }

        return {sp, std::sqrt(1.0F - sp_length_squared)};
    }

    bool VirtualSphere::operator==(const VirtualSphere &rhs) const {
        return screenSize == rhs.screenSize;
    }
} // namespace XE
