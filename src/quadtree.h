#ifndef _QUADTREE_H_
#define _QUADTREE_H_


#include "node.h"
#include "math.h"
#include "exceptions.h"
#include "utils.h"
#include "float.h"

using namespace std;


/*
This struct is returned by the nearest neighbor search
It contains the nearest point and the distance to it.
 */
template <typename T>
struct PtInfo {
  T data;
  double d;
};

/*
  Represents node of a quad tree. It is a pure virtual class.
 */
template <typename T>
class QuadNode {
public:
  double xmin, xmax, ymin, ymax;
  int bucket_size = 0;

  
public:

  QuadNode() {
  }

  /*
    This constructor constructs a node that represents the region
    as given my input parameters
    @params xmin : minmum bound of node in x direction
    @params xmax : maximum bound of node in x direction
    @params ymin : minmum bound of node in y direction
    @params ymax : maximum bound of node in y direction
   */
  QuadNode(double xmin, double xmax, double ymin, double ymax) {
    this->xmin = xmin;
    this->xmax = xmax;
    this->ymin = ymin;
    this->ymax = ymax;
  }


  /*
    The method inserts a data point into the sub tree representee
    by this node
    @params data : point to be inserted
  */
  virtual QuadNode<T>* insert(T data) = 0;

  /*
    The method returns a flag wheter the node is empty or not.
    A node is empty if the sub tree represented by it is empty
    @returns : bool representing if node is empty
  */
  virtual bool empty() {
    return this->bucket_size == 0;
  }
  

  /*
    @returns returns the number of points in the subtree represented by this node
  */
  virtual int bucketsize() = 0;

  /*
    This method returns a bool telling if a point is within region specified by this node
  */
  bool IsPointInQuad(T data) {
    if (data[0] <= xmax && data[0] >= xmin && data[1] <= ymax && data[1] >= ymin) {
      return true;
    }
    return false;
  }

  /*
    The method returns all the points in the subtree of this node as a vector
  */
  virtual vector<T> bfs() = 0;

  /*
    The method returns the nearest neighbor to the parameter point.
    @params p : point whose nearest neighbor is to be searched
  */
  virtual T nearestNeighbor(T p) = 0;

  /*
    The method returns the nearest neighbor and distance to the parameter point.
    @params p : point whose nearest neighbor is to be searched
    @params nn : point currently the nearest neighbor
   */
  virtual struct PtInfo<T> nearestNeighbor(T p, struct PtInfo<T> nn) = 0;

  /*
    This method returns the nearest neighbor to a point only if the
    query point is in the quad represented by this node.
    @params p : query point
  */
  virtual PtInfo<T> findNearestPointInQuad(T p) = 0;

  /*
    returns the bound of the quad represented by this node.
   */
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

  BoundaryNode(double xmin, double xmax, double ymin, double ymax):
    QuadNode<T>(xmin,xmax,ymin,ymax) {
  }

