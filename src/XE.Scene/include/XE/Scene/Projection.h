
#ifndef __XE_SCENE_PROJECTION_HPP__
#define __XE_SCENE_PROJECTION_HPP__

#include <xe/math/Vector.h>
#include <xe/math/Matrix.h>
#include <xe/graphics/Viewport.h>

namespace XE {
    /**
     * @brief Transforms a four-dimensional vector from World coordinates to Screen coordinates. 
     */
    template<typename T>
    Vector<T, 4> project(const Vector<T, 4> &world, const Matrix<T, 4, 4> &projViewModel, const Viewport &viewport) {
        const auto window = projViewModel * world;

        auto clip = window;

        // TODO: Add check with machine epsilon
        if (clip.W > T(0.0)) {
             clip /= window.W;
        }

        const auto screen = Vector<T, 4> {
            (clip.X + T(1.0)) * T(0.5) * viewport.size.X + viewport.position.X,
            (clip.Y + T(1.0)) * T(0.5) * viewport.size.Y + viewport.position.Y,
            clip.Z,
            clip.W
        };
        
        return screen;
    }

    /**
     * @brief Transforms a four-dimensional vector from Screen coordinates to World coordinates.
     */
    template<typename T>
    Vector<T, 4> unproject(const Vector<T, 4> &screen, const Matrix<T, 4, 4> &invProjViewModel, const Viewport &viewport) {
        const auto clip = Vector<T, 4> {
            (screen.X - viewport.position.X) / (T(0.5) * viewport.size.X) - T(1.0),
            (screen.Y - viewport.position.Y) / (T(0.5) * viewport.size.Y) - T(1.0),
            screen.Z,
            screen.W
        };

        auto world = invProjViewModel * clip;

        // TODO: Add check with machine epsilon
        if (world.W > T(0.0)) {
             world /= world.W;
        }

        return world;
    }
}

#endif
