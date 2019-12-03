#include "common.h"
#include "sdf.h"

int main(int argc, char const *argv[]) {

  vec3 A(1, -0.86, -0.5), B(-1, 0.86, 0.5), C(-1, -0.86, -0.5);
  vec3 P1(0, -1.615, 1.327);
  vec3 P2(0.481, 0.344, 0.717);
  vec3 P3(-1.372, 1.374, 0.84);

  float dist1 = distPoint2Triangle(A, B, C, P1);
  std::cout << "P1 dist = " << dist1 << '\n';
  std::cout << '\n';
  // std::cout << "answer = " << dot(vec3(0, -0.502, 0.864), P1 - A) << '\n';

  float dist2 = distPoint2Triangle(A, B, C, P2);
  std::cout << "P2 dist = " << dist2 << '\n';
  std::cout << '\n';

  float dist3 = distPoint2Triangle(A, B, C, P3);
  std::cout << "P3 dist = " << dist3 << '\n';
  std::cout << '\n';

  // std::cout << "dist = "
  //           << distPoint2Triangle(vec3(1, 2, 0), vec3(5, 3, 0), vec3(2, 4,
  //           0),
  //                                 vec3(1, 2.5, 10))
  //           << '\n';

  return 0;
}