  /*
    @returns returns the number of points in the subtree represented by this node
  */
  int bucketsize() {
    return this->bucket_size;
  }

  
  /*
    returns the bound of the quad represented by this node.
  */
  vector<T> getBounds() {
    vector<T> a = bl->getBounds();
    vector<T> b = br->getBounds();
    vector<T> c = tl->getBounds();
    vector<T> d = tr->getBounds();
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



  /*
    The method returns the nearest neighbor to the parameter point.
    @params p : point whose nearest neighbor is to be searched
  */
  T nearestNeighbor(T p) {
    if(this->empty()) {
      throw no_point_in_quad_exception();
    }
    T q;
    struct PtInfo<T> nn;
    nn = this->findNearestPointInQuad(p);
    nn = this->nearestNeighbor(p, nn);
    return nn.data;
  }


  /*
    This method returns the nearest neighbor to a point only if the
    query point is in the quad represented by this node.
    @params p : query point
  */
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
      vector<T> v = bfs();
      PtInfo<T> nn;
      nn.data = v[0];
      nn.d = dist(p, v[0]);
      for(auto e: v) {
	double d = dist(p,e);
	if(d < nn.d) {
	  nn.d = d;
	  nn.data = e;
	}
      }
      return nn;
    }
  }

  /*
    The method returns all the points in the subtree of this node as a vector
  */
  vector<T> bfs() {
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
    
  /*
    The method inserts a data point into the sub tree representee
    by this node
    @params data : point to be inserted
  */
  QuadNode<T>* insert(T p) {

    if(!this->IsPointInQuad(p)) {
      throw point_out_of_boundary_exception();
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


  /*
    The method returns the nearest neighbor and distance to the parameter point.
    @params p : point whose nearest neighbor is to be searched
    @params nn : point currently the nearest neighbor
  */
  struct PtInfo<T> nearestNeighbor(T p, struct PtInfo<T> nn) {
    
    if (p[0] < this->xmin - nn.d || p[0] > this->xmax + nn.d || p[1] < this->ymin - nn.d || p[1] > this->ymax + nn.d) {
      return nn;
    }

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

  /*
    @returns returns the number of points in the subtree represented by this node
  */
  int bucketsize() {
    return data.size();
  }

  
  /*
    returns the bound of the quad represented by this node.
  */  
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


  /*
    The method returns the nearest neighbor and distance to the parameter point.
    @params p : point whose nearest neighbor is to be searched
    @params nn : point currently the nearest neighbor
  */
  PtInfo<T> nearestNeighbor(T p, PtInfo<T> nn) {
    if(this->data.size() == 0) {
      throw no_point_in_quad_exception();
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

  

  /*
    This method returns the nearest neighbor to a point only if the
    query point is in the quad represented by this node.
    @params p : query point
  */
  PtInfo<T> findNearestPointInQuad(T p) {
    T n = this->nearestNeighbor(p);
    PtInfo<T> nn;
    nn.d = dist(p,n);
    nn.data = n;
    return nn;
  }

  /*
    The method returns the nearest neighbor to the parameter point.
    @params p : point whose nearest neighbor is to be searched
  */
  T nearestNeighbor(T p) {
    if(this->data.size() == 0) {
      throw no_point_in_quad_exception();
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

  /*
    The method returns all the points in the subtree of this node as a vector
  */
  vector<T> bfs() {
    return data;
  }


  /*
    The method inserts a data point into the sub tree representee
    by this node
    @params data : point to be inserted
  */

  QuadNode<T>* insert(T p) {
    data.push_back(p);
    this->bucket_size = this->bucket_size + 1;
    //cout << p << " " << data.size() << "\n";
    
    if(data.size() > this->MAX_BUCKET_SIZE) {
      return this->splitNode();
    }
    return this;
  }

  /*
    This method splits the node in a direction decided by the level/depth
    of this node in tree. It returns a group node with children containing 
    the points previously contained in this node
    @returns : new group node with children containing 
               the points previously contained in this node
   */
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

/*
The class represents a Quad Tree. Only the leaf nodes store
points while group nodes store the dividing plane.
 */
template <typename T>
class QuadTree {
private:
  QuadNode<T>* root;

public:

  QuadTree(double xmin, double xmax, double ymin, double ymax, int max_bucket_size) {
    this->root = new DataNode<T>(xmin,xmax,ymin,ymax,max_bucket_size);
  }

  /*
    returns the root of this tree
   */
  QuadNode<T>* getRoot() {
    return root;
  }

  /*
    The method inserts one point into the tree by calling 
    insert method of root node
  */
  void insert(T data) {
    this->root = this->root->insert(data);
  }

  /*
    This method creates a tree by inserting the points sequentially into
    the root of this tree;
    @params begin : iterator to begining location of points to be inserted
    @params end : iterator to end location of points to be inserted
  */
  
  template <typename Iterator>
  void createTree(Iterator begin, Iterator end) {
    auto it = begin;
    while(it != end) {
      this->insert(*it);
      it++;
    }
  }

  /*
    The method returns all the points in the tree as a vector
  */
  vector<T> bfs() {
    return this->root->bfs();
  }

  /*
    The method returns the nearest neighbor to the parameter point.
    @params p : point whose nearest neighbor is to be searched
  */
  T nearestNeighbor(T p) {
    return this->root->nearestNeighbor(p);
  }

  /*
    returns the bound of this tree.
   */
  vector<T> getBounds() {
    return this->root->getBounds();
  }
};
  

#endif


