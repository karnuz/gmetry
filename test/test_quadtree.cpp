#include "quadtree.h"
#include "point2d.h"
#include <vector>


using namespace std;


class TestQuadTree {

private:
  QuadTree<Point2D<double>> qd;

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

    

    int M = 10000;
    vector<double> testVec(M);
    generate(testVec.begin(), testVec.end(), []()
					     {
					       return static_cast <double> (rand() % 2000 - 1000)/50.0;
					     });

    vector<Point2D<double>> tpoints(M/2);
    
    for (int i = 0; i < M/2; i++) {
      tpoints[i].x = testVec[2*i];
      tpoints[i].y = testVec[2*i+1];
    }
    
    //    Point2D<double> p = {6.06, -19.12};
    //    vector<Point2D<double>> tpoints{p};

    for(auto e : tpoints) {
      Point2D<double> a =  getNearestNeighborTrue(e, points);
      Point2D<double> b = getNearestNeighbor(e);
      if(!(a==b)) {
	cout << "this: " << e << a << b ;
      }
    }
    
  }

  Point2D<double> getNearestNeighborTrue(Point2D<double> p, vector<Point2D<double>> v) {
    Point2D<double> q = v[0];
    double d = distSq(p,q);
    for(auto e: v) {
      double r = distSq(e,p);
      if(r < d) {
	q = e;
	d = r;
      }
    }
    return q;
  }
  
  Point2D<double> getNearestNeighbor(Point2D<double> p) {
    
    Point2D<double> nn = qd.nearestNeighbor(p);
    return nn;
  }


  
};
  
int main() {

  TestQuadTree t;
  t.test_createTree();

}
