#ifndef _POINTS_H_
#define _POINTS_H_

using namespace std;

template<typename T>
struct Point2D {
  T x, y;

public:
  bool operator == (Point2D const &obj) const {
					     bool res;
					     res = (obj.x == x) && (obj.y == y); 
					     return res;
  }

  T operator [] (int i) {
    if (i == 0) {
      return x;
    }
    else if (i == 1) {
      return y;
    }
    else {
      throw out_of_range("This is a 2D point\n");
    }
  }
  
};



// returns 0 if points are collinear, 1 if counter-clockwise, -1 if clockwise
template <typename T>
int get_orientation(Point2D<T> p1, Point2D<T> p2, Point2D<T> p3) {
  T val;
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

template <typename T>
T distSq(const Point2D<T> &p1, const Point2D<T> &p2) {
  T a = (p2.y - p1.y);
  T b = (p2.x - p1.x);
  return a*a + b*b;
}

template<class T>
ostream& operator<<(ostream& os, const Point2D<T>& t)
{
  
  os << t.x << " " << t.y << "\n";
  return os;
}


#endif
