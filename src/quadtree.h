#ifndef _QUADTREE_H_
#define _QUADTREE_H_


#include "node.h"
#include "math.h"
#include "exceptions.h"
#include "utils.h"
#include "float.h"

using namespace std;

template <typename T>
struct PtInfo {
  T data;
  double d;
};

template <typename T>
class QuadNode {
public:
  double xmin, xmax, ymin, ymax;
  int bucket_size = 0;

  
public:

  QuadNode() {
  }

  QuadNode(double xmin, double xmax, double ymin, double ymax) {
    this->xmin = xmin;
    this->xmax = xmax;
    this->ymin = ymin;
    this->ymax = ymax;
    //    this->MAX_BUCKET_SIZE = max_bucket_size;
  }
  
  virtual QuadNode<T>* insert(T data) = 0;

  virtual bool empty() {
    return this->bucket_size == 0;
  }
  

  virtual int bucketsize() = 0;
  
  bool IsPointInQuad(T data) {
    //cout << xmin << " " << xmax << " " << ymin << " " << ymax << "\n"; 
    if (data[0] <= xmax && data[0] >= xmin && data[1] <= ymax && data[1] >= ymin) {
      return true;
    }
    return false;
  }

  virtual vector<T> bfs() = 0;

  virtual T nearestNeighbor(T p) = 0;

  virtual struct PtInfo<T> nearestNeighbor(T p, struct PtInfo<T> nn) = 0;

  virtual PtInfo<T> findNearestPointInQuad(T p) = 0;

  virtual vector<T> getBounds() = 0;
};


template <typename T>
class BoundaryNode: public QuadNode<T> {
public:
  QuadNode<T>* bl;
  QuadNode<T>* br;
  QuadNode<T>* tr;
  QuadNode<T>* tl;
  
public:

  int bucketsize() {

    return this->bucket_size;
    //return this->bl->bucketsize() + this->br->bucketsize() + this->tr->bucketsize() + this->tl->bucketsize();
    //    return -1;
  }

  vector<T> getBounds() {
    vector<T> a = bl->getBounds();
    vector<T> b = br->getBounds();
    vector<T> c = tl->getBounds();
    vector<T> d = tr->getBounds();
    //cout << a.size() << "size a" << "\n";
    //cout << b.size() << "size b" << "\n";
    //cout << c.size() << "size c" << "\n";
    //cout << d.size() << "size d" << "\n";
    a.insert(
      a.end(),
      std::make_move_iterator(b.begin()),
      std::make_move_iterator(b.end())
    );
    a.insert(
      a.end(),
      std::make_move_iterator(c.begin()),
      std::make_move_iterator(c.end())
    );
    a.insert(
      a.end(),
      std::make_move_iterator(d.begin()),
      std::make_move_iterator(d.end())
    );

    return a;
  }

  T nearestNeighbor(T p) {
    if(this->empty()) {
      throw no_point_in_quad_exception;
    }
    T q;
    struct PtInfo<T> nn;
    nn = this->findNearestPointInQuad(p);
    //cout << nn.data << "  this \n";
    //nn.d = 100000.0;
      //nn.data = q;
    nn = this->nearestNeighbor(p, nn);
    return nn.data;
  }


  // should be renamed - this name gives wrong impression
  struct PtInfo<T> findNearestPointInQuad(T p) {
    //cout << "point is " << p << "\n";
    if(!this->IsPointInQuad(p)) {
      PtInfo<T> nn;
      nn.d = DBL_MAX;
      return nn;
    }
    if(bl->IsPointInQuad(p) && !bl->empty()) {
      return bl->findNearestPointInQuad(p);
    }
    else if(br->IsPointInQuad(p) && !br->empty()) {
      return br->findNearestPointInQuad(p);
    }
    else if(tl->IsPointInQuad(p) && !tl->empty()) {
      return tl->findNearestPointInQuad(p);
    }
    else if(tr->IsPointInQuad(p) && !tr->empty()) {
      return tr->findNearestPointInQuad(p);
    } else {
      //cout << "hhhereree\n";
      vector<T> v = bfs();
      PtInfo<T> nn;
      nn.data = v[0];
      nn.d = dist(p, v[0]);
      for(auto e: v) {
	//cout << "all: " << e;
	double d = dist(p,e);
	if(d < nn.d) {
	  nn.d = d;
	  nn.data = e;
	}
      }
      return nn;
    }
  }


