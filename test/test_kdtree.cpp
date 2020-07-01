#include "kdtree.h"
#include "point2d.h"
#include <vector>
#include <queue>
#include <ctime>


using namespace std;


class TestKDTree {

private:
  KDTree<Point2D<double>> kd;


public:
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
      kd.insert(points[i], 2);
    }
    
    //    kd.createTree(points.begin(),points.end(),2, points.size());

    vector<Point2D<double>> tr = kd.PreOrder();
    

    
    int M = 100000;
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
      //cout << "searching: " << e << a << b << "\n";
      if(!(a==b) && dist(e,a)!=dist(e,b)) {
	cout << "this: " << e << " " << a << " "  << b << " " << dist(e,a) << " " << dist(e,b) << "\n\n\n";
	vector<Point2D<double>> path;
	Node<Point2D<double>>* nd = kd.getRoot();
	Node<Point2D<double>>* ptr = nd;
	int dim = 2;
	int level = 0;
	Point2D<double> data = a;
	while(true) {
	  Point2D<double> ndata = ptr->GetData();
	  int split_idx = level%dim;
	  path.push_back(ndata);
	  if(data[split_idx] == ndata[split_idx]) {
	    break;
	  }
	  else if (data[split_idx] < ndata[split_idx]) {
	      ptr = ptr->GetLeft();
	  }
	  else if (data[split_idx] > ndata[split_idx]){
	      ptr = ptr->GetRight();
	  }
	  level += 1;
	}

	for(auto e: path) {
	  cout << e << " ";
	}
	cout << "\n";
	return;
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
    
    Point2D<double> nn = kd.nearestNeighbor3(&kd, p, 2);
    return nn;
  }

};
  
int main() {

  TestKDTree t;
  t.test_createTree();

}
