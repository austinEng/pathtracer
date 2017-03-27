#include <unistd.h>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION

#include <chrono>
#include <geometry/polygon.h>
#include <spatial_acceleration/geometry.h>
#include <importers/obj_loader.h>
#include <spatial_acceleration/bound.h>
#include <spatial_acceleration/bvh_tree.h>
#include <raytrace/context.h>
#include <raytrace/camera.h>

#include <png/png.h>

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

  png_structp png_ptr;
  png_infop info_ptr;

  std::string file_name = std::string(argv[1]) + ".png";

  FILE *fp = fopen(file_name.c_str(), "wb");
  if (!fp) exit(1);
  
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    std::cerr << "png_create_write_struct failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    std::cerr << "png_create_info_struct failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    std::cerr << "Error during init_io" << std::endl;
    exit(EXIT_FAILURE);
  }

  png_init_io(png_ptr, fp);
  
  if (setjmp(png_jmpbuf(png_ptr))) {
    std::cerr << "Error during writing header" << std::endl;
    exit(EXIT_FAILURE);
  }

  png_set_IHDR(png_ptr, info_ptr, camera.width, camera.height,
                16, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  png_bytep row = (png_bytep) malloc(2 * camera.width * sizeof(png_byte));
  for (unsigned int y = 0; y < camera.height; y++) {
    for (unsigned int x = 0; x < camera.width; x++) {
      row[x*2] = (int) (255 * depths[y * camera.width + x]);
    }
    png_write_row(png_ptr, row);
  }

  png_write_end(png_ptr, NULL);

  fclose(fp);
  if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
  if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
  if (row != NULL) free(row);
  
  exit(0);
}