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
