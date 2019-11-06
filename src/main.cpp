// Input: min vertex, max vertex (i.e. bounding box)
// Output: draw a box (can be used to visualize aabb)

#include "common.h"

GLint uniform_mvp;
GLFWwindow *window;
GLuint vtxShader, fragShader;
GLuint programObj;

float verticalAngle = -2.3f;
float horizontalAngle = 5.4f;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;
float farPlane = 2000.f;
float dudv_move = 0.f;

vec3 eyePoint = vec3(5.7f, 9.7f, -5.9f);
vec3 eyeDirection =
    vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
         sin(verticalAngle) * sin(horizontalAngle));
vec3 up = vec3(0.f, 1.f, 0.f);

mat4 matOriModel, matModel, matView, matProject;

const string projectDir = "/Users/YJ-work/sdf3d/shader/";

// must be the same value as the location parameter in vertex shader
GLuint boxVaoVtxIdx = 8, boxVaoClrIdx = 9;

void initGL() {
  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
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
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                            "GLFW window with AntTweakBar", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to open GLFW window." << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetKeyCallback(window, keyCallback);

  /* Initialize GLEW */
  // without this, glGenVertexArrays will report ERROR!
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
  }
}

void buildShader() {
  vtxShader =
      create_shader(projectDir + "vertex_shader.glsl", GL_VERTEX_SHADER);
  fragShader =
      create_shader(projectDir + "fragment_shader.glsl", GL_FRAGMENT_SHADER);

  programObj = glCreateProgram();
  glAttachShader(programObj, vtxShader);
  glAttachShader(programObj, fragShader);

  glLinkProgram(programObj);
  GLint link_ok;
  glGetProgramiv(programObj, GL_LINK_STATUS, &link_ok);

  if (link_ok == GL_FALSE) {
    std::cout << "Fail to link shader program." << std::endl;
    printLog(programObj);
    glDeleteProgram(programObj);
  }

  glUseProgram(programObj);
}

void recomputeMatrices() {
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
  // As the cursor is put at the center of the screen,
  // (WINDOW_WIDTH/2.f - xpos) and (WINDOW_HEIGHT/2.f - ypos) are offsets
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

  matView = lookAt(eyePoint, eyePoint + direction, newUp);
  matProject = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f,
                           farPlane);

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;

  mat4 mvp = matProject * matView * matModel;
  glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, value_ptr(mvp));
}

void initMatrices() {
  uniform_mvp = glGetUniformLocation(programObj, "mvp");

  matModel = translate(mat4(1.f), vec3(0.f, 0.f, -4.f));
  matView = lookAt(vec3(0.f, 2.f, 0.f),  // eye position
                   vec3(0.f, 0.f, -4.f), // look at
                   vec3(0.f, 1.f, 0.f)   // up
  );
  matProject =
      perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 10.f);

  mat4 mvp = matProject * matView * matModel;

  glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, value_ptr(mvp));
}

void drawBox(vec3 lb) {
  float size = 1.0f;

  // first, write vertex coordinates into vector<vec3>
  vector<vec3> vertices;
  vertices.push_back(lb);                                // 0
  vertices.push_back(vec3(lb + vec3(size, 0.f, 0.f)));   // 1
  vertices.push_back(vec3(lb + vec3(size, 0.f, size)));  // 2
  vertices.push_back(vec3(lb + vec3(0.f, 0.f, size)));   // 3
  vertices.push_back(vec3(lb + vec3(size, size, 0.f)));  // 4
  vertices.push_back(vec3(lb + vec3(size, size, size))); // 5
  vertices.push_back(vec3(lb + vec3(0.f, size, size)));  // 6
  vertices.push_back(vec3(lb + vec3(0.f, size, 0.f)));   // 7

  // then, write vertex coordinates from vector<vec3> to array
  // 8 verices, 3 GLfloat per vertex
  GLfloat *vertexArray = new GLfloat[8 * 3];
  for (size_t i = 0; i < 8; i++) {
    vec3 &vtxCoord = vertices[i];
    vertexArray[3 * i] = vtxCoord.x;
    vertexArray[3 * i + 1] = vtxCoord.y;
    vertexArray[3 * i + 2] = vtxCoord.z;
  }

  // vertex color
  GLfloat colorArray[] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
                          1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
                          1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};

  // vertex index
  GLushort indexArray[] = {
      0, 1, 4, 7, // front
      2, 3, 6, 5, // back
      4, 5, 6, 7, // up
      0, 3, 2, 1, // down
      7, 6, 3, 0, // left
      1, 2, 5, 4  // right
  };

  // prepare buffers to draw
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vboVertex;
  glGenBuffers(1, &vboVertex);
  glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * 3, vertexArray,
               GL_STATIC_DRAW);
  glVertexAttribPointer(boxVaoVtxIdx, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(boxVaoVtxIdx);

  GLuint vboColor;
  glGenBuffers(1, &vboColor);
  glBindBuffer(GL_ARRAY_BUFFER, vboColor);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray, GL_STATIC_DRAW);
  glVertexAttribPointer(boxVaoClrIdx, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(boxVaoClrIdx);

  GLuint ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray,
               GL_STATIC_DRAW);

  // draw box
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  for (size_t i = 0; i < 6; i++) {
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT,
                   (GLvoid *)(sizeof(GLushort) * 4 * i));
  }

  delete[] vertexArray;
}

int main(int argc, char **argv) {
  initGL();
  buildShader();
  initMatrices();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* clean the buffer */
    glClearColor(175.f / 255.f, 184.f / 255.f, 228.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    recomputeMatrices();

    for (size_t x = 0; x < 4; x++) {
      for (size_t y = 0; y < 4; y++) {
        for (size_t z = 0; z < 4; z++) {
          vec3 pos = vec3(float(x), float(y), float(z));
          drawBox(pos);
        }
      }
    }

    // swap, event
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}
