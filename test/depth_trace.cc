#include <unistd.h>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION

#include <chrono>
#include <geometry/polygon.h>
// #include <intersection/geometry.h>
#include <spatial_acceleration/geometry.h>
#include <importers/obj_loader.h>
#include <spatial_acceleration/bound.h>
#include <spatial_acceleration/bvh_tree.h>
#include <raytrace/context.h>
#include <raytrace/camera.h>

#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfOutputFile.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: obj_file" << std::endl;
    return 1;
  }

  std::chrono::time_point<std::chrono::steady_clock> start, end;

  std::cout << "Loading obj file " << argv[1] << std::endl;
  start = std::chrono::steady_clock::now();
  std::vector<Polygon> polygons = LoadObj<Polygon>(argv[1]);
  end = std::chrono::steady_clock::now();
  std::cout << "Elapsed: " << std::chrono::duration <double, std::milli>(end - start).count() << "ms" << std::endl;
  std::cout << std::endl;

  rt::Context rtContext;
  std::cout << "Building tree for " << polygons.size() << " polygons" << std::endl;
  start = std::chrono::steady_clock::now();
  rtContext.initialize(polygons);
  end = std::chrono::steady_clock::now();
  std::cout << "Elapsed: " << std::chrono::duration <double, std::milli>(end - start).count() << "ms" << std::endl;
  std::cout << std::endl;

  rt::Camera<::Camera> camera;
  camera.width = 1000;
  camera.height = 1000;
  camera.fovy = 45 * 3.14159265358979 / 180;

  accel::Bound bound;
  for (const Polygon& poly : polygons) {
    for (const glm::vec3 &p : poly.positions) {
      bound.merge(p);
    }
  }

  camera.tgt = glm::vec3(
    bound.max(0) + bound.min(0),
    bound.max(1) + bound.min(1),
    2*bound.max(2)
  ) / 2.f;
  camera.eye = camera.tgt + glm::vec3(0,0,1) * bound.extent(1) / 2.f / std::atan(camera.fovy / 2.f);

  std::vector<Ray> rays = camera.GenerateRays(1);

  #ifdef RAY_PACKETS
  std::vector<RayPacket<4>> ray_packets((rays.size() + 3) / 4);
  RaysToRayPackets(rays.data(), ray_packets.data(), rays.size());
  
  std::cout << ray_packets.size() << " ray packets" << std::endl;
  #endif

  std::cout << "Tracing " << rays.size() << " rays" << std::endl;
  
  start = std::chrono::steady_clock::now();
  #ifdef RAY_PACKETS
  std::vector<Intersection> intersections = rtContext.trace(ray_packets);
  #else
  std::vector<Intersection> intersections = rtContext.trace(rays);
  #endif
  end = std::chrono::steady_clock::now();
  std::cout << "Elapsed: " << std::chrono::duration <double, std::milli>(end - start).count() << "ms" << std::endl;
  std::cout << std::endl;

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

  // pid_t pid = fork();
  // if (pid == 0) {
  //   std::string command = std::string(EXRDISPLAY_PATH " -c Z -n -u ");
  //   command.insert(command.size(), fname);
  //   exit(system(command.c_str()));
  // }
  exit(0);
}