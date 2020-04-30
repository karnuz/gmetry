#include <iostream>
#include <cassert>
#include "line.h"
#include "convexhull.h"
#include <math.h> 
#include <algorithm>
#define pi 3.142857

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

using namespace std;


void test_doesIntersect() {

  Point2D p1 = {0, 0}, p2 = {4, 4}, p3 = {5, 5}, p4 = {8, 8} ;
  Line l1 = {p1, p2}, l2 = {p3, p4};

  assert(doesIntersect(l1, l2) == false);

}


void test_sortPolar() {
  //  Point2D p1 = {2,1}, p2 = {3,1}, p3 = {4,2}, p4 = {4,3}, p5 = {2,3};

  Point2D p0;

  cout << p0.x << "," << p0.y << "\n";
  
  Point2D p1 = {1,1}, p2 = {2,2}, p3 = {3,4}, p4 = {4,8}, p5 = {5,16};
  
  ConvexHull c;

  Point2D points[] = {p3,p2,p4,p5,p1};
  c.sortPolar(points, 5);
  //assert(false);

  cout<< "Array Sort\n";
  
  for(int i=0; i < 5; i++) {
    cout << points[i].x << " " << points[i].y << "\n";
  }
  cout << "\n\n\n";


  vector<Point2D> p_vec{p3,p2,p4,p5,p1};
  c.sortPolar(p_vec);

  cout << "Vector Sort\n";
  for(int i=0; i < 5; i++) {
    cout << p_vec[i].x << " " << p_vec[i].y << "\n";
  }

  
  vector<Point2D> p_vec_exp = {p1,p2,p3,p4,p5};

  //  assert(p_vec_exp == p_vec);
}





int main(int argc, char** argv) {

  test_doesIntersect();
  test_sortPolar();
}
