#include "quadtree.h"
#include "point2d.h"
#include <vector>
#include "drawutils.h"


using namespace std;


class TestQuadTree {

private:
  QuadTree<Point2D<double>> qd;
  GLScene s;

public:
  TestQuadTree() : qd(-20,20,-20,20,5)
  {
  }
  
  void test_createTree() {
  
    srand(time(0));
    int N = 1000;
    vector<double> vecOfRandomNums(N);
    
    generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), []()
							     {
							       return static_cast <double> (rand() % 2000 - 1000)/50.0;
                                                           });
    
    vector<Point2D<double>> points(N/2);
    
    for (int i = 0; i < N/2; i++) {
      points[i].x = vecOfRandomNums[2*i];
      points[i].y = vecOfRandomNums[2*i+1];
      //      qd.insert(points[i]);
    }

    
    qd.createTree(points.begin(),points.end());

    
    
    vector<Point2D<double>> tr = qd.bfs();
    s.addPoints2D(tr, tr.size(), "GL_POINTS");

    vector<Point2D<double>> bd = qd.getBounds();
    s.addPoints2D(bd, bd.size(), "GL_LINES");
    
    //  s.addPoints2D(points, N/2, "GL_POINTS");
    
    typedef void (TestQuadTree::*Fun) (double x, double y);
    Fun fp = &TestQuadTree::getNearestNeighbor;
    s.setClickPositionCallback(fp, this);
        
    s.draw();


    
  }

  
  void getNearestNeighbor(double xpos, double ypos) {

    std::clock_t start;
    double duration;
    start = std::clock();

    Point2D<double> pt = {xpos, ypos};
    Point2D<double> nn = qd.nearestNeighbor(pt);
    cout << "Cccursor Position at " << xpos << " : " << ypos << endl;
    cout << "Nearest neighbor is :" << nn << endl;

    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    cout<< "Time taken: "<< duration <<'\n';

    vector<Point2D<double>> v;
    v.push_back(pt);
    v.push_back(nn);
    
    s.addPoints2D(v, v.size(), "GL_LINE_STRIP");
  }  

};
  
int main() {

  TestQuadTree t;
  t.test_createTree();

}
