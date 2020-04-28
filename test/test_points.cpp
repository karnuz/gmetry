#include <iostream>
#include <cassert>
#include "point2d.h"

using namespace std;



bool test_distSq(){

  Point2D p1 = {2.0, 3.0};
  Point2D p2 = {4.0, -5.0};
  assert(distSq(p1,p2) == 68.0);
  return true;
}

bool test_getOrientation() {
  Point2D p1 = {0, 0}, p2 = {4, 4}, p3 = {2, 4}; 
  // 0 == Linear
  // -1 == Clockwise
  // 1 == Counter-clockwise
  assert(get_orientation(p1, p2, p3) == 1);
  return true;
}

int main() {
  test_distSq();
  test_getOrientation();
  return 0;
}


