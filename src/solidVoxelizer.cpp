#include <common.h>
#include <sdf.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>

void writePointCloud(std::vector<glm::vec3> &, const std::string);

int main(int argc, char const *argv[]) {
  std::vector<glm::vec3> pointCloud;

  // Mesh mesh = loadObj("./model/cube.obj");
  // Mesh mesh = loadObj("./model/cube30d.obj");
  // Mesh mesh = loadObj("./model/monkey.obj");
  Mesh mesh = loadObj("./model/sphere.obj");
  mesh.translate(glm::vec3(2, 2, 2));
  mesh.scale(glm::vec3(1, 1, 1));

  glm::ivec3 grid(20, 20, 20); // grid index
  int WND_WIDTH = grid.x, WND_HEIGHT = grid.y;
  float cellSize = 0.15f;

  for (int z = 0; z < grid.z; z++) {
    for (int x = 0; x < grid.x; x++) {
      for (int y = 0; y < grid.y; y++) {
        glm::vec3 P = glm::vec3(x, y, z) * cellSize; // grid position
        float dist = 9999.f;

        // iterate vertices
        for (size_t i = 0; i < mesh.faces.size(); i++) {
          glm::ivec4 face = mesh.faces[i];

          glm::vec3 A, B, C, N;
          A = mesh.vertices[face[0]];
          B = mesh.vertices[face[1]];
          C = mesh.vertices[face[2]];
          N = mesh.faceNormals[face[3]];

          float temp = distPoint2Triangle(A, B, C, N, P);
          dist = (glm::abs(temp) < glm::abs(dist)) ? temp : dist;
        } // end iterate vertices

        // use sdf3d as a solid voxelier
        // if dist < threshold, output grid position
        float threshold = 0.f;
        if (dist < threshold) {
          pointCloud.push_back(P);
        }
      } // end iterate y
    }   // end iterate x
  }

  writePointCloud(pointCloud, "output.txt");

  return 0;
}

void writePointCloud(std::vector<glm::vec3> &pointCloud,
                     const std::string fileName) {
  std::ofstream output(fileName);

  for (size_t i = 0; i < pointCloud.size(); i++) {
    output << pointCloud[i].x;
    output << " ";
    output << pointCloud[i].y;
    output << " ";
    output << pointCloud[i].z;
    output << '\n';
  }

  output.close();
}
