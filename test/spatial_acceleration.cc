#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <geometry/polygon.h>
#include <spatial_acceleration/geometry.h>
#include <spatial_acceleration/bound.h>
#include <spatial_acceleration/bvh_tree.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: obj_file" << std::endl;
    return 1;
  }
  
  typedef Polygon<3, float> polygon_t;

  std::vector<std::shared_ptr<Boundable<3, float>>> polygons;

  std::cout << "Loading obj file " << argv[1] << std::endl;

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, argv[1]);

  if (!err.empty()) { // `err` may contain warning message.
    std::cerr << err << std::endl;
  }

  if (!ret) {
    exit(1);
  }

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      unsigned int fv = shapes[s].mesh.num_face_vertices[f];

      std::shared_ptr<Bounded<polygon_t>> p = std::make_shared<Bounded<polygon_t>>(fv);
      polygons.push_back(p);

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        
        float vx, vy, vz, nx, ny, nz, tx, ty = 0;

        if (idx.vertex_index != -1) {
          vx = attrib.vertices[3*idx.vertex_index+0];
          vy = attrib.vertices[3*idx.vertex_index+1];
          vz = attrib.vertices[3*idx.vertex_index+2];
        }

        if (idx.normal_index != -1) {
          nx = attrib.normals[3*idx.normal_index+0];
          ny = attrib.normals[3*idx.normal_index+1];
          nz = attrib.normals[3*idx.normal_index+2];
        }

        if (idx.texcoord_index != -1) { 
          tx = attrib.texcoords[2*idx.texcoord_index+0];
          ty = attrib.texcoords[2*idx.texcoord_index+1];
        }

        p->points.emplace_back(vx, vy, vz);
      }
      index_offset += fv;

      // per-face material
      shapes[s].mesh.material_ids[f];
    }
  }

  BVHTree<3, float> tree;
  tree.build(polygons);

  return 0;
}