#include "common.h"

std::string readFile(const std::string filename) {
  std::ifstream in;
  in.open(filename.c_str());
  std::stringstream ss;
  ss << in.rdbuf();
  std::string sOut = ss.str();
  in.close();

  return sOut;
}

Mesh loadObj(std::string filename) {
  Mesh outMesh;

  std::ifstream fin;
  fin.open(filename.c_str());

  if (!(fin.good())) {
    std::cout << "failed to open file : " << filename << std::endl;
  }

  while (fin.peek() != EOF) { // read obj loop
    std::string s;
    fin >> s;

    // vertex coordinate
    if ("v" == s) {
      float x, y, z, w;
      fin >> x;
      fin >> y;
      fin >> z;
      outMesh.vertices.push_back(glm::vec3(x, y, z));
    }
    // face normal (recorded as vn in obj file)
    else if ("vn" == s) {
      float x, y, z;
      fin >> x;
      fin >> y;
      fin >> z;
      outMesh.faceNormals.push_back(glm::vec3(x, y, z));
    }
    // vertices contained in face, and face normal
    else if ("f" == s) {
      //(v1, v2, v3, n) indices
      glm::ivec4 tempFace; // for v in "v//vn"

      fin >> tempFace[0]; // first "v//vn"
      fin.ignore(2);      // remove "//"
      fin >> tempFace[3]; // all vn in "v//vn" are same

      fin >> tempFace[1]; // second "v//vn"
      fin.ignore(2);
      fin >> tempFace[3];

      fin >> tempFace[2]; // third "v//vn"
      fin.ignore(2);
      fin >> tempFace[3];

      // Note:
      //  v and vn in "v//vn" start from 1,
      //  but indices of std::vector start from 0,
      //  so we need minus 1 for all elements
      tempFace -= glm::ivec4(1, 1, 1, 1);
      outMesh.faces.push_back(tempFace);
    } else {
      continue;
    }
  } // end read obj loop

  fin.close();

  return outMesh;
}

GLuint createShader(std::string filename, GLenum type) {
  /* read source code */
  std::string sTemp = readFile(filename);
  std::string info;
  const GLchar *source = sTemp.c_str();

  switch (type) {
  case GL_VERTEX_SHADER:
    info = "Vertex";
    break;
  case GL_FRAGMENT_SHADER:
    info = "Fragment";
    break;
  }

  if (source == NULL) {
    std::cout << info << " Shader : Can't read shader source file."
              << std::endl;
    return 0;
  }

  const GLchar *sources[] = {source};
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, sources, NULL);
  glCompileShader(shader);

  GLint compile_ok;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
  if (compile_ok == GL_FALSE) {
    std::cout << info << " Shader : Fail to compile." << std::endl;
    printLog(shader);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

void printLog(GLuint &object) {
  GLint log_length = 0;
  if (glIsShader(object)) {
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  } else if (glIsProgram(object)) {
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  } else {
    std::cout << "printlog: Not a shader or a program" << std::endl;
    return;
  }

  char *log = (char *)malloc(log_length);

  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);

  std::cout << log << '\n';

  free(log);
}

GLint myGetUniformLocation(GLuint &prog, std::string name) {
  GLint location;
  location = glGetUniformLocation(prog, name.c_str());
  if (location == -1) {
    std::cout << "Could not bind uniform : " << name << ". "
              << "Did you set the right name? "
              << "Or is " << name << " not used?" << std::endl;
  }

  return location;
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
    // case GLFW_KEY_I: {
    //   std::cout << "eyePoint: " << to_string(eyePoint) << '\n';
    //   std::cout << "verticleAngle: " << fmod(verticalAngle, 6.28f) << ", "
    //             << "horizontalAngle: " << fmod(horizontalAngle, 6.28f) <<
    //             endl;
    //   break;
    // }
    // case GLFW_KEY_Y: {
    //   saveTrigger = !saveTrigger;
    //   frameNumber = 0;
    //   break;
    // }
    default:
      break;
    }
  }
}

/* Mesh class */
void Mesh::translate(glm::vec3 xyz) {
  // move each vertex with xyz
  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i] += xyz;
  }

  // update aabb
}

void Mesh::scale(glm::vec3 xyz) {
  // scale each vertex with xyz
  // for (size_t i = 0; i < vertices.size(); i++) {
  //
  // }
}

