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

float signedArea(glm::vec3 v1, glm::vec3 v2, glm::vec3 n) {
  float sa;

  glm::vec3 resCross = glm::cross(v1, v2);
  // std::cout << "cross = " << glm::to_string(resCross) << '\n';

  // in this case, area = 0
  if (glm::length(resCross) == 0) {
    sa = 0;
  } else {
    // if cross(v1, v2) is in the same direction with n
    // then let sign be +1, otherwise, -1
    // note: if resCross = (0, 0, 0),
    // then normalize(resCross) results in nan
    float sign = glm::dot(n, glm::normalize(resCross));
    // float sign = (resDot > 0) ? 1.f : -1.f;
    // std::cout << "resDot = " << resDot << '\n';

    sa = glm::length(resCross) * sign;
    // std::cout << "cross length = " << glm::length(resCross) << '\n';
    // std::cout << "area sign = " << sign << '\n';
  }

  return sa;
}

// Barycentric coordinates
// Given a triangle ABC, counter-clockwise
// a point P
// Let P = uA + vB + wC
// Return (u, v, w)
// Note that the order of vectors in cross product is important,
// so be careful when calculate the area.
// N: surface normal of ABC
// N is used to determine the sign of the signed area
// i.e. check whether N is in the same direction
// with cross(AB, AP), cross(BC, BP), cross(CA, CP)
glm::vec3 baryCoord(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n,
                    glm::vec3 p) {
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
  float Sabc = signedArea(ab, ac, n);
  float Sabp = signedArea(ab, ap, n);
  float Sbcp = signedArea(bc, bp, n);
  float Scap = signedArea(ca, cp, n);

  // std::cout << "Sabc = " << Sabc << '\n';
  // std::cout << "Sabp = " << Sabp << '\n';
  // std::cout << "Sbcp = " << Sbcp << '\n';
  // std::cout << "Scap = " << Scap << '\n';

  float u, v, w;
  u = Sbcp / Sabc;
  v = Scap / Sabc;
  w = Sabp / Sabc;

  return glm::vec3(u, v, w);
}

// Projecting a point to a plane
// Point is defined by P
// Plane is define by A, B, C, and normal N
// Return the projected point P'
glm::vec3 point2plane(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n,
                      glm::vec3 p) {
  glm::vec3 ab, ac, ap;
  // ab = b - a;
  // ac = c - a;
  ap = p - a;

  // surface normal
  // glm::vec3 n = glm::normalize(glm::cross(ab, ac));

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
// and a surface normal N
// Point: P
float distPoint2Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n,
                         glm::vec3 p) {
  float dist = 9999.f;

  // (necessary?)
  // if dot(ap, n) < 0, then re-order A, B, C
  // glm::vec3 tempB = b;
  // b = c;
  // c = tempB;

  // Project P onto a plane at P'
  glm::vec3 Pproj = point2plane(a, b, c, n, p);
  // std::cout << "P projected = " << glm::to_string(Pproj) << '\n';

  // barycentric coordinate of P'
  glm::vec3 Pbary = baryCoord(a, b, c, n, Pproj);
  // std::cout << "P' bary = " << glm::to_string(Pbary) << '\n';

  // P' is inside ABC
  // including: P' is on the edge or vertex
  if ((Pbary.x >= 0 && Pbary.x <= 1.f) && (Pbary.y >= 0 && Pbary.y <= 1.f) &&
      (Pbary.z >= 0 && Pbary.z <= 1.f)) {
    // std::cout << "P' is inside triangle" << '\n';
    glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));

    // For convenience, I multiply the sign later in the return statement
    dist = glm::abs(glm::dot(n, p - a));
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
    glm::vec3 uvwAbc = baryCoord(a, b, c, n, Pproj);

    // std::cout << "uvAb = " << glm::to_string(uvAb) << '\n';
    // std::cout << "uvBc = " << glm::to_string(uvBc) << '\n';
    // std::cout << "uvCa = " << glm::to_string(uvCa) << '\n';
    // std::cout << "uvwAbc = " << glm::to_string(uvwAbc) << '\n';

    // first: vertex regions
    if (uvAb[1] <= 0 && uvCa[0] <= 0) {
      // std::cout << "region A" << '\n';
      dist = glm::length(p - a);
    } else if (uvAb[0] <= 0 && uvBc[1] <= 0) {
      // std::cout << "region B" << '\n';
      dist = glm::length(p - b);
    } else if (uvBc[0] <= 0 && uvCa[1] <= 0) {
      // std::cout << "region C" << '\n';
      dist = glm::length(p - c);
    }
    // Second: edge regions
    else if (uvAb[0] > 0 && uvAb[1] > 0 && uvwAbc[2] <= 0) {
      // std::cout << "region AB" << '\n';
      glm::vec3 dirAb = glm::normalize(b - a);
      glm::vec3 APproj = Pproj - a;
      float frac = glm::dot(APproj, dirAb);
      glm::vec3 Pinter = a + dirAb * frac;
      // std::cout << "Pinter = " << glm::to_string(Pinter) << '\n';
      dist = glm::length(p - Pinter);
    } else if (uvBc[0] > 0 && uvBc[1] > 0 && uvwAbc[0] <= 0) {
      // std::cout << "region BC" << '\n';
      glm::vec3 dirBc = glm::normalize(c - b);
      glm::vec3 BPproj = Pproj - b;
      float frac = glm::dot(BPproj, dirBc);
      glm::vec3 Pinter = b + dirBc * frac;
      // std::cout << "Pinter = " << glm::to_string(Pinter) << '\n';
      dist = glm::length(p - Pinter);
    } else if (uvCa[0] > 0 && uvCa[1] > 0 && uvwAbc[1] <= 0) {
      // std::cout << "region CA" << '\n';
      glm::vec3 dirCa = glm::normalize(a - c);
      glm::vec3 CPproj = Pproj - c;
      float frac = glm::dot(CPproj, dirCa);
      glm::vec3 Pinter = c + dirCa * frac;
      // std::cout << "Pinter = " << glm::to_string(Pinter) << '\n';
      dist = glm::length(p - Pinter);
    }
  } // end if P' is outside ABC

  // std::cout << "dist = " << dist << '\n';

  // sign
  // glm::vec3 ap = p - a;
  float temp = glm::dot(p - a, n);
  // if P is on the same plane of triangle
  // if (temp == 0) {
  //   sign = 1.f;
  // } else {
  //   // std::cout << "dot(ap, n) = " << sign << '\n';
  //   sign = temp;
  //   sign /= glm::abs(sign);
  // }

  // general case
  float sign = (temp == 0) ? 1.f : (temp / glm::abs(temp));

  // special case
  float epsilon = 0.01f;
  if (abs(temp) < epsilon) {
    sign = 1.f;
  }

  // std::cout << "sign = " << sign << '\n';

  return dist * sign;
}

// using world space position to calculate node hash
// pay attention to the order of x, y, z
// otherwise, a index error happens
int calCellHash(vec3 pos, ivec3 nOfCells, float cellSize) {
  // std::cout << to_string(nOfCells) << '\n';

  int hx = int(floor(pos.x / cellSize));
  int hy = int(floor(pos.y / cellSize) * nOfCells.x);
  int hz = int(floor(pos.z / cellSize) * nOfCells.x * nOfCells.y);

  return (hx + hy + hz);
}

//
