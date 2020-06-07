#include <common.h>
#include <sdf.h>

GLFWwindow *window;

ivec3 nOfCells;
float cellSize = 0.1f;
vec3 gridOrigin(0, 0, 0);
vec3 rangeOffset(0.2f, 0.2f, 0.2f);
Grid grid;
Mesh mesh;

void initGL();
void initOther();
void initGrid();
void initMesh();

void writeSdf(Grid &, const string);
vec3 calCellPos(vec3);
float randf();

int main(int argc, char const *argv[]) {
  initGL();
  initOther();
  initMesh();
  initGrid();

  /* find a searching range */
  // select an area a little bigger than mesh's aabb
  vec3 rangeMin = mesh.min - rangeOffset;
  vec3 rangeMax = mesh.max + rangeOffset;

  // find cells which cover those area
  vec3 startCell = calCellPos(rangeMin);
  vec3 endCell = calCellPos(rangeMax);

  // for the selected range
  for (float z = startCell.z; z < endCell.z; z += cellSize) {
    for (float y = startCell.y; y < endCell.y; y += cellSize) {
      for (float x = startCell.x; x < endCell.x; x += cellSize) {
        vec3 P(x, y, z); // cell position
        float dist = 9999.f;

        // iterate triangles in the mesh
        for (size_t i = 0; i < mesh.faces.size(); i++) {
          Face face = mesh.faces[i];

          glm::vec3 A, B, C, N;
          A = mesh.vertices[face.v1];
          B = mesh.vertices[face.v2];
          C = mesh.vertices[face.v3];
          N = mesh.faceNormals[face.vn1];

          float temp = distPoint2Triangle(A, B, C, N, P);
          float oldDist = dist;

          // for general case
          dist = (glm::abs(temp) < glm::abs(dist)) ? temp : dist;

          // for a special case
          float delta = abs(abs(temp) - abs(oldDist));
          // if delta is less than some threshold
          // we decide that temp is equal to dist
          if (delta < 0.0001f) {

            // if dist will change its sign
            // we keep dist at the positive one
            dist = (temp > 0) ? temp : oldDist;
          }
        } // end iterate triangles

        // write dist into grid
        int hash = calCellHash(P, nOfCells, cellSize);
        grid.cells[hash].sd = dist;
      } // end x direction
    }   // end y direction
  }     // end z direction

  writeSdf(grid, "sdf.txt");

  return 0;
}

// calculate the position of the cell which covers the specified point
vec3 calCellPos(vec3 pt) {
  // change reference frame
  vec3 ptRef = pt - gridOrigin;

  // grid index along each axis of this cell
  int ix = int(floor(ptRef.x / cellSize));
  int iy = int(floor(ptRef.y / cellSize));
  int iz = int(floor(ptRef.z / cellSize));

  // position of this cell
  vec3 posRef = vec3(ix * cellSize, iy * cellSize, iz * cellSize);

  // change reference frame
  vec3 pos = posRef + gridOrigin;

  return pos;
}

void initGrid() {
  /* grid parameters */
  // The grid covers the area of mesh
  // Between the grid and the mesh,
  // there is a offset area which is defined by rangeOffset
  vec3 gridSize = (mesh.max + rangeOffset) - gridOrigin;
  nOfCells = ivec3(gridSize / cellSize);

  grid.origin = gridOrigin;
  grid.cellSize = cellSize;
  grid.nOfCells = nOfCells;

  // ATTENTION: the order of iterating x, y, z relates to
  // the hash of cell index
  for (size_t iz = 0; iz < nOfCells.z; iz++) {
    for (size_t iy = 0; iy < nOfCells.y; iy++) {
      for (size_t ix = 0; ix < nOfCells.x; ix++) {
        Cell cell;

        cell.idx = ivec3(ix, iy, iz);
        cell.sd = 9999.f;

        cell.pos = vec3(ix, iy, iz) * cellSize + gridOrigin;

        grid.cells.push_back(cell);
      } // end of iterate x
    }   // end of iterate y
  }     // end of iterate z
}

// format: x, y, z, i, j, k, dist
void writeSdf(Grid &gd, const string fileName) {
  ofstream output(fileName);

  for (size_t i = 0; i < gd.cells.size(); i++) {
    Cell &cell = gd.cells[i];

    output << cell.pos.x;
    output << " ";
    output << cell.pos.y;
    output << " ";
    output << cell.pos.z;
    output << " ";
    output << cell.idx.x;
    output << " ";
    output << cell.idx.y;
    output << " ";
    output << cell.idx.z;
    output << " ";
    output << cell.sd;
    output << '\n';
  }

  output.close();
}

void initMesh() {
  /* prepare mesh data */
  mesh = loadObj("./mesh/sphere.obj");
  // Mesh mesh = loadObj("./mesh/monkey.obj");
  // Mesh mesh = loadObj("./mesh/torus.obj");
  // Mesh mesh = loadObj("./mesh/bunny.obj");
  // Mesh mesh = loadObj("./mesh/cube.obj");
  findAABB(mesh);

  // transform mesh to (origin + offset) position
  vec3 offset = (gridOrigin - mesh.min) + rangeOffset;
  mesh.translate(offset);
}

void initOther() { srand(clock()); }

float randf() {
  // random number in [0, 1]
  float f = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

  return f;
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
  // glfwSetKeyCallback(window, keyCallback);
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
