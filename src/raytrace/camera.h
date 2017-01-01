
#pragma once

#include <camera/camera.h>
#include <intersection/ray.h>

namespace rt {

  template <typename C>
  class CameraBase : public C {
    public:
    virtual std::vector<Ray> GenerateRays(unsigned int spp) const = 0; 
  };
  
  template <typename C>
  class Camera;

  template <>
  class Camera<::Camera> : public CameraBase<::Camera> {
    public:
    virtual std::vector<Ray> GenerateRays(unsigned int spp) const {
      std::vector<Ray> rays;
      rays.reserve(width * height * spp);

      glm::vec3 F = glm::normalize(tgt - eye);
      glm::vec3 R = glm::cross(F, glm::vec3(0,1,0));
      glm::vec3 U = glm::cross(R, F);
      
      glm::vec3 ref = eye + F * near;
      glm::vec3 V = U*near*std::tan(fovy/2.f);
      glm::vec3 H = R*near*((float)width/height)*std::tan(fovy/2.f);

      for (unsigned int x = 0; x < width; ++x) {
        for (unsigned int y = 0; y < height; ++y) {
          for (unsigned int s = 0; s < spp; ++s) {
            glm::vec3 p = ref + ((float)2*x/width - 1) * H + (1 - (float)2*y/height) * V;
            rays.emplace_back(p, glm::normalize(p - eye));
          }
        }
      }
      return rays;
    } 
  };
}