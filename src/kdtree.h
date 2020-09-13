/*
  We implement a KD Tree here. 
 */

#ifndef _KDTREE_H_
#define _KDTREE_H_


#include "node.h"
#include "bst.h"
#include "math.h"

using namespace std;


/*
This class represents a KDTree. We implement a nearest neighbor search algorithm
in this KD tree.
 */
template <class T>
class KDTree: public BinaryTree<T>
{

public:

  KDTree() { }

  /*
    a callable struct we use to sort the points in specified dimension.
   */

  struct Compare {
  public:
    Compare(int dim_idx) { this->dim_idx = dim_idx; }
    bool operator () (T pt_a, T pt_b) { return (pt_a[dim_idx] < pt_b[dim_idx])? true:false; }
  private:
    int dim_idx;
  };


  /*
    This method creats a KD Tree and returns root node of the tree. Here we take a 
    batch of points and not sequentially.
    @params begin : iterator pointing to the begining of points to be inserted
    @params end : iterator pointing to the end of the points to be inserted
    @params dim : dimension of points.
    @params n : numer of points
    @level : level of node that helps in deciding whether tree will be partitioned 
             horizontally or vertically
   */
  template <typename Iterator>
  Node<T>* createTree(Iterator begin, Iterator end, int dim, int n, int level=0) {

    if (n == 0) {
      return NULL;
    }
    
    int d_idx = level%dim;
    Compare comp(d_idx);

    std::sort(begin, end, comp);

    int median_idx = n/2;
    Iterator median = begin + median_idx;

    Node<T> *newnode = new Node<T>(*(median));

    if (this->isEmpty()) {
      this->setRoot(newnode);
    }
    
    if (begin+1 == end) {
      return newnode;
    }

    
    level = level + 1;
    
    Node<T> *leftnode = createTree(begin, median, dim, median_idx , level);
    Node<T> *rightnode = createTree(median + 1, end, dim, (n-1)/2 , level);

    newnode->SetLeft(leftnode);
    newnode->SetRight(rightnode);
    
    return newnode;

  }

  /*
    This method is used to insert one point into the KD tree.
    @params data : point to be inserted
    @params dim : dimension of point
   */
  Node<T>* insert(T data, int dim) {
    Node<T>* newnode = new Node<T>(data);

    if (this->isEmpty()) {
      this->setRoot(newnode);
      return this->getRoot();
    }
    
    
    Node<T> * ptr = this->getRoot();
    int level = 0;
    

    while(true) {
      T ndata = ptr->GetData();
      int split_idx = level%dim;
      if (data[split_idx] <= ndata[split_idx]) {
	if(ptr->IsLeft()) {
	  ptr = ptr->GetLeft();
	} else {
	  ptr->SetLeft(newnode);
	  break;
	}
      }
      else if (data[split_idx] > ndata[split_idx]) {
	if(ptr->IsRight()) {
	  ptr = ptr->GetRight();
	}
	else {
	  ptr->SetRight(newnode);
	  break;
	}
      }
      level += 1;
    }
    return ptr;
  }


