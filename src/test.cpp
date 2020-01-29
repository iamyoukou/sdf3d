#include <common.h>
#include <sdf.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

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

  // Mesh mesh = loadObj("./model/cube.obj");
  Mesh mesh = loadObj("./model/cube30d.obj");
  // Mesh mesh = loadObj("./model/cubeBig.obj");
  // mesh.scale(glm::vec3(10, 10, 10));
  mesh.translate(glm::vec3(5, 5, 5));

  // glm::vec3 P(0.8, 0.5, 1.5);
  // glm::vec3 Prot(0.8, -0.317, 1.55);
  // float dist = 9999.f;

  // iterate each triangle
  // for (size_t i = 0; i < mesh.faces.size(); i++) {
  //   glm::ivec4 face = mesh.faces[i];
  //
  //   glm::vec3 A, B, C, N;
  //   A = mesh.vertices[face[0]];
  //   B = mesh.vertices[face[1]];
  //   C = mesh.vertices[face[2]];
  //   N = mesh.faceNormals[face[3]];
  //
  //   // float temp = distPoint2Triangle(A, B, C, N, P);
  //   float temp = distPoint2Triangle(A, B, C, N, Prot);
  //   dist = (glm::abs(temp) < glm::abs(dist)) ? temp : dist;
  //   std::cout << "triangle " << (i + 1) << ", temp = " << temp << '\n';
  //   std::cout << '\n';
  // }
  // std::cout << "dist = " << dist << '\n';

  glm::ivec3 grid(100, 100, 100); // grid index
  int WND_WIDTH = grid.x, WND_HEIGHT = grid.y;
  float cellSize = 0.1f;
  float sdfScale = 15.f;

  for (int z = 0; z < grid.z; z++) {

    // temporarily hold sdf of (x, y, ...)
    float xySdf[100][100] = {};

    for (int x = 0; x < grid.x; x++) {
      for (int y = 0; y < grid.y; y++) {
        glm::vec3 P = glm::vec3(x, y, z) * cellSize; // grid position
        // std::cout << "P = " << glm::to_string(P) << '\n';
        float dist = 9999.f;

        // iterate vertices
        for (size_t i = 0; i < mesh.faces.size(); i++) {
          glm::ivec4 face = mesh.faces[i];

          glm::vec3 A, B, C, N;
          A = mesh.vertices[face[0]];
          B = mesh.vertices[face[1]];
          C = mesh.vertices[face[2]];
          N = mesh.faceNormals[face[3]];

          // std::cout << "A = " << glm::to_string(A) << '\n';
          // std::cout << "B = " << glm::to_string(B) << '\n';
          // std::cout << "C = " << glm::to_string(C) << '\n';
          // std::cout << "N = " << glm::to_string(N) << '\n';

          float temp = distPoint2Triangle(A, B, C, N, P);
          dist = (glm::abs(temp) < glm::abs(dist)) ? temp : dist;
          xySdf[x][y] = dist;
          // std::cout << "temp = " << temp << '\n';

        } // end iterate vertices
        // std::cout << xySdf[x][y] << '\n';
        // std::cout << '\n';

        // std::cout << glm::to_string(P) << " dist = " << dist << '\n';
      }
    }

    // to image
    cv::Mat canvas =
        cv::Mat(WND_HEIGHT, WND_WIDTH, CV_8UC3, cv::Scalar(255, 255, 255));

    for (int x = 0; x < grid.x; x++) {
      for (int y = 0; y < grid.y; y++) {

        float dist = ((xySdf[x][y] / sdfScale) + 1.f) * 0.5f;  // to [0.0, 1.0]
        int iDist = int(glm::clamp(dist * 255.f, 0.f, 255.f)); // to [0, 255]
        // std::cout << "(" << x << "," << y << "," << z << "): "
        //           << "iDist = " << iDist << '\n';

        // to window space
        // int ix = grid.x;
        // int iy = grid.y;

        cv::Vec3b &pixel = canvas.at<cv::Vec3b>(cv::Point(x, y));
        pixel = cv::Vec3b(iDist, iDist, iDist);
      }
    }
    imwrite(cv::format("./result/sim%03d.png", int(z)), canvas);
    canvas.release();
    std::cout << "level " << z << " completed." << '\n';
  }

  return 0;
}
