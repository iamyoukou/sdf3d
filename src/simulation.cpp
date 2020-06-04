#include "common.h"
#include <ctime>
#include <cstdlib>

#include <fstream>

GLint uniParM, uniParV, uniParP;
GLint uniMeshM, uniMeshV, uniMeshP;
GLFWwindow *window;
GLuint shaderPar, shaderSphere;
Particles particles;
Mesh sphere;

void initGL();
void initShader();
void initParticles();
void initMesh();
void initMatrix();
void initUniform();
void initBuffers();
void releaseResource();
void step();
void loadPoints(Particles &, const string);
void computeMatricesFromInputs();
void keyCallback(GLFWwindow *, int, int, int, int);

float dt = 0.01;
vec3 g(0, -9.8, 0);

// for view control
float verticalAngle = -2.02955;
float horizontalAngle = 1.83207;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;

mat4 commonM, commonV, commonP;
vec3 eyePoint = vec3(0.644592, 2.937514, 0.668253);
vec3 eyeDirection =
    vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
         sin(verticalAngle) * sin(horizontalAngle));
vec3 up = vec3(0.f, 1.f, 0.f);

// random number in [-0.5, 0.5]
float randf() {
  // srand(clock());

  float f = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

  return f - 0.5f;
}

int main(int argc, char **argv) {
  initGL();
  initShader();
  initMatrix();
  initUniform();
  initParticles();
  initMesh();
  initBuffers();

  // a rough way to solve cursor position initialization problem
  // must call glfwPollEvents once to activate glfwSetCursorPos
  // this is a glfw mechanism problem
  glfwPollEvents();
  glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view control
    computeMatricesFromInputs();

    // simulation
    step();

    // draw points
    glUseProgram(shaderPar);

    glUniformMatrix4fv(uniParV, 1, GL_FALSE, value_ptr(commonV));
    glUniformMatrix4fv(uniParP, 1, GL_FALSE, value_ptr(commonP));

    drawPoints(particles);

    // draw mesh
    glUseProgram(shaderSphere);

    glUniformMatrix4fv(uniMeshV, 1, GL_FALSE, value_ptr(commonV));
    glUniformMatrix4fv(uniMeshP, 1, GL_FALSE, value_ptr(commonP));

    glBindVertexArray(sphere.vao);
    glDrawArrays(GL_TRIANGLES, 0, sphere.faces.size() * 3);

    // refresh
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  releaseResource();

  return EXIT_SUCCESS;
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
  glEnable(GL_PROGRAM_POINT_SIZE);
  glPointSize(10);
}

void initShader() {
  shaderPar = buildShader("./shader/vsPoint.glsl", "./shader/fsPoint.glsl");
  shaderSphere = buildShader("./shader/vsPhong.glsl", "./shader/fsPhong.glsl");
}

void initMatrix() {
  /* common matrix */
  commonM = translate(mat4(1.f), vec3(0.f, 0.f, -4.f));
  commonV = lookAt(eyePoint,     // eye position
                   eyeDirection, // look at
                   up            // up
  );
  commonP =
      perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 10.f);
}

void initBuffers() {
  int nOfPs = particles.Ps.size();
  GLfloat *aPos = new GLfloat[nOfPs * 3];
  GLfloat *aColor = new GLfloat[nOfPs * 3];

  // implant data
  for (size_t i = 0; i < nOfPs; i++) {
    Point &p = particles.Ps[i];

    // positions
    aPos[i * 3 + 0] = p.pos.x;
    aPos[i * 3 + 1] = p.pos.y;
    aPos[i * 3 + 2] = p.pos.z;

    // colors
    aColor[i * 3 + 0] = p.color.r;
    aColor[i * 3 + 1] = p.color.g;
    aColor[i * 3 + 2] = p.color.b;
  }

  // initialize buffer objects
  glGenVertexArrays(1, &particles.vao);
  glBindVertexArray(particles.vao);

  glGenBuffers(1, &particles.vboPos);
  glBindBuffer(GL_ARRAY_BUFFER, particles.vboPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nOfPs, aPos,
               GL_STREAM_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &particles.vboColor);
  glBindBuffer(GL_ARRAY_BUFFER, particles.vboColor);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nOfPs, aColor,
               GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // release
  delete[] aPos;
  delete[] aColor;
}

