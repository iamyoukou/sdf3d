#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <GLFW/glfw3.h>

using namespace std;
using namespace glm;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct vertex_info_t {
  int vertexIndex;
  vec3 vertexCoordinate;
  vec3 vertexNormal;
  vector<int> connectedFaceIndex;
};

struct face_info_t {
  int faceIndex;
  ivec4 containedVertexIndex;
  vec3 faceNormal;
};

struct mesh_info_t {
  vector<vertex_info_t> vertexTable;
  vector<face_info_t> faceTable;
};

string read_file(const string);
mesh_info_t load_obj(string);
GLuint create_shader(string, GLenum);
void printLog(GLuint &);
vector<ivec3> quad2tri(mesh_info_t &);
GLint myGetUniformLocation(GLuint &, string);
void keyCallback(GLFWwindow *, int, int, int, int);
