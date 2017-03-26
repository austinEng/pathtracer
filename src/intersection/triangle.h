
#pragma once
#include <bitset>
#include <core/util.h>
#include <spatial_acceleration/triangle.h>
#include "ray.h"
#include "intersection.h"

template <unsigned int N>
void Intersect(const accel::TriangleGroup<N> &tris, const Ray &ray, Intersection &intersection) {
    SIMD::Float<3*N> e0 = tris.positions[1].vals - tris.positions[0].vals;
    SIMD::Float<3*N> e1 = tris.positions[2].vals - tris.positions[0].vals;
    SIMD::Float<3*N> T;
    
    #define X i
    #define Y i+N
    #define Z i+2*N

    for (unsigned int i = 0; i < N; ++i) T[X] = ray.pos.x - tris.positions[0].xs[i];
    for (unsigned int i = 0; i < N; ++i) T[Y] = ray.pos.y - tris.positions[0].ys[i];
    for (unsigned int i = 0; i < N; ++i) T[Z] = ray.pos.z - tris.positions[0].zs[i];

    SIMD::Float<3*N> P, Q; // cross(ray.dir, e2); cross(T, e1)
    for (unsigned int i = 0; i < N; ++i) {
        P[X] = ray.dir.y*e1[Z] - ray.dir.z*e1[Y];
        P[Y] = ray.dir.z*e1[X] - ray.dir.x*e1[Z];
        P[Z] = ray.dir.x*e1[Y] - ray.dir.y*e1[X];

        Q[X] = T[Y]*e0[Z] - T[Z]*e0[Y];
        Q[Y] = T[Z]*e0[X] - T[X]*e0[Z];
        Q[Z] = T[X]*e0[Y] - T[Y]*e0[X];
    }

    SIMD::Float<N> det, denom, u, v, t;

    for (unsigned int i = 0; i < N; ++i) det[i] = e0[X] * P[X] + e0[Y] * P[Y] + e0[Z] * P[Z];

    denom = 1.f / det;

    for (unsigned int i = 0; i < N; ++i) u[i] = T[X]*P[X] + T[Y]*P[Y] + T[Z]*P[Z];
    u *= denom;

    for (unsigned int i = 0; i < N; ++i) v[i] = Q[X]*ray.dir.x + Q[Y]*ray.dir.y + Q[Z]*ray.dir.z;
    v *= denom;

    for (unsigned int i = 0; i < N; ++i) t[i] = Q[X]*e1[X] + Q[Y]*e1[Y] + Q[Z]*e1[Z];
    t *= denom;

    #undef X
    #undef Y
    #undef Z

    std::bitset<N> hit_mask;
    for (unsigned int i = 0; i < N; ++i) hit_mask[i] = (u[i] >= 0 && u[i] < 1.f && v[i] >= 0 && u[i]+v[i] < 1.f && !fequal(0.f, det[i]));

    int closest = -1;
    for (unsigned int i = 0; i < N; ++i) {
        if (hit_mask[i] && (!intersection.hit || t[i] < intersection.t)) {
            closest = i;
            intersection.t = t[i];
        }
    }
    intersection.hit |= hit_mask.any();
    if (closest > 0) {
        intersection.point = ray.pos + intersection.t * ray.dir;
    }
}