  /*
    First and most inefficient algorithm to comptute nearest neighbor.
    It starts searching from the root node itself instead of first going 
    to the 'most probable' leaf node.
    @params tree : tree to be searched
    @params data : point whose nearest neighbor we are looking for
    @params dim : dimension of points
    @returns : point which is nearest neighbor
   */
  T nearestNeighbor(KDTree<T> *tree ,T data, int dim) {
    
    if(tree->isEmpty()) {
      throw std::invalid_argument("tree is empty\n");;
    }
    
    Node<T>* ptr = tree->getRoot();
    Node<T>* bestNode = ptr;
    double bestDist = (double) dist(data, bestNode->GetData());
    int level = 0;
    
    stack<tuple<Node<T> *, int>> st;
    st.push(tuple(ptr,level));

    int nnv = 0;
    while(!st.empty()) {

      nnv += 1;
      
      auto t = st.top();
      st.pop();
      ptr = get<0>(t);
      level = get<1>(t);
      
      T ndata = ptr->GetData();
      double d = (double) dist(data, ndata);
      //cout << d <<"\n";
      if (d < bestDist) {
	bestDist = d;
	bestNode = ptr;
      }
      int split_idx = level%dim;

      // point on right/up/greater side of split index
      if (data[split_idx] >= ndata[split_idx]) {
	if(abs(data[split_idx] - ndata[split_idx]) < bestDist && ptr->IsLeft()) {
	  st.push(tuple(ptr->GetLeft(),level+1));
	}
	if(ptr->IsRight()) {
	  st.push(tuple(ptr->GetRight(), level + 1));
	}
      }
      else {
	if(ptr->IsLeft()) {
	  st.push(tuple(ptr->GetLeft(),level+1));
	}
	if(abs(data[split_idx] - ndata[split_idx]) < bestDist && ptr->IsRight()) {
	  st.push(tuple(ptr->GetRight(),level+1));
	}
      }
    }
    cout << "num_nodes_visited:" << nnv << "\n";
    return bestNode->GetData();
  }


  /*
    Second algorithm to comptute nearest neighbor.
    It starts searching from the leaf which is the 'most probable' nearest
    point to the query point. However does not update the minimum distance
    after searching one branch and starts searching other branch.
    @params tree : tree to be searched
    @params data : point whose nearest neighbor we are looking for
    @params dim : dimension of points
    @returns : point which is nearest neighbor
   */

  T nearestNeighbor2(KDTree<T> *tree ,T data, int dim) {
    
    if(tree->isEmpty()) {
      throw std::invalid_argument("tree is empty\n");
    }
    
    Node<T> * ptr = tree->getRoot();
    int level = 0;
    
    //cout << bestNode->GetData();
    vector<tuple<Node<T> *, int>> st;
    st.push_back(tuple(ptr,level));


    while(true) {
      T ndata = ptr->GetData();
      int split_idx = level%dim;
      if (data[split_idx] <= ndata[split_idx]) {
	if(ptr->IsLeft()) {
	  level += 1;
	  ptr = ptr->GetLeft();
	  st.push_back(tuple(ptr,level));
	} else {
	  break;
	}
      }
      else {
	if(ptr->IsRight()) {
	  level += 1;
	  ptr = ptr->GetRight();
	  st.push_back(tuple(ptr,level));
	}
	else {
	  break;
	}
      }
    }

    
    int N = st.size();
    Node<T>* bestNode = ptr;
    double bestDist = (double) dist(data, bestNode->GetData());
    int k;

    int nnv = 0;
    //    cout << "stack:\n";
    
    //cout << "best distance : " << bestDist << "\n";
    while(!st.empty()) {
      nnv += 1;

      for(auto e: st) {
	Node<T>* ee = get<0>(e);
	//	cout << (*ee).GetData() << " ";
      }
      //cout << "\n";

      k = st.size();
      auto t = st[st.size()-1];
      st.erase(st.end()-1);

      ptr = get<0>(t);
      level = get<1>(t);
      
      T ndata = ptr->GetData();
      double d = (double) dist(data, ndata);
      if (d < bestDist) {
	bestDist = d;
	bestNode = ptr;
      }
      //cout << bestNode->GetData() << " :best node\n";
      int split_idx = level%dim;

      // point on right/up/greater side of split index
      if (data[split_idx] <= ndata[split_idx]) {
	if(ptr->IsLeft() && k > N) {
	  st.push_back(tuple(ptr->GetLeft(),level+1));
	}
	if(abs(data[split_idx] - ndata[split_idx]) < bestDist && ptr->IsRight()) {
	  st.push_back(tuple(ptr->GetRight(),level+1));
	}	
      }
      else {
	if(ptr->IsRight() && k > N) {
	  st.push_back(tuple(ptr->GetRight(), level + 1));
	}
	if(abs(data[split_idx] - ndata[split_idx]) < bestDist && ptr->IsLeft()) {
	  st.push_back(tuple(ptr->GetLeft(),level+1));
	}
      }
      if(k <= N) {
	N = N-1;
      }
    }
    //   cout << "num_nodes_visited_second:" << nnv << "\n";
    return bestNode->GetData();

  }



