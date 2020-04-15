#ifndef _LINES_H_
#define _LINES_H_

#include "point.h"
#include <iostream>
#include "utils.h"
#include <cstdlib>
#include <limits>

struct Line {
  Point p1;
  Point p2;
};

bool isPointOnSegment(Point p1, Point p2, Point q);
bool doesIntersect (Line l1, Line l2);


using namespace std;


float getSlope(Line l) {
  float x1, y1, x2, y2;
  x1 = l.p1.x;
  x2 = l.p2.x;
  y1 = l.p1.y;
  y2 = l.p2.y;

  if(x1 == x2) {
    return numeric_limits<float>::infinity();
  } else {
    float m = (y2 - y1)/(x2 - x1);
    return m;
  }
}


bool isPointOnSegment(Point p1, Point p2, Point q) {
  float minx, maxx, miny, maxy;
  minx = min(p1.x, p2.x);
  maxx = max(p1.x, p2.x);
  miny = min(p1.y, p2.y);
  maxy = max(p1.y, p2.y);

  if (q.x <= maxx && q.x >= minx && q.y <= maxy && q.y >= miny) {
    return true;
  } else {
    return false;
  }
}


bool doesIntersect(Line l1, Line l2) {
  Point p1, p2, q1, q2;
  p1 = l1.p1;
  p2 = l1.p2;
  q1 = l2.p1;
  q2 = l2.p2;
  
  int o1, o2, o3, o4;

  // non collinear case
  o1 = get_orientation(p1, p2, q1);
  o2 = get_orientation(p1, p2, q2);
  o3 = get_orientation(q1, q2, p1);
  o4 = get_orientation(q1, q2, p2);
  cout << o1 << "\n" << o2 << "\n" << o3 << "\n" << o4 << "\n";

  if(o1 != o2 && o3 != o4) {
    return true;
  }

  //collinear case
  bool b1, b2;

  b1 = isPointOnSegment(q1, q2, p1);
  b2 = isPointOnSegment(q1, q2, p2);
  cout << b1 << "\n" << b2 << "\n";
  if (b1 || b2 ) {
    return true;
  }

  return false;
  
}

#endif
