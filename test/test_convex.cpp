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

  Point2D<int> p1 = {0, 0}, p2 = {4, 4}, p3 = {5, 5}, p4 = {8, 8} ;
  Line2D<int> l1 = {p1, p2}, l2 = {p3, p4};

  assert(doesIntersect(l1, l2) == false);

}


void test_sortPolar() {
  //  Point p1 = {2,1}, p2 = {3,1}, p3 = {4,2}, p4 = {4,3}, p5 = {2,3};

  Point2D<float> p0;

  cout << p0.x << "," << p0.y << "\n";
  
  Point2D<float> p1 = {1.0,1.0}, p2 = {2.0,2.0}, p3 = {3.0,4.0}, p4 = {4.0,8.0}, p5 = {5.0,16.0};
  
  ConvexHull<float> c;

  Point2D<float> points[] = {p3,p2,p4,p5,p1};
  c.sortPolar(points, 5);
  //assert(false);

  cout<< "Array Sort\n";
  
  for(int i=0; i < 5; i++) {
    cout << points[i].x << " " << points[i].y << "\n";
  }
  cout << "\n\n\n";

  

  vector<Point2D<float>> p_vec{p3,p2,p4,p5,p1};
  c.sortPolar(p_vec);

  cout << "Vector Sort\n";
  for(int i=0; i < 5; i++) {
    cout << p_vec[i].x << " " << p_vec[i].y << "\n";
  }

  
  vector<Point2D<float>> p_vec_exp = {p1,p2,p3,p4,p5};

  //  assert(p_vec_exp == p_vec);
}



void test_grahamScan() {
  //  Point2D p1 = {2,1}, p2 = {3,1}, p3 = {4,2}, p4 = {4,3}, p5 = {2,3};

  Point2D<float> p0;

  cout << p0.x << "," << p0.y << "\n";
  
  Point2D<float> p1 = {1,1}, p2 = {2,2}, p3 = {3,4}, p4 = {4,8}, p5 = {5,16}, p6 = {6,8}, p7 = {5,4.4}, p8 = {3,2}, p9 = {4,4}, p10 = {3,2}, p11 = {-1,5};
  
  ConvexHull<float> c;

  Point2D<float> points[] = {p3,p10,p2,p4,p7,p5,p9,p11,p1,p6,p8};


  vector<Point2D<float>> hull;
  hull = c.grahamScan(points, 10);

  cout << "hull\n";
  vector<Point2D<float>>::iterator it = hull.begin();
  while(it != hull.end())
  {
    cout << (*it).x << " " << (*it).y << "\n";
    it++;
  }

  Point2D<float> p12 = {20,30};
  vector<Point2D<float>> newhull = c.incrementHull(hull, p12);

  cout << "\n\nnewhull\n";
  it = newhull.begin();
  while(it != newhull.end())
  {
    cout << (*it).x << " " << (*it).y << "\n";
    it++;
  }  

}



int main(int argc, char** argv) {

  test_doesIntersect();
  test_sortPolar();
  test_grahamScan();
}
B