  /*
    Second algorithm to comptute nearest neighbor.
    It starts searching from the leaf which is the 'most probable' nearest
    point to the query point. This updates the minimum distance
    after searching one branch and then searches other branch.
    @params tree : tree to be searched
    @params data : point whose nearest neighbor we are looking for
    @params dim : dimension of points
    @returns : point which is nearest neighbor
   */

  T nearestNeighbor3(KDTree<T> *tree ,T data, int dim) {
    
    if(tree->isEmpty()) {
      throw std::invalid_argument("tree is empty\n");
    }
    
    Node<T> * ptr = tree->getRoot();
    int level = 0;
    

    // the third value represents if the node was added unconditionally
    
    vector<tuple<Node<T> *, int, int>> st;
    st.push_back(tuple(ptr,level,0));


    while(true) {
      T ndata = ptr->GetData();
      int split_idx = level%dim;
      if (data[split_idx] <= ndata[split_idx]) {
	if(ptr->IsLeft()) {
	  level += 1;
	  ptr = ptr->GetLeft();
	  st.push_back(tuple(ptr,level,0));
	} else {
	  break;
	}
      }
      else {
	if(ptr->IsRight()) {
	  level += 1;
	  ptr = ptr->GetRight();
	  st.push_back(tuple(ptr,level,0));
	}
	else {
	  break;
	}
      }
    }

    
    int N = st.size();
    Node<T>* bestNode = ptr;
    double bestDist = (double) dist(data, bestNode->GetData());
    int k;

    int nnv = 0;
    
    while(!st.empty()) {
   
      for(auto e: st) {
	Node<T>* ee = get<0>(e);
      }

      k = st.size();
      auto t = st[st.size()-1];
      st.erase(st.end()-1);

      ptr = get<0>(t);
      level = get<1>(t);
      int cond = get<2>(t);
      int split_idx = level%dim;
      T ndata = ptr->GetData();
      double d = (double) dist(data, ndata);

      // if it was a normal push
      if(cond == 0) {

	if (d < bestDist) {
	  bestDist = d;
	  bestNode = ptr;
	}
	
      // point on right/up/greater side of split index
	if (data[split_idx] <= ndata[split_idx]) {
	  if(abs(data[split_idx] - ndata[split_idx]) < bestDist && ptr->IsRight() && cond == 0) {
	    st.push_back(tuple(ptr,level,1));
	  }
	  if(ptr->IsLeft() && k > N && cond == 0) {
	    st.push_back(tuple(ptr->GetLeft(),level+1,0));
	  }
	}
	else {
	  if(abs(data[split_idx] - ndata[split_idx]) < bestDist && ptr->IsLeft() && cond == 0) {
	    st.push_back(tuple(ptr,level,1));
	  }
	  if(ptr->IsRight() && k > N && cond == 0) {
	    st.push_back(tuple(ptr->GetRight(), level+1,0));
	  }
	}
	if(k <= N) {
	  N = N-1;
	}
      } else {
	if (data[split_idx] <= ndata[split_idx]) {
	  if(abs(data[split_idx] - ndata[split_idx]) < bestDist && ptr->IsRight()) {
	    st.push_back(tuple(ptr->GetRight(),level+1,0));
	  }
	}
	else {
	  if(abs(data[split_idx] - ndata[split_idx]) < bestDist && ptr->IsLeft()) {
	    st.push_back(tuple(ptr->GetLeft(),level+1,0));
	  }
	}
	assert(k>N);
      }
    }
    return bestNode->GetData();

  }  

};

#endif


