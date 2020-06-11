#include "kdtree.h"
#include "point2d.h"
#include <vector>
#include "drawutils.h"


using namespace std;


class TestKDTree {

private:
  KDTree<Point2D<double>> kd;
  GLScene s;

public:
  void test_createTree() {
  
    srand(time(0));
    int N = 100;
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

    vector<Point2D<double>> tr = kd.PreOrder();
    s.addPoints2D(tr, tr.size(), "GL_POINTS");

    vector<Point2D<double>> grid;

    cout << "grid\n";
    
    for (auto e: tr) {
      cout << e << "\n";
    }

    for (int i = 0; i < tr.size(); i++) {
      int j = i%2;
      if (j == 0) {
	Point2D<double> a = {tr[i][0],-30};
	Point2D<double> b = {tr[i][0],30};
	grid.push_back(a);
	grid.push_back(b);
      }
      if(j == 1) {
	Point2D<double> a = {-30,tr[i][1]};
	Point2D<double> b = {30,tr[i][1]};
	grid.push_back(a);
	grid.push_back(b);
      }
    }

    s.addPoints2D(grid, grid.size(), "GL_LINES");
    
    //  s.addPoints2D(points, N/2, "GL_POINTS");
    
    typedef void (TestKDTree::*Fun) (double x, double y);
    Fun fp = &TestKDTree::getNearestNeighbor;
    s.setClickPositionCallback(fp, this);
    
    
    s.draw();
    
  }
  
  void getNearestNeighbor(double xpos, double ypos) {
    Point2D<double> d = {xpos, ypos};
    Point2D<double> nn = kd.nearestNeighbor(&kd, d, 2);
    cout << "Cccursor Position at " << xpos << " : " << ypos << endl;
    cout << "Nearest neighbor is :" << nn << endl;

    vector<Point2D<double>> v;
    v.push_back(d);
    v.push_back(nn);
    
    s.addPoints2D(v, v.size(), "GL_LINE_STRIP");
  }

};
  
int main() {

  TestKDTree t;
  t.test_createTree();

}
