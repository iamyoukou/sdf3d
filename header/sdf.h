#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

using namespace std;
using namespace glm;

typedef struct {
  ivec3 idx; // index (i, j, k) in grid space
  vec3 pos;  // position in world space
  float sd;  // signed distance
} Node, Cell;

class Grid {
public:
  /* Members */
  // use hash to access each cell
  vector<Cell> cells;

  /* Constructors */
  Grid() {}
  ~Grid() {}
};

vec2 lineUv(vec3, vec3, vec3);
float signedArea(vec3, vec3, vec3);
vec3 baryCoord(vec3, vec3, vec3, vec3, vec3);
vec3 point2plane(vec3, vec3, vec3, vec3, vec3);
float distPoint2Triangle(vec3, vec3, vec3, vec3, vec3);
int calCellHash(vec3, ivec3, float);
