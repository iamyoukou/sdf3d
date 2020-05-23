#include <iostream>
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

class Mesh {
public:
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> faceNormals;
  std::vector<glm::ivec4> faces;

  // aabb
  glm::vec3 min, max;

  /* Constructors */
  Mesh(){};
  ~Mesh(){};

  /* Member functions */
  void translate(glm::vec3);
  void scale(glm::vec3);
  void rotate(glm::vec3);
};

std::string readFile(const std::string);
Mesh loadObj(std::string);
GLuint createShader(std::string, GLenum);
void printLog(GLuint &);
GLint myGetUniformLocation(GLuint &, std::string);
void keyCallback(GLFWwindow *, int, int, int, int);
void findAABB(Mesh &);
void drawBox(glm::vec3, glm::vec3);
