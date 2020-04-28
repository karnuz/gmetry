#include <iostream>
#include <cassert>
#include "line.h"

using namespace std;

int main() {
  Point2D p1 = {0, 0}, p2 = {4, 4}, p3 = {5, 5}, p4 = {8, 8} ;
  Line l1 = {p1, p2}, l2 = {p3, p4};

  assert(doesIntersect(l1, l2) == false);
  return 0;
}
