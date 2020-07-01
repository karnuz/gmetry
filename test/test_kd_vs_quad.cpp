#include "kdtree.h"
#include "quadtree.h"
#include "point2d.h"
#include <vector>
#include <queue>
#include <ctime>


using namespace std;

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


class TestQuadTree {

private:
  QuadTree<Point2D<double>> qd;

public:
  TestQuadTree(double a, double b, double c, double d, double e) : qd(a,b,c,d,e)
  {
  }
  
  void test_quadTree(vector<Point2D<double>> points, vector<Point2D<double>> tpoints) {
    
    qd.createTree(points.begin(),points.end());
  
    std::clock_t start;
    double duration = 0;

    for(auto e : tpoints) {
      Point2D<double> a =  getNearestNeighborTrue(e, points);

      start = std::clock();  
      Point2D<double> b = getNearestNeighbor(e);
      duration += ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
      
      if(!(a==b) && dist(e,a)!=dist(e,b)) {
	cout << "this: " << e << a << b << "\n" ;
      }
    }

    cout<< "Time taken Quad Tree: "<< duration <<'\n'; 
    
  }

  Point2D<double> getNearestNeighbor(Point2D<double> p) {
    
    Point2D<double> nn = qd.nearestNeighbor(p);
    return nn;
  }
  
};
  


class TestKDTree {

private:
  KDTree<Point2D<double>> kd;
  KDTree<Point2D<double>> kds;

public:
  void test_kdTree(vector<Point2D<double>> points, vector<Point2D<double>> tpoints) {
      
    
    for (auto e : points) {
      kds.insert(e, 2);
    }
    
    kd.createTree(points.begin(),points.end(),2, points.size());
    
    //    vector<Point2D<double>> tr = kd.PreOrder();

    std::clock_t start;
    double duration = 0;
    double duration2 = 0;
    double duration3 = 0;
    double duration4 = 0;
    
    for(auto e : tpoints) {

    
      Point2D<double> a =  getNearestNeighborTrue(e, points);
      
      start = std::clock();
      Point2D<double> b = getNearestNeighbor(e);
      duration += ( std::clock() - start ) / (double) CLOCKS_PER_SEC;


      start = std::clock();
      Point2D<double> c = getNearestNeighbor2(e);
      duration2 += ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

      start = std::clock();
      Point2D<double> p = getNearestNeighborS(e);
      duration3 += ( std::clock() - start ) / (double) CLOCKS_PER_SEC;


      start = std::clock();
      Point2D<double> q = getNearestNeighbor2S(e);
      duration4 += ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

      //cout << "searching: " << e << a << b << "\n";
      if(!(a==b) && dist(e,a)!=dist(e,b)) {
	cout << "this: " << e << " " << a << " "  << b << " " << dist(e,a) << " " << dist(e,b) << "\n\n\n";
      }
      if(!(a==c) && dist(e,a)!=dist(e,c)) {
	cout << "this: " << e << " " << a << " "  << c << " " << dist(e,a) << " " << dist(e,c) << "\n\n\n";
      }
      if(!(a==p) && dist(e,a)!=dist(e,p)) {
	cout << "this: " << e << " " << a << " "  << p << " " << dist(e,a) << " " << dist(e,p) << "\n\n\n";
      }
      if(!(a==q) && dist(e,a)!=dist(e,q)) {
	cout << "this: " << e << " " << a << " "  << q << " " << dist(e,a) << " " << dist(e,q) << "\n\n\n";
      }

    }

    cout<< "Time taken KD Tree: "<< duration <<'\n';
    cout<< "Time taken KD Tree Efficient: "<< duration2 <<'\n';
    cout<< "Time taken Sequential Insert KD Tree: "<< duration3 <<'\n';
    cout<< "Time taken Sequential Insert KD Tree Efficient: "<< duration4 <<'\n';
    
  }
    
    
  Point2D<double> getNearestNeighbor(Point2D<double> p) {
    Point2D<double> nn = kd.nearestNeighbor3(&kd, p, 2);
    return nn;
  }
  
  Point2D<double> getNearestNeighbor2(Point2D<double> p) {
    Point2D<double> nn = kd.nearestNeighbor2(&kd, p, 2);
    return nn;
  }

  Point2D<double> getNearestNeighborS(Point2D<double> p) {
    Point2D<double> nn = kds.nearestNeighbor3(&kd, p, 2);
    return nn;
  }
  
  Point2D<double> getNearestNeighbor2S(Point2D<double> p) {
    Point2D<double> nn = kds.nearestNeighbor2(&kd, p, 2);
    return nn;
  }


};
  
int main() {

  srand(time(0));
  //srand(0);
  int N = 1000;
  vector<double> vecOfRandomNums(N);
  
  generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), []()
							   {
							     return static_cast <double> (rand() % 2000 - 1000)/0.5;
                                                           });
  
  vector<Point2D<double>> points(N/2);
  
  for (int i = 0; i < N/2; i++) {
    points[i].x = vecOfRandomNums[2*i];
    points[i].y = vecOfRandomNums[2*i+1];
  }



  int M = 2000;
  vector<double> testVec(M);
  generate(testVec.begin(), testVec.end(), []()
					   {
					     return static_cast <double> (rand() % 2000 - 1000)/0.5;
					     });
  
  vector<Point2D<double>> tpoints(M/2);
  
  for (int i = 0; i < M/2; i++) {
    tpoints[i].x = testVec[2*i];
    tpoints[i].y = testVec[2*i+1];
  }

  
  TestKDTree t;
  t.test_kdTree(points, tpoints);


  TestQuadTree q(-2000,2000,-2000,2000,5);
  q.test_quadTree(points, tpoints);
  
  return 0;
}
