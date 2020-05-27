#include <iostream>
#include <cassert>
#include "line.h"
#include "convexhull.h"
#include <math.h> 
#include <algorithm>

#include "drawutils.h"
#include <vector>
using namespace std;


void test_doesIntersect() {

  Point2D<int> p1 = {0, 0}, p2 = {4, 4}, p3 = {5, 5}, p4 = {8, 8} ;
  Line2D<int> l1 = {p1, p2}, l2 = {p3, p4};

  assert(doesIntersect(l1, l2) == false);

}


void test_sortPolar() {
  //  Point2D p1 = {2,1}, p2 = {3,1}, p3 = {4,2}, p4 = {4,3}, p5 = {2,3};

  Point2D<int> p0;

  cout << p0.x << "," << p0.y << "\n";
  
  Point2D<int> p1 = {1,1}, p2 = {2,2}, p3 = {3,4}, p4 = {4,8}, p5 = {5,16};
  
  ConvexHull<int> c;

  Point2D<int> points[] = {p3,p2,p4,p5,p1};
  c.sortPolar(points, 5);
  //assert(false);

  cout<< "\n]nArray Sort\n";
  
  for(int i=0; i < 5; i++) {
    cout << points[i].x << " " << points[i].y << "\n";
  }


  vector<Point2D<int>> p_vec{p3,p2,p4,p5,p1};
  c.sortPolar(p_vec);

  cout << "\n\nVector Sort\n\n\n";
  for(int i=0; i < 5; i++) {
    cout << p_vec[i].x << " " << p_vec[i].y << "\n";
  }

  
  vector<Point2D<int>> p_vec_exp = {p1,p2,p3,p4,p5};

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

  cout << "\n\nhull\n";
  vector<Point2D<float>>::iterator it = hull.begin();

  GLScene s;
  s.addPoints2D(hull, hull.size(), "GL_LINE_LOOP");
  s.addPoints2D(points, 10, "GL_POINTS");
  
  s.draw();

}

void test_grahamScan2() {
  //  Point2D p1 = {2,1}, p2 = {3,1}, p3 = {4,2}, p4 = {4,3}, p5 = {2,3};

  srand(time(0));
  int N = 200;
  vector<float> vecOfRandomNums(N);

  generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), []()
							   {
							     return (rand() % 2000 - 1000)/50;
							   });
    

  Point2D<float> points[N/2];

  for (int i = 0; i < N/2; i++) {
    points[i].x = vecOfRandomNums[2*i];
    points[i].y = vecOfRandomNums[2*i+1];
  }
  
  
  ConvexHull<float> c;

  vector<Point2D<float>> hull;
  hull = c.grahamScan(points, N/2);

  cout << "\n\nhull\n";
  vector<Point2D<float>>::iterator it = hull.begin();

  GLScene s;
  s.addPoints2D(hull, hull.size(), "GL_LINE_LOOP");
  s.addPoints2D(points, N/2, "GL_POINTS");
  
  s.draw();



}



class TestIncremental {
private:
  //  Point2D points[];
  ConvexHull<float> c;
  vector<Point2D<float>> hull;
  GLScene s;

public:

  void incrementHull(double xpos, double ypos) {
    cout << "Cursor Clicked at " << xpos << " : " << ypos << endl;


    /*
    float x = (float) xpos;
    float y = (float) ypos;
    Point2D<float> p = {100*x, 100*y};
    vector<Point2D<float>> newhull = c.incrementHull(hull,p);

    cout << newhull.size() << "  newhull size\n";
    for(auto e: newhull) {
      cout << e << "this\n";
    }
    
    Point2D<float> points[] = {p};
    
    s.addPoints2D(newhull, newhull.size(), "GL_LINE_LOOP");
    s.addPoints2D(points, 1, "GL_POINTS");
    cout << "2Cccccccursor Position at " << 100*xpos << " : " << 100*ypos << endl;
    */
  }


  void test_Incremental() {

    srand(time(0));
    int N = 200;
    vector<float> vecOfRandomNums(N);
    
    generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), []()
							     {
							       return (rand() % 2000 - 1000)/50;
							     });
    
    
    Point2D<float> points[N/2];
    
    for (int i = 0; i < N/2; i++) {
      points[i].x = vecOfRandomNums[2*i];
      points[i].y = vecOfRandomNums[2*i+1];
    }
    
    
    
    hull = c.grahamScan(points, N/2);
    
    cout << "\n\nhull\n";
    vector<Point2D<float>>::iterator it = hull.begin();
    
    s.addPoints2D(hull, hull.size(), "GL_LINE_LOOP");
    s.addPoints2D(points, N/2, "GL_POINTS");
    
    
    typedef void (TestIncremental::*Fun) (double x, double y);
    Fun fp = &TestIncremental::incrementHull;
    //auto fp = std::bind(&TestIncremental::incrementHull, this, _1, _2);
    //    s.setClickPositionCallback(fp,this);
    s.setClickPositionCallback(fp, this);
  
    
    
    s.draw();
  }

};




int main(int argc, char** argv) {

  test_doesIntersect();
  test_sortPolar();
  //  test_grahamScan2();

  TestIncremental t;
  t.test_Incremental();
}
