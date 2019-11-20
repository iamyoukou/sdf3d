#include "common.h"

// Given A, B, Q
// Project Q on AB at P
// Let P = vA + uB
// Return (u, v)
vec2 lineUv(vec3 a, vec3 b, vec3 q) {
  vec3 ab = b - a;
  vec3 aq = q - a;
  vec3 dirAb = normalize(ab);

  float v = dot(aq, dirAb) / length(ab);
  float u = 1.f - v;

  return vec2(u, v);
}

float signedArea(vec3 v1, vec3 v2) {
  vec3 temp = cross(v1, v2);
  // std::cout << glm::to_string(temp) << '\n';
  float sign = temp.z / abs(temp.z);
  float sa = length(temp) * sign;

  return sa;
}

// Barycentric coordinates
// Given a triangle ABC, counter-clockwise
// a point P
// Let P = uA + vB + wC
// Return (u, v, w)
// Note that the order of vectors in cross product is important,
// so be careful when calculate the area.
vec3 baryCoord(vec3 a, vec3 b, vec3 c, vec3 p) {
  vec3 ab = b - a;
  vec3 bc = c - b;
  vec3 ca = a - c;
  vec3 ac = c - a;

  vec3 ap = p - a;
  vec3 bp = p - b;
  vec3 cp = p - c;

  // Note: theoretically, 0.5 must be multiplied
  // to get triangle area.
  // However, 0.5 will be cancelled in the ratio.
  // So 0.5 can be avoided here.
  float Sabc = signedArea(ab, ac);
  float Sabp = signedArea(ab, ap);
  float Sbcp = signedArea(bc, bp);
  float Sacp = signedArea(ca, cp);

  // std::cout << "Sabc = " << Sabc << '\n';
  // std::cout << "Sabp = " << Sabp << '\n';
  // std::cout << "Sbcp = " << Sbcp << '\n';
  // std::cout << "Sacp = " << Sacp << '\n';

  float u, v, w;
  u = Sbcp / Sabc;
  v = Sacp / Sabc;
  w = Sabp / Sabc;

  return vec3(u, v, w);
}

int main(int argc, char const *argv[]) {
  vec3 A(1, 2, 0), B(5, 3, 0), C(2, 4, 0), P(2.06, 1.74, 0);

  // barycentric coordinate of P
  vec3 Pbary = baryCoord(A, B, C, P);
  // std::cout << glm::to_string(Pbary) << '\n';

  // P is inside ABC
  if ((Pbary.x > 0 && Pbary.x < 1.f) && (Pbary.y > 0 && Pbary.y < 1.f) &&
      (Pbary.z > 0 && Pbary.z < 1.f)) {
    std::cout << "P is inside triangle" << '\n';
  }
  // When P is outside ABC,
  // find the closest edge or vertex
  else {
    // std::cout << "P is outside triangle" << '\n';

    // Calculate 6 line uv parameters
    vec2 uvAb, uvBc, uvCa;
    uvAb = lineUv(A, B, P);
    uvBc = lineUv(B, C, P);
    uvCa = lineUv(C, A, P);

    // Calculate 3 barycentric  parameters
    vec3 uvwAbc = baryCoord(A, B, C, P);

    // std::cout << "uvAb = " << glm::to_string(uvAb) << '\n';
    // std::cout << "uvBc = " << glm::to_string(uvBc) << '\n';
    // std::cout << "uvCa = " << glm::to_string(uvCa) << '\n';
    // std::cout << "uvwAbc = " << glm::to_string(uvwAbc) << '\n';

    // first: vertex regions
    if (uvAb[1] <= 0 && uvCa[0] <= 0) {
      std::cout << "region A" << '\n';
    } else if (uvAb[0] <= 0 && uvBc[1] <= 0) {
      std::cout << "region B" << '\n';
    } else if (uvBc[0] <= 0 && uvCa[1] <= 0) {
      std::cout << "region C" << '\n';
    }
    // Second: edge regions
    else if (uvAb[0] > 0 && uvAb[1] > 0 && uvwAbc[2] <= 0) {
      std::cout << "region AB" << '\n';
    } else if (uvBc[0] > 0 && uvBc[1] > 0 && uvwAbc[0] <= 0) {
      std::cout << "region BC" << '\n';
    } else if (uvCa[0] > 0 && uvCa[1] > 0 && uvwAbc[1] <= 0) {
      std::cout << "region CA" << '\n';
    }
  }

  // std::cout << glm::to_string(cross(B - A, C - A)) << '\n';
  // std::cout << glm::to_string(cross(B - A, P - A)) << '\n';
  // std::cout << glm::to_string(cross(C - B, P - B)) << '\n';
  // std::cout << glm::to_string(cross(A - C, P - C)) << '\n';

  return 0;
}
