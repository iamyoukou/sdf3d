// Input: min vertex, max vertex (i.e. bounding box)
// Output: draw a box (can be used to visualize aabb)

#include "common.h"

GLint uniform_mvp;
GLFWwindow *window;
GLuint vtxShader, fragShader;
GLuint programObj;

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
  vtxShader = create_shader("vertex_shader.glsl", GL_VERTEX_SHADER);
  fragShader = create_shader("fragment_shader.glsl", GL_FRAGMENT_SHADER);

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

void createMatrices() {
  uniform_mvp = glGetUniformLocation(programObj, "mvp");
  mat4 M = translate(mat4(1.f), vec3(0.f, 0.f, -4.f));
  mat4 V = lookAt(vec3(0.f, 2.f, 0.f),  // eye position
                  vec3(0.f, 0.f, -4.f), // look at
                  vec3(0.f, 1.f, 0.f)   // up
  );
  mat4 P = perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 10.f);
  mat4 mvp = P * V * M;
  glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, value_ptr(mvp));
}

void drawBox(vec3 min, vec3 max) {
  float lengthX, lengthY, lengthZ;
  lengthX = max.x - min.x;
  lengthY = max.y - min.y;
  lengthZ = max.z - min.z;

  // first, write vertex coordinates into vector<vec3>
  vector<vec3> vertices;
  vertices.push_back(vec3(max - vec3(lengthX, 0.f, 0.f)));     // vertex 0
  vertices.push_back(vec3(min + vec3(0.f, 0.f, lengthZ)));     // 1
  vertices.push_back(vec3(min + vec3(lengthX, 0.f, lengthZ))); // 2
  vertices.push_back(vec3(max));                               // 3
  vertices.push_back(vec3(max - vec3(lengthX, 0.f, lengthZ))); // 4
  vertices.push_back(vec3(min));                               // 5
  vertices.push_back(vec3(min + vec3(lengthX, 0.f, 0.f)));     // 6
  vertices.push_back(vec3(max - vec3(0.f, 0.f, lengthZ)));     // 7

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
      0, 1, 2, 3, // front face
      4, 7, 6, 5, // back
      4, 0, 3, 7, // up
      5, 6, 2, 1, // down
      0, 4, 5, 1, // left
      3, 2, 6, 7  // right
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
  createMatrices();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* clean the buffer */
    glClearColor(175.f / 255.f, 184.f / 255.f, 228.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawBox(vec3(-1.f, -1.f, -1.f), // min
            vec3(1.5f, 1.f, 0.5f)   // max
    );

    // swap, event
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}
