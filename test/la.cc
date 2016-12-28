#include <iostream>
#include <core/vector.h>
#include <core/la.h>

int main(int argc, char** argv) {

  typedef Vector<3, float> vec_t;

  vec_t v1(1.f,0.f,0.f);
  vec_t v2(0.f,1.f,0.f);
  vec_t v3 = Cross(v1, v2); 
  std::cout << v1 << std::endl;
  std::cout << v2 << std::endl;
  std::cout << v3 << std::endl;

  std::cout << Dot(v1, v2) << std::endl;

  return 0;
}