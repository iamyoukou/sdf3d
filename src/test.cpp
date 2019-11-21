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

// Projecting a point to a plane
// Point is defined by P
// Plane is define by A, B, C
// Return the projected point P'
vec3 point2plane(vec3 a, vec3 b, vec3 c, vec3 p) {
  vec3 ab, ac, ap;
  ab = b - a;
  ac = c - a;
  ap = p - a;

  // surface normal
  vec3 n = normalize(cross(ab, ac));

  // PP'
  vec3 ppProj = -dot(ap, n) * n;

  // AP'
  vec3 apProj = ap + ppProj;

  // P'
  vec3 pProj = apProj + a;

  return pProj;
}

// Distance between a point and a triangle
// Triangle is defined by A, B, C in counter-clockwise
// (and maybe a surface normal N if provided)
// Point: P
float distPoint2Triangle(vec3 a, vec3 b, vec3 c, vec3 p) {
  float dist = 9999.f;

  // Project P onto a plane at P'
  vec3 Pproj = point2plane(a, b, c, p);
  // std::cout << "P' = " << glm::to_string(Pproj) << '\n';
  // std::cout << dot(n, P1 - a) << '\n';

  // barycentric coordinate of P'
  vec3 Pbary = baryCoord(a, b, c, Pproj);
  // std::cout << glm::to_string(Pbary) << '\n';

  // P' is inside ABC
  if ((Pbary.x > 0 && Pbary.x < 1.f) && (Pbary.y > 0 && Pbary.y < 1.f) &&
      (Pbary.z > 0 && Pbary.z < 1.f)) {
    std::cout << "P' is inside triangle" << '\n';
  }
  // When P' is outside ABC,
  // find the closest edge or vertex
  else {
    // std::cout << "P' is outside triangle" << '\n';

    // Calculate 6 line uv parameters
    vec2 uvAb, uvBc, uvCa;
    uvAb = lineUv(a, b, Pproj);
    uvBc = lineUv(b, c, Pproj);
    uvCa = lineUv(c, a, Pproj);

    // Calculate 3 barycentric  parameters
    vec3 uvwAbc = baryCoord(a, b, c, Pproj);

    // std::cout << "uvAb = " << glm::to_string(uvAb) << '\n';
    // std::cout << "uvBc = " << glm::to_string(uvBc) << '\n';
    // std::cout << "uvCa = " << glm::to_string(uvCa) << '\n';
    // std::cout << "uvwAbc = " << glm::to_string(uvwAbc) << '\n';

    // first: vertex regions
    if (uvAb[1] <= 0 && uvCa[0] <= 0) {
      std::cout << "region A" << '\n';
      dist = length(p - a);
    } else if (uvAb[0] <= 0 && uvBc[1] <= 0) {
      std::cout << "region B" << '\n';
      dist = length(p - b);
    } else if (uvBc[0] <= 0 && uvCa[1] <= 0) {
      std::cout << "region C" << '\n';
      dist = length(p - c);
    }
    // Second: edge regions
    else if (uvAb[0] > 0 && uvAb[1] > 0 && uvwAbc[2] <= 0) {
      std::cout << "region AB" << '\n';
      vec3 dirAb = normalize(b - a);
      vec3 APproj = Pproj - a;
      float frac = dot(APproj, dirAb);
      vec3 Pinter = a + dirAb * frac;
      dist = length(p - Pinter);
    } else if (uvBc[0] > 0 && uvBc[1] > 0 && uvwAbc[0] <= 0) {
      std::cout << "region BC" << '\n';
      vec3 dirBc = normalize(c - b);
      vec3 BPproj = Pproj - b;
      float frac = dot(BPproj, dirBc);
      vec3 Pinter = b + dirBc * frac;
      dist = length(p - Pinter);
    } else if (uvCa[0] > 0 && uvCa[1] > 0 && uvwAbc[1] <= 0) {
      std::cout << "region CA" << '\n';
      vec3 dirCa = normalize(a - c);
      vec3 CPproj = Pproj - c;
      float frac = dot(CPproj, dirCa);
      vec3 Pinter = c + dirCa * frac;
      dist = length(p - Pinter);
    }

    // std::cout << "dist = " << dist << '\n';
  } // end if P' is inside ABC

  return dist;
}

int main(int argc, char const *argv[]) {

  // vec3 A(1, -0.86, -0.5), B(1, 0.86, 0.5), C(-1, -0.86, -0.5), P(1, 2.5, 10);
  float dist = 9999.f;

  // test
  // vec3 P1(0, -1.615, 1.327);

  // std::cout << "n = " << glm::to_string(n) << '\n';

  std::cout << "dist = "
            << distPoint2Triangle(vec3(1, 2, 0), vec3(5, 3, 0), vec3(2, 4, 0),
                                  vec3(1, 2.5, 10))
            << '\n';

  return 0;
}
