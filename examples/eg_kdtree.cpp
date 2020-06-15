#include "kdtree.h"
#include "point2d.h"
#include <vector>
#include "drawutils.h"
#include <queue>

using namespace std;


class TestKDTree {

private:
  KDTree<Point2D<double>> kd;
  GLScene s;

public:
  void test_createTree() {
  
    srand(time(0));
    int N = 50;
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


    cout << "grid\n";
    
    for (auto e: tr) {
      cout << e << "\n";
    }


    vector<Point2D<double>> grid;
    Node<Point2D<double>>* ptr = kd.getRoot();
    queue<tuple<Node<Point2D<double>> *, double, double, double, double, int>> v;
    v.push(tuple(ptr,-30,30,-30,30,0));

    while(!v.empty()) {
      auto t = v.front();
      v.pop();
      ptr = get<0>(t);
      double a = get<1>(t);
      double b = get<2>(t);
      double c = get<3>(t);
      double d = get<4>(t);
      int l = get<5>(t);
      int j = l%2;
      Point2D<double> data = ptr->GetData();
      Point2D<double> p;
      Point2D<double> q;
      if(j == 0) {
	p = {data[0],c};
	q = {data[0],d};
	grid.push_back(p);
	grid.push_back(q);
	if(ptr->IsLeft()){
	  v.push(tuple(ptr->GetLeft(),a,data[0],c,d,l+1));
	}
	if(ptr->IsRight()){
	  v.push(tuple(ptr->GetRight(),data[0],b,c,d,l+1));
	}
      }
      if(j == 1) {
	p = {a,data[1]};
	q = {b,data[1]};
	grid.push_back(p);
	grid.push_back(q);
	if(ptr->IsLeft()){
	  v.push(tuple(ptr->GetLeft(),a,b,c,data[1],l+1));
	}
	if(ptr->IsRight()){
	  v.push(tuple(ptr->GetRight(),a,b,data[1],d,l+1));
	}
      }
    }
    
    
    


    /*

    
    for (int i = 0; i < tr.size(); i++) {
      int j = i%2;
      if (j == 0) {
	Point2D<double> a = {tr[i][0],b1};
	Point2D<double> b = {tr[i][0],b2};
	
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
    */
    s.addPoints2D(grid, grid.size(), "GL_LINES");

    
    typedef void (TestKDTree::*Fun) (double x, double y);
    Fun fp = &TestKDTree::getNearestNeighbor;
    s.setClickPositionCallback(fp, this);
    
    
    s.draw();
    
  }
  
  void getNearestNeighbor(double xpos, double ypos) {
    Point2D<double> d = {xpos, ypos};
    Point2D<double> nn = kd.nearestNeighbor2(&kd, d, 2);
    cout << "Cccursor Position at " << xpos << " : " << ypos << endl;
    cout << "Nearest neighbor is :" << nn << endl;

    Point2D<double> nn2 = kd.nearestNeighbor(&kd, d, 2);

    vector<Point2D<double>> v;
    v.push_back(d);
    v.push_back(nn);
    v.push_back(d);
    v.push_back(nn2);
    
    s.addPoints2D(v, v.size(), "GL_LINE_STRIP");
    //    s.addPoints2D(v, v.size(), "GL_LINE_STRIP");
  }

};
  
int main() {

  TestKDTree t;
  t.test_createTree();

}
