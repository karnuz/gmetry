#include "kdtree.h"
#include "point2d.h"
#include <vector>
#include "drawutils.h"


using namespace std;


class TestKDTree {

private:
  KDTree<Point2D<double>> kd;

public:
  void test_createTree() {
  
    srand(time(0));
    int N = 10000;
    vector<double> vecOfRandomNums(N);
    
    generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), []()
							     {
							       return static_cast <double> (rand() % 2000 - 1000)/50.0;
                                                           });
    
    vector<Point2D<double>> points(N/2);
    
    for (int i = 0; i < N/2; i++) {
      points[i].x = vecOfRandomNums[2*i];
      points[i].y = vecOfRandomNums[2*i+1];
    }
    
    kd.createTree(points.begin(),points.end(),2, points.size());

    vector<Point2D<double>> tr = kd.InOrder();
    GLScene s;
    s.addPoints2D(tr, tr.size(), "GL_POINTS");
    
    //  s.addPoints2D(points, N/2, "GL_POINTS");
    
    typedef void (TestKDTree::*Fun) (double x, double y);
    Fun fp = &TestKDTree::getNearestNeighbor;
    s.setClickPositionCallback(fp, this);
    
    
    s.draw();
    
    //    Point2D<double> d = {7.29,-0.2};
    //    Point2D<double> nn = kd.nearestNeighbor(&kd, d, 2);
    
    //    cout << "nearest neighbor to " << d << " is : " << nn;
    //  for (auto e: tr) {
    //  cout << e << "\n";
    //}
    
    
  }
  
  void getNearestNeighbor(double xpos, double ypos) {
    Point2D<double> d = {xpos, ypos};
    Point2D<double> nn = kd.nearestNeighbor(&kd, d, 2);
    cout << "Cccursor Position at " << xpos << " : " << ypos << endl;

    cout << "Nearest neighbor is :" << nn << endl;
  }

};
  
int main() {

  TestKDTree t;
  t.test_createTree();

}
