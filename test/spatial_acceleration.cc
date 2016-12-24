#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <spatial_acceleration/bound.h>
#include <spatial_acceleration/bvh_tree.h>

#define X 0
#define Y 1
#define Z 2

typedef Boundable<3, float> boundable_t;

class Polygon : public boundable_t {
  
  public:

  class Vertex {
    public:
    float x;
    float y;
    float z;

    Vertex() { }
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
  };

  std::vector<Vertex> vertices;
  
  Polygon(int vcount) {
    vertices = std::vector<Vertex>(vcount);
  }

  virtual Bound3f GetBound() const {
    Bound3f bound;
    bound.min(0) = vertices[0].x;
    bound.min(1) = vertices[0].y;
    bound.min(2) = vertices[0].z;
    bound.max(0) = vertices[0].x;
    bound.max(1) = vertices[0].y;    
    bound.max(2) = vertices[0].z;
    for (const Vertex& v : vertices) {
      if (v.x < bound.min(0)) bound.min(0) = v.x;
      if (v.x > bound.max(0)) bound.max(0) = v.x;
      if (v.y < bound.min(1)) bound.min(1) = v.y;
      if (v.y > bound.max(1)) bound.max(1) = v.y;
      if (v.z < bound.min(2)) bound.min(2) = v.z;
      if (v.z > bound.max(2)) bound.max(2) = v.z;
    }
    return bound;
  }

  virtual Point<3, float> GetCentroid() const {
    Point<3, float> centroid;
    for (const Vertex &v : vertices) {
      centroid.vals[0] += v.x;
      centroid.vals[1] += v.y;
      centroid.vals[2] += v.z;
    }
    centroid.vals[0] /= vertices.size();
    centroid.vals[1] /= vertices.size();
    centroid.vals[2] /= vertices.size();
    return centroid;
  }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: obj_file" << std::endl;
    return 1;
  }

  typedef boundable_t object_t;

  std::vector<std::shared_ptr<object_t>> polygons;

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

      std::shared_ptr<Polygon> p = std::make_shared<Polygon>(fv);
      polygons.push_back(p);

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        float vx = attrib.vertices[3*idx.vertex_index+0];
        float vy = attrib.vertices[3*idx.vertex_index+1];
        float vz = attrib.vertices[3*idx.vertex_index+2];

        p->vertices.emplace_back(vx, vy, vz);
        // float nx = attrib.normals[3*idx.normal_index+0];
        // float ny = attrib.normals[3*idx.normal_index+1];
        // float nz = attrib.normals[3*idx.normal_index+2];
        // float tx = attrib.texcoords[2*idx.texcoord_index+0];
        // float ty = attrib.texcoords[2*idx.texcoord_index+1];
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