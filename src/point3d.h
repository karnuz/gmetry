#ifndef _POINTS_H_
#define _POINTS_H_

#include "utils.h"

using namespace std;

struct Point3D {
  float x, y, z;

public:
  bool operator == (Point3D const &obj) const {
					     bool res;
					     res = (floatequal(obj.x,x)) && (floatequal(obj.y, y)) && (floatequal(obj.z, z)); 
					     return res;
  }

};


float distSq(const Point3D &p1, const Point3D &p2) {
  float a = (p2.y - p1.y);
  float b = (p2.x - p1.x);
  float c = (p2.z - p1.z);
  return a*a + b*b + c*c;
}


#endif
