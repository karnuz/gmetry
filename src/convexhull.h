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

using namespace std;


int findMinYXIndex(vector<Point2D> p_vec);
int findMinYXIndex(Point2D points[], int n);
int compare(const void *vp1, const void *vp2);
  
// compare as per polar angle with reespect to private field p0. If p1 < p2 then it returns -1 else 1

Point2D p0;


class ConvexHull {
 private:
  vector<Point2D> p_vec;
  Point2D p;
  
 public:
  vector<Point2D> grahamScan(Point2D points[], int n);

  void sortPolar(Point2D p_vec[], int n);
  void sortPolar(vector<Point2D> &p_vec);
  
private:
  int polarCompare(const Point2D &p1, const Point2D &p2);
};


int ppolarCompare(const Point2D &p1, const Point2D &p2) {
  int o = get_orientation(p0, p1, p2);
  if(o == 0) {
    double d1 = distSq(p0, p1);
    double d2 = distSq(p0, p2);
    return (d2 > d1)? -1:1;
  }
  return o == 1 ? -1:1;
}


int ConvexHull::polarCompare(const Point2D &p1, const Point2D &p2) {
  int o = get_orientation(this->p, p1, p2);
  if(o == 0) {
    double d1 = distSq(this->p, p1);
    double d2 = distSq(this->p, p2);
    return (d2 > d1)? -1:1;
  }
  return o == 1 ? -1:1;
}


// sorts point in place
void ConvexHull::sortPolar(vector<Point2D> &p_vec) {
  int min = findMinYXIndex(p_vec);
  Point2D temp = p_vec[min];
  p_vec[min] = p_vec[0];
  p_vec[0] = temp;


  // Both method below give same answer, and it is wrong.
  
  // 1.
  // using independent polarcompare function
  //p0 = p_vec[0];
  //sort(p_vec.begin()+1, p_vec.end(), ppolarCompare );

  //2.
  // using class polar compare function
  this->p = p_vec[0];
  sort(p_vec.begin()+1, p_vec.end() , [this](Point2D x, Point2D y) { return this->polarCompare(x,y);});
}

// sorts point in place
void ConvexHull::sortPolar(Point2D points[], int n) {
  
  int min = findMinYXIndex(points, n);   
  Point2D temp = points[min];
  points[min] = points[0];
  points[0] = temp;
  p0 = points[0];
  qsort(&points[1], n-1, sizeof(Point2D) , compare);
}


vector<Point2D> ConvexHull::grahamScan(Point2D points[], int n) {
  assert(n >= 3);

  vector<Point2D> convexHullBoundary;
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
      vector<Point2D>::iterator it = convexHullBoundary.end();
      --it;
      Point2D last = *it;
      --it;
      Point2D secondlast = *it;
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

/*
vector<Point2D> ConvexHull::incrementHull(Point2D points[], int n) {
  assert(n >= 3);

  vector<Point2D> convexHullBoundary;
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
      vector<Point2D>::iterator it = convexHullBoundary.end();
      --it;
      Point2D last = *it;
      --it;
      Point2D secondlast = *it;
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


vector<Point2D> ConvexHull::incremental(Point2D points[], int n) {
  assert(n >= 3);

  vector<Point2D> convexHullBoundary;
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
      vector<Point2D>::iterator it = convexHullBoundary.end();
      --it;
      Point2D last = *it;
      --it;
      Point2D secondlast = *it;
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
*/


// polar comparison function 
int compare(const void *vp1, const void *vp2) 
{
  //   assert(p0 != NULL);
   Point2D *p1 = (Point2D *)vp1; 
   Point2D *p2 = (Point2D *)vp2; 
  
   // Find orientation 
   int o = get_orientation(p0, *p1, *p2); 
   if (o == 0) 
     return (distSq(p0, *p2) >= distSq(p0, *p1))? -1 : 1; 
  
   return (o == 1)? -1: 1; 
}


// Find the bottommost point 
int findMinYXIndex(Point2D points[], int n) {
  
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
int findMinYXIndex(vector<Point2D> p_vec) {
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
