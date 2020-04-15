#ifndef _POINTS_H_
#define _POINTS_H_

struct Point {
  float x, y;

public:
  bool operator == (Point const &obj) const {
					     bool res;
					     res = (obj.x == x) && (obj.y == y); 
					     return res;
  }

};

int get_orientation(Point p1, Point p2, Point p3);

float distSq(const Point &p1, const Point &p2); 

using namespace std;

// returns 0 if points are collinear, 1 if counter-clockwise, -1 if clockwise
int get_orientation(Point p1, Point p2, Point p3) {
  float val;
  //  val = (p2.y - p1.y) * (p3.x - p2.x) - 
  //  (p2.x - p1.x) * (p3.y - p2.y); 

  val = (p3.y - p2.y)*(p2.x - p1.x) - (p2.y - p1.y)*(p3.x - p2.x);
  if (val > 0) {
    return 1; // counter clockwise
  } else if (val < 0) {
    return -1; // clockwise
  } else {
    return 0; // collinear
  }  
}

float distSq(const Point &p1, const Point &p2) {
  float a = (p2.y - p1.y);
  float b = (p2.x - p1.x);
  return a*a + b*b;
}


#endif
