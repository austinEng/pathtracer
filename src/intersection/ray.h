
#pragma once

#include <glm/vec3.hpp>
#include <ae_core/simd/types.h>
#include <bitset>

struct Ray {
  glm::vec3 pos;
  glm::vec3 dir;

  Ray() { }
  Ray(const glm::vec3 &pos, const glm::vec3 &dir) : pos(pos), dir(dir) { }

};

template <unsigned int N>
struct RayPacket {
  struct pos_t {
    SIMD::Float<N> x;
    SIMD::Float<N> y;
    SIMD::Float<N> z;
  };
  struct dir_t {
    SIMD::Float<N> x;
    SIMD::Float<N> y;
    SIMD::Float<N> z;
  };
  pos_t pos;
  dir_t dir;
  std::bitset<N> mask;
};

template <unsigned int N>
static void RaysToRayPackets(const Ray* const rays, RayPacket<N>* const packets, unsigned int count) {
  for (unsigned int i = 0; i < count; i += N) {
    for (unsigned int j = 0; j < N; ++j) {
      unsigned int ray_index = i + j;
      unsigned int packet_index = i / N;
      float valid = ray_index < count;
      packets[packet_index].mask.set(j, valid);
      if (valid) {
        packets[packet_index].pos.x[j] = rays[ray_index].pos.x;
        packets[packet_index].pos.y[j] = rays[ray_index].pos.y;
        packets[packet_index].pos.z[j] = rays[ray_index].pos.z;
        packets[packet_index].dir.x[j] = rays[ray_index].dir.x;
        packets[packet_index].dir.y[j] = rays[ray_index].dir.y;
        packets[packet_index].dir.z[j] = rays[ray_index].dir.z;
      }
    }
  }
}