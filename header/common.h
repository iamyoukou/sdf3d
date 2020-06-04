#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

using namespace std;
using namespace glm;

/* Define a 3D triangle */
typedef struct {
  vec3 v1, v2, v3;
} Triangle;

/* Define a 3D point */
typedef struct {
  vec3 pos;
  vec3 color;
  vec3 v;
  float m;
} Point;

/* Define a particle system */
class Particles {
public:
  std::vector<Point> Ps;
  GLuint vao, vboPos, vboColor;

  /* Constructors */
  Particles() {}
  ~Particles() {
    glDeleteBuffers(1, &vboPos);
    glDeleteBuffers(1, &vboColor);
    glDeleteBuffers(1, &vao);
  }
};

typedef struct {
  // data index
  GLuint v1, v2, v3;
  GLuint vt1, vt2, vt3;
  GLuint vn1, vn2, vn3;
} Face;

class Mesh {
public:
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> faceNormals;
  std::vector<Face> faces;

  // opengl data
  GLuint vboVtxs, vboUvs, vboNormals;
  GLuint vao;

  // aabb
  glm::vec3 min, max;

  /* Constructors */
  Mesh(){};
  ~Mesh() {
    glDeleteBuffers(1, &vboVtxs);
    glDeleteBuffers(1, &vboUvs);
    glDeleteBuffers(1, &vboNormals);
    glDeleteVertexArrays(1, &vao);
  };

  /* Member functions */
  void translate(glm::vec3);
  void scale(glm::vec3);
  void rotate(glm::vec3);
};

std::string readFile(const std::string);
Mesh loadObj(std::string);
GLuint buildShader(string, string);
GLuint compileShader(string, GLenum);
GLuint linkShader(GLuint, GLuint);
void createMesh(Mesh &);
void printLog(GLuint &);
GLint myGetUniformLocation(GLuint &, std::string);
void findAABB(Mesh &);
void drawBox(glm::vec3, glm::vec3);
void updateMesh(Mesh &);
void drawTriangle(Triangle &);
void drawLine(vec3, vec3);
void drawPoints(std::vector<Point> &);
