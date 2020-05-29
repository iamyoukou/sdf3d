#include "sdf.h"

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

float signedArea(vec3 v1, vec3 v2, vec3 n) {
  float sa;

  vec3 resCross = cross(v1, v2);
  // std::cout << "cross = " << to_string(resCross) << '\n';

  // in this case, area = 0
  if (length(resCross) == 0) {
    sa = 0;
  } else {
    // if cross(v1, v2) is in the same direction with n
    // then let sign be +1, otherwise, -1
    // note: if resCross = (0, 0, 0),
    // then normalize(resCross) results in nan
    float sign = dot(n, normalize(resCross));
    // float sign = (resDot > 0) ? 1.f : -1.f;
    // std::cout << "resDot = " << resDot << '\n';

    sa = length(resCross) * sign;
    // std::cout << "cross length = " << length(resCross) << '\n';
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
vec3 baryCoord(vec3 a, vec3 b, vec3 c, vec3 n, vec3 p) {
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

  return vec3(u, v, w);
}

// Projecting a point to a plane
// Point is defined by P
// Plane is define by A, B, C, and normal N
// Return the projected point P'
vec3 point2plane(vec3 a, vec3 b, vec3 c, vec3 n, vec3 p) {
  vec3 ab, ac, ap;
  // ab = b - a;
  // ac = c - a;
  ap = p - a;

  // surface normal
  // vec3 n = normalize(cross(ab, ac));

  // PP'
  vec3 ppProj = -dot(ap, n) * n;

  // AP'
  vec3 apProj = ap + ppProj;

  // P'
  vec3 pProj = apProj + a;

  // std::cout << "projected point: " << to_string(pProj) << '\n';

  return pProj;
}

// Distance between a point and a triangle
// Triangle is defined by A, B, C in counter-clockwise
// and a surface normal N
// Point: P
float distPoint2Triangle(vec3 a, vec3 b, vec3 c, vec3 n, vec3 p) {
  float dist = 9999.f;

  // (necessary?)
  // if dot(ap, n) < 0, then re-order A, B, C
  // vec3 tempB = b;
  // b = c;
  // c = tempB;

  // Project P onto a plane at P'
  vec3 Pproj = point2plane(a, b, c, n, p);
  // std::cout << "P projected = " << to_string(Pproj) << '\n';

  // barycentric coordinate of P'
  vec3 Pbary = baryCoord(a, b, c, n, Pproj);
  // std::cout << "P' bary = " << to_string(Pbary) << '\n';

  // P' is inside ABC
  // including: P' is on the edge or vertex
  if ((Pbary.x >= 0 && Pbary.x <= 1.f) && (Pbary.y >= 0 && Pbary.y <= 1.f) &&
      (Pbary.z >= 0 && Pbary.z <= 1.f)) {
    // std::cout << "P' is inside triangle" << '\n';
    vec3 n = normalize(cross(b - a, c - a));

    // For convenience, I multiply the sign later in the return statement
    dist = abs(dot(n, p - a));
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
    vec3 uvwAbc = baryCoord(a, b, c, n, Pproj);

    // std::cout << "uvAb = " << to_string(uvAb) << '\n';
    // std::cout << "uvBc = " << to_string(uvBc) << '\n';
    // std::cout << "uvCa = " << to_string(uvCa) << '\n';
    // std::cout << "uvwAbc = " << to_string(uvwAbc) << '\n';

    // first: vertex regions
    if (uvAb[1] <= 0 && uvCa[0] <= 0) {
      // std::cout << "region A" << '\n';
      dist = length(p - a);
    } else if (uvAb[0] <= 0 && uvBc[1] <= 0) {
      // std::cout << "region B" << '\n';
      dist = length(p - b);
    } else if (uvBc[0] <= 0 && uvCa[1] <= 0) {
      // std::cout << "region C" << '\n';
      dist = length(p - c);
    }
    // Second: edge regions
    else if (uvAb[0] > 0 && uvAb[1] > 0 && uvwAbc[2] <= 0) {
      // std::cout << "region AB" << '\n';
      vec3 dirAb = normalize(b - a);
      vec3 APproj = Pproj - a;
      float frac = dot(APproj, dirAb);
      vec3 Pinter = a + dirAb * frac;
      // std::cout << "Pinter = " << to_string(Pinter) << '\n';
      dist = length(p - Pinter);
    } else if (uvBc[0] > 0 && uvBc[1] > 0 && uvwAbc[0] <= 0) {
      // std::cout << "region BC" << '\n';
      vec3 dirBc = normalize(c - b);
      vec3 BPproj = Pproj - b;
      float frac = dot(BPproj, dirBc);
      vec3 Pinter = b + dirBc * frac;
      // std::cout << "Pinter = " << to_string(Pinter) << '\n';
      dist = length(p - Pinter);
    } else if (uvCa[0] > 0 && uvCa[1] > 0 && uvwAbc[1] <= 0) {
      // std::cout << "region CA" << '\n';
      vec3 dirCa = normalize(a - c);
      vec3 CPproj = Pproj - c;
      float frac = dot(CPproj, dirCa);
      vec3 Pinter = c + dirCa * frac;
      // std::cout << "Pinter = " << to_string(Pinter) << '\n';
      dist = length(p - Pinter);
    }
  } // end if P' is outside ABC

  // std::cout << "dist = " << dist << '\n';

  // sign
  // vec3 ap = p - a;
  float temp = dot(p - a, n);
  // if P is on the same plane of triangle
  // if (temp == 0) {
  //   sign = 1.f;
  // } else {
  //   // std::cout << "dot(ap, n) = " << sign << '\n';
  //   sign = temp;
  //   sign /= abs(sign);
  // }

  // general case
  float sign = (temp == 0) ? 1.f : (temp / abs(temp));

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

/* Member functions of Grid */
// calculate a cell's index (or hash) by a given point
int Grid::calCellHash(vec3 pos) {
  int hx = int(floor(pos.x / cellSize));
  int hy = int(floor(pos.y / cellSize) * nOfCells.x);
  int hz = int(floor(pos.z / cellSize) * nOfCells.x * nOfCells.y);

  return (hx + hy + hz);
}

// retrieve signed distance by cell's hash
float Grid::getDistance(int hash) { return cells[hash].sd; }

// retrieve signed distance by point position
float Grid::getDistance(vec3 p) {
  // restriction
  ivec3 idx = floor(p / cellSize);

  // std::cout << "hash = " << hash << '\n';
  // std::cout << "cells.size() = " << cells.size() << '\n';

  // note that grid origin is set to world origin (0, 0, 0)
  if (idx.x < 0 || idx.x > nOfCells.x - 1) {
    return 9999.f;
  } else if (idx.y < 0 || idx.y > nOfCells.y - 1) {
    return 9999.f;
  } else if (idx.z < 0 || idx.z > nOfCells.z - 1) {
    return 9999.f;
  } else {
    int hash = calCellHash(p);
    return cells[hash].sd;
  }
}

// retrieve gradient of a point p
vec3 Grid::getGradient(vec3 p) {
  vec3 grad;

  // currently, grid origin is set to world origin (0, 0, 0)
  // if not, be careful to transform p to reference frame
  vec3 pref = p;

  // calculate alpha (or the "normalized" position inside a cell)
  vec3 pCellPos = pref / cellSize;
  vec3 pCellPosFloor = glm::floor(pCellPos);
  vec3 alpha = pCellPos - pCellPosFloor;

  // std::cout << "alpha = " << to_string(alpha) << '\n';

  // interpolation along x (xy plane)
  float temp1 = getDistance(vec3(p.x + cellSize, p.y, p.z)) -
                getDistance(vec3(p.x - cellSize, p.y, p.z));
  float temp2 = getDistance(vec3(p.x + cellSize, p.y + cellSize, p.z)) -
                getDistance(vec3(p.x - cellSize, p.y + cellSize, p.z));
  grad.x = lerp(temp1, temp2, alpha.y);

  // interpolation along y (yz plane)
  float temp3 = getDistance(vec3(p.x, p.y + cellSize, p.z)) -
                getDistance(vec3(p.x, p.y - cellSize, p.z));
  float temp4 = getDistance(vec3(p.x, p.y + cellSize, p.z + cellSize)) -
                getDistance(vec3(p.x, p.y - cellSize, p.z + cellSize));
  grad.y = lerp(temp3, temp4, alpha.z);

  // interpolation along z (zx plane)
  float temp5 = getDistance(vec3(p.x, p.y, p.z + cellSize)) -
                getDistance(vec3(p.x, p.y, p.z - cellSize));
  float temp6 = getDistance(vec3(p.x + cellSize, p.y, p.z + cellSize)) -
                getDistance(vec3(p.x + cellSize, p.y, p.z - cellSize));
  grad.z = lerp(temp5, temp6, alpha.x);

  // be careful to check the direction of grad before using it
  // in some cases, "-grad" is appropriate
  return glm::normalize(-grad);
}
