#ifndef _KDTREE_H_
#define _KDTREE_H_


#include "node.h"
#include "bst.h"
#include "math.h"

using namespace std;

template <class T>
class KDTree: public BinaryTree<T>
{

public:

  KDTree() { }


  struct Compare {
  public:
    Compare(int dim_idx) { this->dim_idx = dim_idx; }
    bool operator () (T pt_a, T pt_b) { return (pt_b[dim_idx] > pt_a[dim_idx])? true:false; }
  private:
    int dim_idx;
  };
  
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

    cout << "median" << newnode->GetData().x << ":" << newnode->GetData().y << "\n";

    if (this->isEmpty()) {
      cout << "what happened \n";
      this->setRoot(newnode);
    }
    
    if (begin == end) {
      return newnode;
    }

    
    level = level +1;
    
    Node<T> *leftnode = createTree(begin, median, dim, median_idx , level);
    Node<T> *rightnode = createTree(median + 1, end, dim, (n-1)/2 , level);

    newnode->SetLeft(leftnode);
    newnode->SetRight(rightnode);
    
    return newnode;

  }


  T nearestNeighbor(KDTree<T> *tree ,T data, int dim) {

    
    if(tree->isEmpty()) {
      throw std::invalid_argument("tree is empty\n");;
    }
    
    Node<T> * ptr = tree->getRoot();
    Node<T>* bestNode = ptr;
    double bestDist = (double) distSq(data, bestNode->GetData());
    int level = 0;
    
    //cout << bestNode->GetData();
    stack<tuple<Node<T> *, int>> st;
    st.push(tuple(ptr,level));

    
    //cout << "best distance : " << bestDist << "\n";
    while(!st.empty()) {

      auto t = st.top();
      st.pop();
      ptr = get<0>(t);
      level = get<1>(t);
      
      T ndata = ptr->GetData();
      double d = (double) distSq(data, ndata);
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
    return bestNode->GetData();

  }
  
  
};

#endif