  vector<T> bfs() {
    //cout <<",boundarynode,datasize:" << this->bucket_size << "\n";
    vector<T> a = this->bl->bfs();
    vector<T> b = this->br->bfs();
    vector<T> c = this->tr->bfs();
    vector<T> d = this->tl->bfs();
    a.insert(
      a.end(),
      std::make_move_iterator(b.begin()),
      std::make_move_iterator(b.end())
    );
    a.insert(
      a.end(),
      std::make_move_iterator(c.begin()),
      std::make_move_iterator(c.end())
    );
    a.insert(
      a.end(),
      std::make_move_iterator(d.begin()),
      std::make_move_iterator(d.end())
    );

    return a;
  }
    
  BoundaryNode(double xmin, double xmax, double ymin, double ymax):
    QuadNode<T>(xmin,xmax,ymin,ymax) {
  }

  QuadNode<T>* insert(T p) {

    if(!this->IsPointInQuad(p)) {
      throw point_out_of_boundary_exception;
    }
    // find the right quad. delegate to that quad
    if(bl->IsPointInQuad(p)) {
      this->bl = bl->insert(p);
    }
    else if(br->IsPointInQuad(p)) {
      this->br = br->insert(p);
    }
    else if(tl->IsPointInQuad(p)) {
      this->tl = tl->insert(p);
    }
    else if(tr->IsPointInQuad(p)) {
      this->tr = tr->insert(p);
    }
    this->bucket_size = this->bucket_size + 1;
    return this;
  }


  
  struct PtInfo<T> nearestNeighbor(T p, struct PtInfo<T> nn) {
    
    if (p[0] < this->xmin - nn.d || p[0] > this->xmax + nn.d || p[1] < this->ymin - nn.d || p[1] > this->ymax + nn.d) {
      return nn;
    }

    /*
    // redefine nn
    if(!bl->empty()) {
      if (p[0] < bl->xmin - nn.d || p[0] > bl->xmax + nn.d || p[1] < bl->ymin - nn.d || p[1] > bl->ymax + nn.d) {
	//skip this
      } else {	
	double a = (p[0] - bl->xmin)*(p[0] - bl->xmin) + (p[1] - bl->ymin)*(p[1] - bl->ymin);
	double b = (p[0] - bl->xmax)*(p[0] - bl->xmax) + (p[1] - bl->ymin)*(p[1] - bl->ymin);
	double c = (p[0] - bl->xmin)*(p[0] - bl->xmin) + (p[1] - bl->ymax)*(p[1] - bl->ymax);
	double d = (p[0] - bl->xmax)*(p[0] - bl->xmax) + (p[1] - bl->ymax)*(p[1] - bl->ymax);
	nn.d = min(nn.d, max(max(max(a,b),c),d));
      }
    }
      
    if(!br->empty()) {
      if (p[0] < br->xmin - nn.d || p[0] > br->xmax + nn.d || p[1] < br->ymin - nn.d || p[1] > br->ymax + nn.d) {
	//skip this
      } else {
	double a = (p[0] - br->xmin)*(p[0] - br->xmin) + (p[1] - br->ymin)*(p[1] - br->ymin);
	double b = (p[0] - br->xmax)*(p[0] - br->xmax) + (p[1] - br->ymin)*(p[1] - br->ymin);
	double c = (p[0] - br->xmin)*(p[0] - br->xmin) + (p[1] - br->ymax)*(p[1] - br->ymax);
	double d = (p[0] - br->xmax)*(p[0] - br->xmax) + (p[1] - br->ymax)*(p[1] - br->ymax);
	nn.d = min(nn.d, max(max(max(a,b),c),d));
      }
    }
      
    if(!tl->empty()) {
      if (p[0] < tl->xmin - nn.d || p[0] > tl->xmax + nn.d || p[1] < tl->ymin - nn.d || p[1] > tl->ymax + nn.d) {
	//skip this
      } else {
	double a = (p[0] - tl->xmin)*(p[0] - tl->xmin) + (p[1] - tl->ymin)*(p[1] - tl->ymin);
	double b = (p[0] - tl->xmax)*(p[0] - tl->xmax) + (p[1] - tl->ymin)*(p[1] - tl->ymin);
	double c = (p[0] - tl->xmin)*(p[0] - tl->xmin) + (p[1] - tl->ymax)*(p[1] - tl->ymax);
	double d = (p[0] - tl->xmax)*(p[0] - tl->xmax) + (p[1] - tl->ymax)*(p[1] - tl->ymax);
	nn.d = min(nn.d, max(max(max(a,b),c),d));
      }
    }
      
    if(!tr->empty()) {
      if (p[0] < tr->xmin - nn.d || p[0] > tr->xmax + nn.d || p[1] < tr->ymin - nn.d || p[1] > tr->ymax + nn.d) {
	//skip this
      } else {
	double a = (p[0] - tr->xmin)*(p[0] - tr->xmin) + (p[1] - tr->ymin)*(p[1] - tr->ymin);
	double b = (p[0] - tr->xmax)*(p[0] - tr->xmax) + (p[1] - tr->ymin)*(p[1] - tr->ymin);
	double c = (p[0] - tr->xmin)*(p[0] - tr->xmin) + (p[1] - tr->ymax)*(p[1] - tr->ymax);
	double d = (p[0] - tr->xmax)*(p[0] - tr->xmax) + (p[1] - tr->ymax)*(p[1] - tr->ymax);
	nn.d = min(nn.d, max(max(max(a,b),c),d));
      }
    }
    */

