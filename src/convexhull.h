#ifndef _CONVEXHULL_H_
#define _CONVEXHULL_H_

#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include "point2d.h"
#include "convexhull.h"
#include "utils.h"
#include <stdlib.h> // qsort
#include "line.h"


using namespace std;

template <typename T>
int findMinYXIndex(vector<Point2D<T>> p_vec);
template <typename T>
int findMinYXIndex(Point2D<T> points[], int n);

int compare(const void *vp1, const void *vp2);
  
// compare as per polar angle with reespect to private field p0. If p1 < p2 then it returns -1 else 1

template <typename T>
Point2D<T> p0;

template <typename T>
class ConvexHull {
 private:
  vector<Point2D<T>> p_vec;
  //Point2D<T> p;
  
 public:
  vector<Point2D<T>> grahamScan(Point2D<T> points[], int n);
  vector<Point2D<T>> incrementHull(const vector<Point2D<T>> hull, const Point2D<T> p);
  void sortPolar(Point2D<T> p_vec[], int n);
  void sortPolar(vector<Point2D<T>> &p_vec);

  struct PolarCompare {

    Point2D<T> p;

    
  public:

    PolarCompare(Point2D<T> p0) {
      p = p0;
    }
    
    bool operator () (const Point2D<T>& p1, const Point2D<T>& p2) const {
      int o = get_orientation(p, p1, p2);
      if(o == 0) {
	double d1 = distSq(p, p1);
	double d2 = distSq(p, p2);
	return (d2 > d1)? true:false;
      }
      return o == 1 ? true:false;
    }
  };

  
  //private:
  //int polarCompare(const Point2D<T> &p1, const Point2D<T> &p2);
};


// sorts point in place
template <typename T>
void ConvexHull<T>::sortPolar(vector<Point2D<T>> &p_vec) {
  int min = findMinYXIndex(p_vec);
  Point2D<T> temp = p_vec[min];
  p_vec[min] = p_vec[0];
  p_vec[0] = temp;

  // Both method below give same answer, and it is wrong.
  
  // 1.
  // using independent polarcompare function
  //p0 = p_vec[0];
  //sort(p_vec.begin()+1, p_vec.end(), ppolarCompare );

  //2.
  // using class polar compare function
  //this->p = p_vec[0];
  //sort(p_vec.begin()+1, p_vec.end() , [this](Point2D x, Point2D y) { return this->polarCompare(x,y);});

  Point2D<T> p = p_vec[0];
  
  auto comp = [&](Point2D<T> p1, Point2D<T> p2)-> bool {
      int o = get_orientation(p, p1, p2);
      if(o == 0) {
	T d1 = distSq(p, p1);
	T d2 = distSq(p, p2);
	return (d2 > d1)? true:false;
      }
      return o == 1 ? true:false;
  };
  
  //PolarCompare comp(p_vec[0]);
  sort(p_vec.begin()+1, p_vec.end() ,comp);
  
}




// sorts points in place
//typedef std::size_t SIZE;

template <typename T>
void ConvexHull<T>::sortPolar(Point2D<T> points[], int n) {
  
  int min = findMinYXIndex(points, n);
  Point2D temp = points[min];
  points[min] = points[0];
  points[0] = temp;
  //  PolarCompare comp(points[0]);
  //  p0 = points[0];
  //sort(&points[1], n-1, sizeof(Point2D<T>) , comp);
  vector<Point2D<T>> v(points, points + n); 
  for(int i=0; i < n; i++) {
    cout << v[i].x << " :: " << v[i].y << " n \n";
  }

  Point2D<T> p = v[0];
  
  auto comp = [&](Point2D<T> p1, Point2D<T> p2)-> bool {
      int o = get_orientation(p, p1, p2);
      if(o == 0) {
	T d1 = distSq(p, p1);
	T d2 = distSq(p, p2);
	return (d2 > d1)? -1:1;
      }
      return o == 1 ? -1:1;
  };
    
  sort(v.begin()+1, v.end(), PolarCompare(v[0]));
  //sort(v.begin()+1, v.end(), comp);
  for(int i=0; i < n; i++) {
    points[i] = v[i];
  }
}

