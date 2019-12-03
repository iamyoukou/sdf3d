#include "sdf.h"

// Given A, B, Q
// Project Q on AB at P
// Let P = vA + uB
// Return (u, v)
glm::vec2 lineUv(glm::vec3 a, glm::vec3 b, glm::vec3 q) {
  glm::vec3 ab = b - a;
  glm::vec3 aq = q - a;
  glm::vec3 dirAb = glm::normalize(ab);

  float v = glm::dot(aq, dirAb) / glm::length(ab);
  float u = 1.f - v;

  return glm::vec2(u, v);
}

float signedArea(glm::vec3 v1, glm::vec3 v2) {
  glm::vec3 temp = glm::cross(v1, v2);
  // std::cout << glm::to_string(temp) << '\n';
  float sign = temp.z / glm::abs(temp.z);
  float sa = glm::length(temp) * sign;

  return sa;
}

// Barycentric coordinates
// Given a triangle ABC, counter-clockwise
// a point P
// Let P = uA + vB + wC
// Return (u, v, w)
// Note that the order of vectors in cross product is important,
// so be careful when calculate the area.
glm::vec3 baryCoord(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 p) {
  glm::vec3 ab = b - a;
  glm::vec3 bc = c - b;
  glm::vec3 ca = a - c;
  glm::vec3 ac = c - a;

  glm::vec3 ap = p - a;
  glm::vec3 bp = p - b;
  glm::vec3 cp = p - c;

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

  return glm::vec3(u, v, w);
}

// Projecting a point to a plane
// Point is defined by P
// Plane is define by A, B, C
// Return the projected point P'
glm::vec3 point2plane(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 p) {
  glm::vec3 ab, ac, ap;
  ab = b - a;
  ac = c - a;
  ap = p - a;

  // surface normal
  glm::vec3 n = glm::normalize(glm::cross(ab, ac));

  // PP'
  glm::vec3 ppProj = -glm::dot(ap, n) * n;

  // AP'
  glm::vec3 apProj = ap + ppProj;

  // P'
  glm::vec3 pProj = apProj + a;

  // std::cout << "projected point: " << glm::to_string(pProj) << '\n';

  return pProj;
}

// Distance between a point and a triangle
// Triangle is defined by A, B, C in counter-clockwise
// (and maybe a surface normal N if provided)
// Point: P
float distPoint2Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 p) {
  float dist = 9999.f;

  // Project P onto a plane at P'
  glm::vec3 Pproj = point2plane(a, b, c, p);
  // std::cout << "P projected = " << glm::to_string(Pproj) << '\n';
  // std::cout << glm::dot(n, P1 - a) << '\n';

  // barycentric coordinate of P'
  glm::vec3 Pbary = baryCoord(a, b, c, Pproj);
  // std::cout << glm::to_string(Pbary) << '\n';

  // P' is inside ABC
  if ((Pbary.x > 0 && Pbary.x < 1.f) && (Pbary.y > 0 && Pbary.y < 1.f) &&
      (Pbary.z > 0 && Pbary.z < 1.f)) {
    std::cout << "P' is inside triangle" << '\n';
    glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));
    dist = glm::dot(n, p - a);
  }
  // When P' is outside ABC,
  // find the closest edge or vertex
  else {
    // std::cout << "P' is outside triangle" << '\n';

    // Calculate 6 line uv parameters
    glm::vec2 uvAb, uvBc, uvCa;
    uvAb = lineUv(a, b, Pproj);
    uvBc = lineUv(b, c, Pproj);
    uvCa = lineUv(c, a, Pproj);

    // Calculate 3 barycentric  parameters
    glm::vec3 uvwAbc = baryCoord(a, b, c, Pproj);

    // std::cout << "uvAb = " << glm::to_string(uvAb) << '\n';
    // std::cout << "uvBc = " << glm::to_string(uvBc) << '\n';
    // std::cout << "uvCa = " << glm::to_string(uvCa) << '\n';
    // std::cout << "uvwAbc = " << glm::to_string(uvwAbc) << '\n';

    // first: vertex regions
    if (uvAb[1] <= 0 && uvCa[0] <= 0) {
      std::cout << "region A" << '\n';
      dist = glm::length(p - a);
    } else if (uvAb[0] <= 0 && uvBc[1] <= 0) {
      std::cout << "region B" << '\n';
      dist = glm::length(p - b);
    } else if (uvBc[0] <= 0 && uvCa[1] <= 0) {
      std::cout << "region C" << '\n';
      dist = glm::length(p - c);
    }
    // Second: edge regions
    else if (uvAb[0] > 0 && uvAb[1] > 0 && uvwAbc[2] <= 0) {
      std::cout << "region AB" << '\n';
      glm::vec3 dirAb = glm::normalize(b - a);
      glm::vec3 APproj = Pproj - a;
      float frac = glm::dot(APproj, dirAb);
      glm::vec3 Pinter = a + dirAb * frac;
      // std::cout << "Pinter = " << glm::to_string(Pinter) << '\n';
      dist = glm::length(p - Pinter);
    } else if (uvBc[0] > 0 && uvBc[1] > 0 && uvwAbc[0] <= 0) {
      std::cout << "region BC" << '\n';
      glm::vec3 dirBc = glm::normalize(c - b);
      glm::vec3 BPproj = Pproj - b;
      float frac = glm::dot(BPproj, dirBc);
      glm::vec3 Pinter = b + dirBc * frac;
      // std::cout << "Pinter = " << glm::to_string(Pinter) << '\n';
      dist = glm::length(p - Pinter);
    } else if (uvCa[0] > 0 && uvCa[1] > 0 && uvwAbc[1] <= 0) {
      std::cout << "region CA" << '\n';
      glm::vec3 dirCa = glm::normalize(a - c);
      glm::vec3 CPproj = Pproj - c;
      float frac = glm::dot(CPproj, dirCa);
      glm::vec3 Pinter = c + dirCa * frac;
      // std::cout << "Pinter = " << glm::to_string(Pinter) << '\n';
      dist = glm::length(p - Pinter);
    }

    // std::cout << "dist = " << dist << '\n';
  } // end if P' is inside ABC

  return dist;
}
