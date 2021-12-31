
#ifndef __XE_MATH_TRIANGLE_HPP__
#define __XE_MATH_TRIANGLE_HPP__

#include <array>
#include "Vector.h"

namespace XE {
    template<typename T>
    struct Triangle {
        Vector<T, 3> P1;
        Vector<T, 3> P2;
        Vector<T, 3> P3;

        Triangle() {}

        Triangle(const Vector<T, 3> &p1, const Vector<T, 3> &p2, const Vector<T, 3> &p3) {
            P1 = p1;
            P2 = p2;
            P3 = p3;
        }

        Vector<T, 3> computeNormal() const {
            return normalize(computeNormalUnnormalized());
        }
        
        Vector<T, 3> computeNormalUnnormalized() const {
            return cross((P2 - P1), P3 - P1);
        }
    };

    /*
    template<typename Vector>
    bool Triangle<Vector>::hit(const Ray& ray, IntersectInfo *info=nullptr) const {
        
        // Generar un plano con los tres puntos
        auto plane = Plane();
        
        plane.set(tri.p1, tri.p2, tri.p3);
        if (plane.intersect(ray, &info) == true) {
            if (info.distance <= 0.0) {
                continue;
            }
            
            // Comprobar si el punto de interseccion pertenece al triangulo o no, usando 
            // coordenadas baricentricas
            Vector3f r0=ray.getPointAt(info.distance);
            Vector3f p=ray.getPoint(), q=r0;
            Vector3f a=tri.p1, b=tri.p2, c=tri.p3;
            
            Vector3f pq = (q - p);
            Vector3f pa = (a - p);
            Vector3f pb = (b - p);
            Vector3f pc = (c - p);
            
            float u, v, w;
            u = pq.triple(pc, pb);
            v = pq.triple(pa, pc);
            w = pq.triple(pb, pa);
            
            // Detectar si existe colision, sin importar como esten ordenados los triangulos
            bool isBackSide = (u > 0.0f && v > 0.0f && w > 0.0f);
            bool isFrontSide = (u < 0.0f && v < 0.0f && w < 0.0f);
            
            if (isBackSide == true || isFrontSide == true) {
                if (intersectInfo != nullptr){
                    intersectInfo->intersect = true;
                    intersectInfo->distance = info.distance;
                    intersectInfo->materialPtr = meshPart.getMaterial();
                    intersectInfo->normal = tri.getNormal();
                }
                
                return true;
            } else {
                //! TODO: Comprobar mediante un epsilon
                return false;
            }
        }
    }
    */
}

#endif 