template <typename T>
vector<Point2D<T>> ConvexHull<T>::grahamScan(Point2D<T> points[], int n) {
  assert(n >= 3);

  vector<Point2D<T>> convexHullBoundary;
  this->sortPolar(points,n);


  // enter first three points
  convexHullBoundary.push_back(points[0]);
  int i = 1;
  while(convexHullBoundary.size() < 3) {
    if(points[i] == points[i-1]) {
      i += 1;
      continue;
    } else {
      convexHullBoundary.push_back(points[i]);
      i += 1;
    }
  }


  // check and enter other points.
  for(; i < n; i++) {
    if(points[i] == points[i-1]) {
      continue;
    }
    while(true) {
      typename vector<Point2D<T>>::iterator it = convexHullBoundary.end();
      --it;
      Point2D<T> last = *it;
      --it;
      Point2D<T> secondlast = *it;
      int o = get_orientation(secondlast, last, points[i]);
      if(o < 0) {
	convexHullBoundary.pop_back();
	continue;
      } else {
	convexHullBoundary.push_back(points[i]);
	break;
      }
    }
  }
  return convexHullBoundary;  
}


template <typename T>
vector<Point2D<T>> ConvexHull<T>::incrementHull(const vector<Point2D<T>> hull, const Point2D<T> p) {

  vector<int> change_corners{-1,-1};
  vector<Point2D<T>> newhull;
  
  for(int i = 0; i < hull.size(); i++) {
    int j = i%hull.size();
    int k = (i+1)%hull.size();
    int l = (i+2)%hull.size();
    int o1 = get_orientation(hull[j], hull[k], p);
    if(o1 == 0 && isPointOnSegment(hull[j], hull[k], p)) {
      change_corners[0] = j;
      change_corners[1] = k;
      break;
    }
    int o2 = get_orientation(hull[k], hull[l], p);
    if(o2 == 0 && isPointOnSegment(hull[j], hull[l], p)) {
      change_corners[0] = k;
      change_corners[1] = l;
      break;
    }
    if(o1 == o2) {
      continue;
    } else {
      if (o1 == 0 && o2 == -1) {
	change_corners[0] = k;
      }
      else if (o1 == 0 && o2 == 1) {
	continue;
      }
      else if (o2 == 0 && o1 == -1) {
	change_corners[1] = k;
      }
      else if (o2 == 0 && o1 == 1) {
	continue;
      }
      else if (o1 == 1 && o2 == -1) {
	change_corners[0] = k;
      }
      else if (o1 == -1 && o2 == 1) {
	change_corners[1] = k;
      } 
    }
  }


  int idx = 0;
  bool inc = true;

  for(int m = 0; m < hull.size(); m ++) {
    if(m != change_corners[idx]) {
      if(inc) {
	newhull.push_back(hull[m]);
      }
      continue;
    } else {
      newhull.push_back(hull[m]);
      if(inc) {
	newhull.push_back(p);
	idx++;
      }
      inc = !inc;
    }
  }
  
  return newhull;
}




// polar comparison function
/*
template <typename T>
int compare(const void *vp1, const void *vp2) 
{
  //   assert(p0 != NULL);
  Point2D<T> *p1 = (Point2D<T> *)vp1; 
  Point2D<T> *p2 = (Point2D<T> *)vp2; 
  
   // Find orientation 
   int o = get_orientation(p0, *p1, *p2); 
   if (o == 0) 
     return (distSq(p0, *p2) >= distSq(p0, *p1))? -1 : 1; 
  
   return (o == 1)? -1: 1; 
}
*/

// Find the bottommost point
template <typename T>
int findMinYXIndex(Point2D<T> points[], int n) {
  
  int r = 0;
  int i;
  for (i=1; i < n; i++) {
    bool c1 = points[i].y < points[r].y;
    if(c1) {
	r = i;
    } else {
      bool c2 = points[i].y == points[r].y;
      if(c2) {
	bool c3 = points[i].x < points[r].x;
	if(c3) {
	  r = i;
	}
      }
    }
  }
  return r;
}



// returns the index of point which is least in Y. If more than two points are least in Y, returns the one least in X.
template <typename T>
int findMinYXIndex(vector<Point2D<T>> p_vec) {
  assert(p_vec.size() > 0);
  int r = 0;
  int i;
  for (i=1; i < p_vec.size(); i++) {
    bool c1 = p_vec[i].y < p_vec[r].y;
    if(c1) {
	r = i;
    } else {
      bool c2 = p_vec[i].y == p_vec[r].y;
      if(c2) {
	bool c3 = p_vec[i].x < p_vec[r].x;
	if(c3) {
	  r = i;
	}
      }
    }
  }
  return r;
}


#endif
