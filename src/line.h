#ifndef _LINE2D_H_
#define _LINE2D_H_

#include "point2d.h"
#include <iostream>
#include "utils.h"
#include <cstdlib>
#include <limits>

template <typename T>
struct Line2D {
  Point2D<T> p1;
  Point2D<T> p2;
};

template <typename T>
bool isPointOnSegment(Point2D<T> p1, Point2D<T> p2, Point2D<T> q);
template <typename T>
bool doesIntersect (Line2D<T> l1, Line2D<T> l2);


using namespace std;

template <typename T>
float getSlope(Line2D<T> l) {
  T x1, y1, x2, y2;
  x1 = l.p1.x;
  x2 = l.p2.x;
  y1 = l.p1.y;
  y2 = l.p2.y;

  if(x1 == x2) {
    return numeric_limits<T>::infinity();
  } else {
    T m = (y2 - y1)/(x2 - x1);
    return m;
  }
}

template <typename T>
bool isPointOnSegment(Point2D<T> p1, Point2D<T> p2, Point2D<T> q) {
  T minx, maxx, miny, maxy;
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

template <typename T>
bool doesIntersect(Line2D<T> l1, Line2D<T> l2) {
  Point2D<T> p1, p2, q1, q2;
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