    if(!bl->empty()) {
      nn = bl->nearestNeighbor(p, nn);
    }
    if(!br->empty()) {
      nn = br->nearestNeighbor(p, nn);
    }
    if(!tl->empty()) {
      nn = tl->nearestNeighbor(p, nn);
    }
    if(!tr->empty()) {
      nn = tr->nearestNeighbor(p, nn);
    }

    return nn;
  }
};



// DataNode
template <typename T>
class DataNode: public QuadNode<T> {
private:
  int MAX_BUCKET_SIZE;
  vector<T> data;
  
public:

  DataNode(double xmin, double xmax, double ymin, double ymax, int max_bucket_size) : QuadNode<T>(xmin,xmax,ymin,ymax) {
    this->MAX_BUCKET_SIZE = max_bucket_size;
  }

  int bucketsize() {
    return data.size();
  }

  vector<T> getBounds() {
    vector<T> v;
    T a = {this->xmin,this->ymin};
    T b = {this->xmax,this->ymin};
    T c = {this->xmax,this->ymax};
    T d = {this->xmin,this->ymax};
    v.push_back(a);
    v.push_back(b);
    v.push_back(b);
    v.push_back(c);
    v.push_back(c);
    v.push_back(d);
    v.push_back(d);
    v.push_back(a);
    
    return v;
  }

  PtInfo<T> nearestNeighbor(T p, PtInfo<T> nn) {
    if(this->data.size() == 0) {
      throw no_point_in_quad_exception;
    }
    for(auto e: data) {
      double d = dist(p,e);
      if(d < nn.d) {
	nn.d = d;
	nn.data = e;
      }
    }
    return nn;
  }

  PtInfo<T> findNearestPointInQuad(T p) {
    T n = this->nearestNeighbor(p);
    PtInfo<T> nn;
    nn.d = dist(p,n);
    nn.data = n;
    return nn;
  }

  T nearestNeighbor(T p) {
    if(this->data.size() == 0) {
      throw no_point_in_quad_exception;
    }
    T n = this->data[0];
    double b = dist(p,n);
    for(auto e: this->data) {
      double d = dist(p,e);
      if(d < b) {
	n = e;
	b = d;
      }
    }
    return n;
  }

  vector<T> bfs() {
    //cout << this->MAX_BUCKET_SIZE <<",datanode,datasize:" << data.size() << "\n";
    return data;
  }

