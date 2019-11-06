#include "common.h"

string read_file(const string filename) {
  ifstream in;
  in.open(filename.c_str());
  stringstream ss;
  ss << in.rdbuf();
  string sOut = ss.str();
  in.close();

  return sOut;
}

mesh_info_t load_obj(string filename) {
  /*
      Read vertex information from filename,
      and temporarily store in the following variables.
  */
  vector<vec3> tempV;                         // store v
  vector<vec3> tempVn;                        // store vn
  vector<ivec4> tempFaceContainedVertexIndex; // store v of "v//vn" in f
  vector<int> tempFaceNormalIndex;            // store vn of "v//vn" in f
  int vertexNumber = 0, faceNormalNumber = 0, faceNumber = 0;

  ifstream fin;
  fin.open(filename.c_str());

  if (!(fin.good())) {
    cerr << "failed to open file : " << filename << endl;
  }

  int temp_vNum = 0;          // number of vertices
  while (fin.peek() != EOF) { // read obj loop
    string s;
    fin >> s;

    // vertex coordinate
    if ("v" == s) {
      float x, y, z, w;
      fin >> x;
      fin >> y;
      fin >> z;
      tempV.push_back(vec3(x, y, z));
      vertexNumber++;
    }
    // face normal (recorded as vn in obj file)
    else if ("vn" == s) {
      float x, y, z;
      fin >> x;
      fin >> y;
      fin >> z;
      tempVn.push_back(vec3(x, y, z));
      faceNormalNumber++;
    }
    // vertices contained in face, and face normal
    else if ("f" == s) {
      ivec4 v4i; // for v in "v//vn"
      int v1i;   // for vn in "v//vn"

      fin >> v4i[0]; // first "v//vn"
      fin.ignore(2); // remove "//"
      fin >> v1i;    // all vn in "v//vn" are same

      fin >> v4i[1]; // second "v//vn"
      fin.ignore(2);
      fin >> v1i;

      fin >> v4i[2]; // third "v//vn"
      fin.ignore(2);
      fin >> v1i;

      // in case we are dealing with triangle face
      if (fin.peek() != '\n') {
        fin >> v4i[3]; // fourth "v//vn"
        fin.ignore(2);
        fin >> v1i;
      } else {
        v4i[3] = -1; // means "no value"
      }

      // CAUTION:
      //  v and vn in "v//vn" start from 1,
      //  but indices of std::vector start from 0,
      //  so we need (v4i - 1), (v1i - 1)
      tempFaceContainedVertexIndex.push_back(v4i - 1);
      tempFaceNormalIndex.push_back(v1i - 1);

      faceNumber++;
    }
  } // end read obj loop

  fin.close();

  /* Computing mesh information from those temp*s */
  mesh_info_t meshInfo;

  for (size_t i = 0; i < vertexNumber; i++) {
    vertex_info_t vertexInfo;

    vertexInfo.vertexIndex = i;
    vertexInfo.vertexCoordinate = tempV[i];

    meshInfo.vertexTable.push_back(vertexInfo);
  }

  for (size_t i = 0; i < faceNumber; i++) {
    face_info_t faceInfo;

    faceInfo.faceIndex = i;
    faceInfo.containedVertexIndex = tempFaceContainedVertexIndex[i];
    faceInfo.faceNormal = tempVn[tempFaceNormalIndex[i]];

    meshInfo.faceTable.push_back(faceInfo);
  }

  /* compute "connected face index" */
  // check "contained vertex index" in face table,
  // write "face index" to "connected face index"
  for (size_t i = 0; i < faceNumber; i++) {
    // for convenience
    ivec4 &cvIdx = meshInfo.faceTable[i].containedVertexIndex;

    for (size_t j = 0; j < 4; j++) { // ivec4 has 4 elements
      // for convenience
      int vtxIdx = cvIdx[j];
      vector<int> &cfIdx = meshInfo.vertexTable[vtxIdx].connectedFaceIndex;

      if (-1 != vtxIdx) {   // in case we are dealing with a triangle face
        cfIdx.push_back(i); //"i" equals to face index
      }
    }
  }

  /* compute "vertex normal" */
  // check "connected face index" in vertex table,
  // extract "face normal" corresponding to "connected face index",
  // summerize them and compute their mean
  for (size_t i = 0; i < vertexNumber; i++) {
    vector<int> &cfIdx = meshInfo.vertexTable[i].connectedFaceIndex;
    vec3 vtxNormal(0.f);

    // summerize
    for (size_t j = 0; j < cfIdx.size(); j++) {
      int faceIdx = cfIdx[j];
      face_info_t faceInfo = meshInfo.faceTable[faceIdx];
      vtxNormal += faceInfo.faceNormal;
    }
    vtxNormal /= cfIdx.size(); // means

    // always normalize
    meshInfo.vertexTable[i].vertexNormal = normalize(vtxNormal);
  }

  return meshInfo;
}

GLuint create_shader(string filename, GLenum type) {
  /* read source code */
  string sTemp = read_file(filename);
  string info;
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
    cerr << "printlog: Not a shader or a program" << endl;
    return;
  }

  char *log = (char *)malloc(log_length);

  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);

  cerr << log << endl;

  free(log);
}

// convert quadrangle face to triangle face
// return triangle indices
vector<ivec3> quad2tri(mesh_info_t &mesh) {
  vector<ivec3> triangleIndices;
  int times = mesh.faceTable.size();

  for (size_t i = 0; i < times; i++) {
    ivec4 vtxidx = mesh.faceTable[i].containedVertexIndex;

    // triangle one
    triangleIndices.push_back(ivec3(vtxidx[0], vtxidx[1], vtxidx[2]));

    // if there is triangle two
    if (-1 != vtxidx[3]) {
      triangleIndices.push_back(ivec3(vtxidx[2], vtxidx[3], vtxidx[0]));
    }
  }

  return triangleIndices;
}

GLint myGetUniformLocation(GLuint &prog, string name) {
  GLint location;
  location = glGetUniformLocation(prog, name.c_str());
  if (location == -1) {
    cerr << "Could not bind uniform : " << name << ". "
         << "Did you set the right name? "
         << "Or is " << name << " not used?" << endl;
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
