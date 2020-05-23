#include <common.h>
#include <sdf.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>

GLFWwindow *window;

vec3 lightPosition = vec3(3.f, 3.f, 3.f);
vec3 lightColor = vec3(1.f, 1.f, 1.f);
float lightPower = 1.f;

/* for view control */
float verticalAngle = -2.76603;
float horizontalAngle = 1.56834;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;

mat4 model, view, projection;
vec3 eyePoint = vec3(0.106493, 3.517007, 1.688342);
vec3 eyeDirection =
    vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
         sin(verticalAngle) * sin(horizontalAngle));
vec3 up = vec3(0.f, 1.f, 0.f);

/* opengl variables */
GLuint exeShader;
GLuint tboBase, tboNormal;
GLint uniM, uniV, uniP, uniMvp;
GLint uniLightColor, uniLightPosition, uniLightPower;
GLint uniTexBase;
GLint uniEyePoint;

void computeMatricesFromInputs(mat4 &, mat4 &);
void keyCallback(GLFWwindow *, int, int, int, int);

void initGL();
void initMatrix();
void initLight();
void initShader();
void releaseResource();

void writePointCloud(std::vector<glm::vec3> &, const std::string);

int main(int argc, char const *argv[]) {
  initGL();
  initShader();
  initMatrix();
  initLight();

  std::vector<glm::vec3> pointCloud;

  Mesh mesh = loadObj("./mesh/sphere.obj");
  initMesh(mesh);
  findAABB(mesh);

  // mesh.translate(glm::vec3(2, 2, 2));
  // mesh.scale(glm::vec3(1, 1, 1));

  // glm::ivec3 grid(20, 20, 20); // grid index
  // int WND_WIDTH = grid.x, WND_HEIGHT = grid.y;
  // float cellSize = 0.15f;

  // for the entire grid
  // for (int z = 0; z < grid.z; z++) {
  //   for (int x = 0; x < grid.x; x++) {
  //     for (int y = 0; y < grid.y; y++) {
  //       glm::vec3 P = glm::vec3(x, y, z) * cellSize; // grid position
  //       float dist = 9999.f;
  //
  //       // iterate vertices
  //       for (size_t i = 0; i < mesh.faces.size(); i++) {
  //         glm::ivec4 face = mesh.faces[i];
  //
  //         glm::vec3 A, B, C, N;
  //         A = mesh.vertices[face[0]];
  //         B = mesh.vertices[face[1]];
  //         C = mesh.vertices[face[2]];
  //         N = mesh.faceNormals[face[3]];
  //
  //         float temp = distPoint2Triangle(A, B, C, N, P);
  //         dist = (glm::abs(temp) < glm::abs(dist)) ? temp : dist;
  //       } // end iterate vertices
  //
  //       // use sdf3d as a solid voxelier
  //       // if dist < threshold, output grid position
  //       float threshold = 0.f;
  //       if (dist < threshold) {
  //         pointCloud.push_back(P);
  //       }
  //     } // end iterate y
  //   }   // end iterate x
  // }
  //
  // writePointCloud(pointCloud, "output.txt");

  /* glfw loop */
  // a rough way to solve cursor position initialization problem
  // must call glfwPollEvents once to activate glfwSetCursorPos
  // this is a glfw mechanism problem
  glfwPollEvents();
  glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    // reset
    glClearColor(0.f, 0.f, 0.4f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view control
    computeMatricesFromInputs(projection, view);
    glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(projection));
    glUniform3fv(uniEyePoint, 1, value_ptr(eyePoint));

    // draw mesh
    glBindVertexArray(mesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, mesh.faces.size() * 3);
    drawBox(mesh.min, mesh.max);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  releaseResource();

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

void initGL() { // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    exit(EXIT_FAILURE);
  }

  // without setting GLFW_CONTEXT_VERSION_MAJOR and _MINOR，
  // OpenGL 1.x will be used
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  // must be used if OpenGL version >= 3.0
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "With normal mapping",
                            NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to open GLFW window." << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  /* Initialize GLEW */
  // without this, glGenVertexArrays will report ERROR!
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST); // must enable depth test!!
}

void initMatrix() {
  // transform matrix
  uniM = myGetUniformLocation(exeShader, "M");
  uniV = myGetUniformLocation(exeShader, "V");
  uniP = myGetUniformLocation(exeShader, "P");

  model = translate(mat4(1.f), vec3(0.f, 0.f, 0.f));
  view = lookAt(eyePoint,     // eye position
                eyeDirection, // look at
                up            // up
  );

  projection =
      perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.f);

  glUniformMatrix4fv(uniM, 1, GL_FALSE, value_ptr(model));
  glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(projection));

  uniEyePoint = myGetUniformLocation(exeShader, "eyePoint");
  glUniform3fv(uniEyePoint, 1, value_ptr(eyePoint));
}

void initLight() { // light
  uniLightColor = myGetUniformLocation(exeShader, "lightColor");
  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));

  uniLightPosition = myGetUniformLocation(exeShader, "lightPosition");
  glUniform3fv(uniLightPosition, 1, value_ptr(lightPosition));

  // uniLightPower = myGetUniformLocation(exeShader, "lightPower");
  // glUniform1f(uniLightPower, lightPower);
}

void initShader() {
  // build shader program
  exeShader = buildShader("./shader/vsPhong.glsl", "./shader/fsPhong.glsl");
  glUseProgram(exeShader);
}

void releaseResource() { glfwTerminate(); }

void computeMatricesFromInputs(mat4 &newProject, mat4 &newView) {
  // glfwGetTime is called only once, the first time this function is called
  static float lastTime = glfwGetTime();

  // Compute time difference between current and last frame
  float currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  // std::cout << xpos << ", " << ypos << '\n';

  // Reset mouse position for next frame
  glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  // Compute new orientation
  // The cursor is set to the center of the screen last frame,
  // so (currentCursorPos - center) is the offset of this frame
  horizontalAngle += mouseSpeed * float(xpos - WINDOW_WIDTH / 2.f);
  verticalAngle += mouseSpeed * float(-ypos + WINDOW_HEIGHT / 2.f);

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  vec3 direction =
      vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
           sin(verticalAngle) * sin(horizontalAngle));

  // Right vector
  vec3 right = vec3(cos(horizontalAngle - 3.14 / 2.f), 0.f,
                    sin(horizontalAngle - 3.14 / 2.f));

  // new up vector
  vec3 newUp = cross(right, direction);

  // Move forward
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    eyePoint += direction * deltaTime * speed;
  }
  // Move backward
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    eyePoint -= direction * deltaTime * speed;
  }
  // Strafe right
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    eyePoint += right * deltaTime * speed;
  }
  // Strafe left
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    eyePoint -= right * deltaTime * speed;
  }

  // float FoV = initialFoV;
  newProject =
      perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.f);
  // Camera matrix
  newView = lookAt(eyePoint, eyePoint + direction, newUp);

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}