  QuadNode<T>* insert(T p) {
    data.push_back(p);
    this->bucket_size = this->bucket_size + 1;
    //cout << p << " " << data.size() << "\n";
    
    if(data.size() > this->MAX_BUCKET_SIZE) {
      return this->splitNode();
    }
    return this;
  }

  
  QuadNode<T>* splitNode() {

    BoundaryNode<T>* s = new BoundaryNode<T>(this->xmin, this->xmax, this->ymin, this->ymax);
    
    double mx = (this->xmin + this->xmax)/2;
    double my = (this->ymin + this->ymax)/2;
    
    s->bl = new DataNode<T>(this->xmin, mx, this->ymin, my, this->MAX_BUCKET_SIZE);
    s->br = new DataNode<T>(mx, this->xmax, this->ymin, my, this->MAX_BUCKET_SIZE);
    s->tl = new DataNode<T>(this->xmin, mx, my, this->ymax, this->MAX_BUCKET_SIZE);
    s->tr = new DataNode<T>(mx, this->xmax, my, this->ymax, this->MAX_BUCKET_SIZE);
    
    for (T d: data) {
      if (s->bl->IsPointInQuad(d)) {
	s->bl = s->bl->insert(d);
      }
      else if (s->br->IsPointInQuad(d)) {
	s->br = s->br->insert(d);
      }
      else if (s->tl->IsPointInQuad(d)) {
	s->tl = s->tl->insert(d);
      }
      else if (s->tr->IsPointInQuad(d)) {
	s->tr = s->tr->insert(d);
      }
    }

    data.clear();
    s->bucket_size = this->bucket_size;
    return s;
  }


};


// T will be Point2D<double>.
template <typename T>
class QuadTree {
private:
  QuadNode<T>* root;

public:

  QuadTree(double xmin, double xmax, double ymin, double ymax, int max_bucket_size) {
    this->root = new DataNode<T>(xmin,xmax,ymin,ymax,max_bucket_size);
  }
  
  QuadNode<T>* getRoot() {
    return root;
  }

  void insert(T data) {
    this->root = this->root->insert(data);
  }

  template <typename Iterator>
  void createTree(Iterator begin, Iterator end) {
    auto it = begin;
    while(it != end) {
      this->insert(*it);
      it++;
    }
  }

  vector<T> bfs() {
    return this->root->bfs();
  }

  T nearestNeighbor(T p) {
    return this->root->nearestNeighbor(p);
  }

  vector<T> getBounds() {
    return this->root->getBounds();
  }
};
  
  /*

  // instead of double, we should have a generic type, same as that of T::double/int/float
  QuadNode(double xmin, double xmax, double ymin,  double ymax, int bucket_size, int tile_size) {
    this->xmin = xmin;
    this->xmax = xmax;
    this->ymin = ymin;
    this->ymax = ymax;
    this->bucket_size = bucket_size;
    this->tile_size = tile_size;
  }
  
    
  void setBotLeft(QuadNode<T> *bl)
  {
    this->botleft = bl;
  }
  
  QuadNode<T> *getBotLeft()
  {
    return this->botleft;
  }
  
  void setBotRight(QuadNode<T> *br)
  {
    this->botright = br;
  }
  
  QuadNode<T> *getBotRight()
  {
    return this->botright;
  }
  
  void setTopLeft(QuadNode<T> *tl)
  {
    this->topleft = tl;
  }
  
  QuadNode<T> *getTopLeft()
  {
    return this->topleft;
  }
  
  void setTopRight(QuadNode<T> *tr)
  {
    this->topright = tr;
  }
  
  QuadNode<T> *getTopRight()
  {
    return this->topright;
  }
  
  
  void SetData(T d)
  {
    this->data = d;
  }

  void SetHeight(int h) {
    this->height = h;
  }

  T GetData()
  {
    return this->data;
  }

  
  bool hasBotLeft()
  {
    if(botleft == NULL)
      return false;
    else
      return true;
  }

  bool hasBotRight()
  {
    if(botright == NULL)
      return false;
    else
      return true;
  }

  bool hasTopLeft()
  {
    if(topleft == NULL)
      return false;
    else
      return true;
  }

  bool hasTopRight()
  {
    if(topright == NULL)
      return false;
    else
      return true;
  }
  
  int & GetHeight()
  {
    return this->height;
  }
  
  
}
  */

#endif


