#include "common.h"
#include "sdf.h"

int main(int argc, char const *argv[]) {

  // glm::vec3 A(1, -0.86, -0.5), B(-1, 0.86, 0.5), C(-1, -0.86, -0.5);
  // glm::vec3 P1(0, -1.615, 1.327);
  // glm::vec3 P2(0.481, 0.344, 0.717);
  // glm::vec3 P3(-1.372, 1.374, 0.84);
  //
  // glm::vec3 N1 = glm::normalize(glm::cross(B - A, P1 - A));
  // float dist1 = distPoint2Triangle(A, B, C, N1, P1);
  // std::cout << "P1 dist = " << dist1 << '\n';
  // std::cout << '\n';

  // float dist2 = distPoint2Triangle(A, B, C, P2);
  // std::cout << "P2 dist = " << dist2 << '\n';
  // std::cout << '\n';
  //
  // float dist3 = distPoint2Triangle(A, B, C, P3);
  // std::cout << "P3 dist = " << dist3 << '\n';
  // std::cout << '\n';
  //
  // std::cout << "dist = "
  //           << distPoint2Triangle(vec3(1, 2, 0), vec3(5, 3, 0), vec3(2, 4,
  //           0),
  //                                 vec3(1, 2.5, 10))
  //           << '\n';

  Mesh mesh = loadObj("./model/cube.obj");

  glm::vec3 P(0.8, 0.5, 1.5);
  float dist = 9999.f;

  // iterate each triangle
  for (size_t i = 0; i < mesh.faces.size(); i++) {
    glm::ivec4 face = mesh.faces[i];

    glm::vec3 A, B, C, N;
    A = mesh.vertices[face[0]];
    B = mesh.vertices[face[1]];
    C = mesh.vertices[face[2]];
    N = mesh.faceNormals[face[3]];

    float temp = distPoint2Triangle(A, B, C, N, P);
    dist = (glm::abs(temp) < glm::abs(dist)) ? temp : dist;
    std::cout << "triangle " << (i + 1) << ", temp = " << temp << '\n';
    std::cout << '\n';
  }
  std::cout << "dist = " << dist << '\n';

  return 0;
}