void initParticles() {
  // loadPoints(particles, "cubePointCloud.txt");
  // loadPoints(particles, "spherePointCloud.txt");
  // loadPoints(particles, "monkeyPointCloud.txt");
  // loadPoints(particles, "torusPointCloud.txt");
  // loadPoints(particles, "bunnyPointCloud.txt");
  loadPoints(particles, "test.txt");
}

void initMesh() {
  sphere = loadObj("./mesh/sphere.obj");
  createMesh(sphere);
}

void releaseResource() { glfwTerminate(); }

void step() {
  int nOfPs = particles.Ps.size();

  for (size_t i = 0; i < nOfPs; i++) {
    Point &p = particles.Ps[i];

    p.v += dt * g;
    p.pos += dt * p.v;
  }
}

void loadPoints(Particles &pars, const string fileName) {
  // read point information from file
  ifstream ifs(fileName);

  while (ifs.peek() != EOF) {
    float x, y, z;

    ifs >> x;
    ifs >> y;
    ifs >> z;

    // ignore '\n'
    // otherwise, the last empty line will be read
    ifs.ignore(1);

    Point p;
    p.pos = (vec3(x, y, z));
    // p.color = vec3(randf(), randf(), randf());
    p.color = vec3(0.5, 0.5, 0.5);
    p.v = vec3(0, 0, 0);
    p.m = randf();

    pars.Ps.push_back(p);
  }

  ifs.close();
}

void computeMatricesFromInputs() {
  // glfwGetTime is called only once, the first time this function is called
  static float lastTime = glfwGetTime();

  // Compute time difference between current and last frame
  float currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

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

  // update common matrix
  commonP =
      perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.f);
  commonV = lookAt(eyePoint, eyePoint + direction, newUp);

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}

void keyCallback(GLFWwindow *keyWnd, int key, int scancode, int action,
                 int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_ESCAPE: {
      glfwSetWindowShouldClose(keyWnd, GLFW_TRUE);
      break;
    }
    case GLFW_KEY_F: {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
    }
    case GLFW_KEY_L: {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    }
    case GLFW_KEY_I: {
      std::cout << "eyePoint: " << to_string(eyePoint) << '\n';
      std::cout << "verticleAngle: " << fmod(verticalAngle, 6.28f) << ", "
                << "horizontalAngle: " << fmod(horizontalAngle, 6.28f) << endl;
      break;
    }
    default:
      break;
    }
  }
}

void initUniform() {
  /* for particles */
  glUseProgram(shaderPar);

  uniParM = glGetUniformLocation(shaderPar, "M");
  uniParV = glGetUniformLocation(shaderPar, "V");
  uniParP = glGetUniformLocation(shaderPar, "P");

  glUniformMatrix4fv(uniParM, 1, GL_FALSE, value_ptr(commonM));
  glUniformMatrix4fv(uniParV, 1, GL_FALSE, value_ptr(commonV));
  glUniformMatrix4fv(uniParP, 1, GL_FALSE, value_ptr(commonP));

  /* for mesh */
  glUseProgram(shaderSphere);

  uniMeshM = glGetUniformLocation(shaderSphere, "M");
  uniMeshV = glGetUniformLocation(shaderSphere, "V");
  uniMeshP = glGetUniformLocation(shaderSphere, "P");

  glUniformMatrix4fv(uniMeshM, 1, GL_FALSE, value_ptr(commonM));
  glUniformMatrix4fv(uniMeshV, 1, GL_FALSE, value_ptr(commonV));
  glUniformMatrix4fv(uniMeshP, 1, GL_FALSE, value_ptr(commonP));
}
