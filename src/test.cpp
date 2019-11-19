#include "common.h"

int main(int argc, char const *argv[]) {
  vec3 a(1, 2, 0), b(5, 3, 0), c(2, 4, 0), p(3, 3, 0);

  vec3 ab = b - a;
  vec3 ac = c - a;
  vec3 bc = c - b;

  vec3 ap = p - a;
  vec3 bp = p - b;

  float Sabc = length(cross(ab, ac)) * 0.5f;
  float Sabp = length(cross(ap, ab)) * 0.5f;
  float Sbcp = length(cross(bp, bc)) * 0.5f;
  float Sacp = length(cross(ap, ac)) * 0.5f;

  // std::cout << "Sabc = " << Sabc << '\n';
  // std::cout << "Sabp = " << Sabp << '\n';
  // std::cout << "Sbcp = " << Sbcp << '\n';
  // std::cout << "Sacp = " << Sacp << '\n';

  float u, v, w;
  u = Sbcp / Sabc;
  v = Sacp / Sabc;
  w = Sabp / Sabc;

  std::cout << "The barycentric coordinate of P is "
            << glm::to_string(vec3(u, v, w)) << '\n';

  return 0;
}
