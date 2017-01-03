#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION

#include <geometry/polygon.h>
#include <intersection/geometry.h>
#include <spatial_acceleration/geometry.h>
#include <importers/obj_loader.h>
#include <spatial_acceleration/bound.h>
#include <spatial_acceleration/bvh_tree.h>
#include <raytrace/context.h>

#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfOutputFile.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: obj_file" << std::endl;
    return 1;
  }

  std::vector<std::shared_ptr<IntersectableInterface>> polygons;

  std::cout << "Loading obj file " << argv[1] << std::endl;
  polygons = LoadObj<IntersectableInterface, Intersectable<Polygon>>(argv[1]);

  rt::Context rtContext;
  rtContext.initialize(polygons);

  rt::Camera<::Camera> camera;
  camera.width = 800;
  camera.height = 600;
  camera.fovy = 45 * 3.14159265358979 / 180;

  Bound& bound = rtContext.bvhTree.root.bound;

  camera.tgt = glm::vec3(
    bound.max(0) + bound.min(0),
    bound.max(1) + bound.min(1),
    bound.max(2) + bound.min(2)
  ) / 2.f;
  camera.eye = camera.tgt + glm::vec3(0,0,1) * bound.extent(1) / 2.f / std::atan(camera.fovy / 2.f);

  std::vector<Ray> rays = std::move(camera.GenerateRays(1));

  std::cout << "Tracing " << rays.size() << " rays" << std::endl;

  std::vector<Intersection> intersections = std::move(rtContext.trace(rays));
  
  float min = std::numeric_limits<float>::max();
  float max = std::numeric_limits<float>::lowest();

  for (unsigned int i = 0; i < intersections.size(); ++i) {
    if (intersections[i].hit) {
      min = std::min(min, intersections[i].t);
      max = std::max(max, intersections[i].t);
    }
  }

  std::vector<float> depths(intersections.size());

  for (unsigned int i = 0; i < intersections.size(); ++i) {
    if (intersections[i].hit) {
      depths[i] = (max - intersections[i].t) / (max - min);
    } else {
      depths[i] = 0;
    }
  }

  Imf::Header header(camera.width, camera.height);
  header.channels().insert ("Z", Imf::Channel(Imf::FLOAT));

  std::string fname = std::string(argv[1]) + ".exr";
  Imf::OutputFile file(fname.c_str(), header); 

  Imf::FrameBuffer frameBuffer; 
  frameBuffer.insert("Z",          // name
    Imf::Slice(Imf::FLOAT,         // type
    (char*)depths.data(),          // base
    sizeof(float)*1,               // xStride
    sizeof(float)*camera.width));  // yStride

  file.setFrameBuffer(frameBuffer);
  file.writePixels(camera.height);

  return 0;
}