void findAABB(Mesh &mesh) {
  int nOfVtxs = mesh.vertices.size();
  glm::vec3 min(0, 0, 0), max(0, 0, 0);

  for (size_t i = 0; i < nOfVtxs; i++) {
    // vec3 &vtx = mesh.vertexTable[i].vertexCoordinate;
    glm::vec3 vtx = mesh.vertices[i];

    // x
    if (vtx.x > max.x) {
      max.x = vtx.x;
    }
    if (vtx.x < min.x) {
      min.x = vtx.x;
    }
    // y
    if (vtx.y > max.y) {
      max.y = vtx.y;
    }
    if (vtx.y < min.y) {
      min.y = vtx.y;
    }
    // z
    if (vtx.z > max.z) {
      max.z = vtx.z;
    }
    if (vtx.z < min.z) {
      min.z = vtx.z;
    }
  }

  // mesh.minVertex = min;
  // mesh.maxVertex = max;
  // mesh.aabb.halfSize = 0.5f * (max - min);
  // mesh.aabb.center = min + mesh.aabb.halfSize;
  mesh.min = min;
  mesh.max = max;
}

void drawBox(glm::vec3 min, glm::vec3 max) {
  // first, write vertex coordinates into vector<vec3>
  // vector<vec3> vertices;
  // // vertex 0
  // vertices.push_back(
  //     vec3(boxCenter + vec3(-boxHalfSize.x, boxHalfSize.y, boxHalfSize.z)));
  // // 1
  // vertices.push_back(
  //     vec3(boxCenter + vec3(-boxHalfSize.x, -boxHalfSize.y, boxHalfSize.z)));
  // // 2
  // vertices.push_back(
  //     vec3(boxCenter + vec3(boxHalfSize.x, -boxHalfSize.y, boxHalfSize.z)));
  // // 3
  // vertices.push_back(
  //     vec3(boxCenter + vec3(boxHalfSize.x, boxHalfSize.y, boxHalfSize.z)));
  // // 4
  // vertices.push_back(
  //     vec3(boxCenter + vec3(-boxHalfSize.x, boxHalfSize.y, -boxHalfSize.z)));
  // // 5
  // vertices.push_back(
  //     vec3(boxCenter + vec3(-boxHalfSize.x, -boxHalfSize.y,
  //     -boxHalfSize.z)));
  // // 6
  // vertices.push_back(
  //     vec3(boxCenter + vec3(boxHalfSize.x, -boxHalfSize.y, -boxHalfSize.z)));
  // // 7
  // vertices.push_back(
  //     vec3(boxCenter + vec3(boxHalfSize.x, boxHalfSize.y, -boxHalfSize.z)));
  //
  // // then, write vertex coordinates from vector<vec3> to array
  // // 8 verices, 3 GLfloat per vertex
  // GLfloat *vertexArray = new GLfloat[8 * 3];
  // for (size_t i = 0; i < 8; i++) {
  //   vec3 &vtxCoord = vertices[i];
  //   vertexArray[3 * i] = vtxCoord.x;
  //   vertexArray[3 * i + 1] = vtxCoord.y;
  //   vertexArray[3 * i + 2] = vtxCoord.z;
  // }
  GLfloat aVtxs[]{
      min.x, max.y, min.z, // 0
      min.x, min.y, min.z, // 1
      max.x, min.y, min.z, // 2
      max.x, max.y, min.z, // 3
      min.x, max.y, max.z, // 4
      min.x, min.y, max.z, // 5
      max.x, min.y, max.z, // 6
      max.x, max.y, max.z  // 7
  };

  // vertex color
  // GLfloat colorArray[] = {color.x, color.y, color.z, color.x, color.y,
  // color.z,
  //                         color.x, color.y, color.z, color.x, color.y,
  //                         color.z, color.x, color.y, color.z, color.x,
  //                         color.y, color.z, color.x, color.y, color.z,
  //                         color.x, color.y, color.z};

  // vertex index
  GLushort aIdxs[] = {
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

  GLuint vboVtx;
  glGenBuffers(1, &vboVtx);
  glBindBuffer(GL_ARRAY_BUFFER, vboVtx);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * 3, aVtxs, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // GLuint vboColor;
  // glGenBuffers(1, &vboColor);
  // glBindBuffer(GL_ARRAY_BUFFER, vboColor);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray,
  // GL_STATIC_DRAW); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  // glEnableVertexAttribArray(1);

  GLuint ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aIdxs), aIdxs, GL_STATIC_DRAW);

  // draw box
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  for (size_t i = 0; i < 6; i++) {
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT,
                   (GLvoid *)(sizeof(GLushort) * 4 * i));
  }

  glDeleteBuffers(1, &vboVtx);
  // glDeleteBuffers(1, &vboColor);
  glDeleteBuffers(1, &ibo);
  glDeleteVertexArrays(1, &vao);
}
