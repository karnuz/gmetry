#ifndef _AVL_H_
#define _AVL_H_

#include "bst.h"
#include <vector>

using namespace std;

template <class T>
class AVLNode : public Node<T>
{

public:
    int height = 0;


    void SetHeight(int &h)
    {
        height = h;
    }

    int & GetHeight()
    {
        return height;
    }


    void UpdateHeight()
    {

      if (this->IsLeft() && this->IsRight()) {
	int lh = this->left->GetHeight();
	int rh = this->right->GetHeight();
	this->SetHeight(1+max(lh,rh));
      }
      else if (this->IsLeft()) {
	int lh = this->left->GetHeight();
	this->SetHeight(1+lh);
      } else if (this->IsRight()) {
	int rh = this->right->GetHeight();
	this->SetHeight(1+rh);
      } else {
	this->SetHeight(0);
      }
    }

    int GetBalance()
    {
      if (this->IsLeft() && this->IsRight()) {
	int lh = this->left->GetHeight();
	int rh = this->right->GetHeight();
	return rh - lh;
      }
      else if (this->IsLeft()) {
	int lh = this->left->GetHeight();
	return -1-lh;
      } else if (this->IsRight()) {
	int rh = this->right->GetHeight();
	return 1+rh;
      } else {
	return 0;
      }

    }
};

template <class T>
class AVLTree: public BinaryTree<T>
{

  //protected:
  // Node<T> *root;

public:

  AVLTree()
  {
    this->root = NULL;
  }

    Node<T> * leftRotate(Node<T> *node)
  {
    
    Node<T> *right_child = node->GetRight();
    
    node->SetRight(right_child->GetLeft());
    right_child->SetLeft(node);
    
    node->UpdateHeight();
    right_child->UpdateHeight();

    return right_child;
    
  }
  
  Node<T> * rightRotate(Node<T> *node)
  {      
    
    Node<T> *left_child = node->GetLeft();
    
    node->SetLeft(left_child->GetRight());
    left_child->SetRight(node);
    
    node->UpdateHeight();
    left_child->UpdateHeight();
    
    return left_child;
  }

  void insert(T data)
  {
    Node<T> *node = new Node<T>(data);
    
    if (this->root == NULL)
      {
	this->root = node;
	return;
      }
    
    // insert part
    Node<T> *ptr = this->root;
    vector<Node<T> *> path;
    
    while (ptr != NULL)
      {
	T cdata = ptr-> GetData();
	// data already exists, nothing to do
	if (cdata == data)
	  {
	    delete node;
	    return;
	  }
	
	path.push_back(ptr);
	if (cdata > data)
	  {
	    if (ptr->GetLeft() == NULL)
	      {
		ptr->SetLeft(node);
		break;
	      }
	    ptr = ptr->GetLeft();
	  }
	else if (cdata < data)
	  {
	    if (ptr->GetRight() == NULL)
	      {
		ptr->SetRight(node);
		break;
	      }
	    ptr = ptr->GetRight();
	  }
      }
    
    // rebalancing part
    this->root = this->rebalancePath(path, false);
  }
  
  // path is iterated from back and rebalanced. the flag FULL_PATH decides whether to exit on first rebalance(as insert requires) or go through the full path (as delete requires)
  Node<T> * rebalancePath(vector<Node<T> *> path, bool FULL_PATH) {

        typename vector<Node<T>*>::iterator it1 = path.end();
        while (it1 != path.begin())
        {
            --it1;
            Node<T> *curr = (*it1);
	    T cdata = curr->GetData();
            Node<T> *left_child = curr->GetLeft();
            Node<T> *right_child = curr->GetRight();
	    curr->UpdateHeight();
	    int balance = curr->GetBalance();
            
            // if node is balanced continue to next parent node in the path.
            if (abs(balance) <= 1)
            {
                continue;
            }
            // if the current node is unbalanced. (cannot be a leaf node)
            else
            {
	      // curr_next might be end of path. don't change it to null in that case. in the following evaluations it is compared if left_child == curr_next. if curr_next is path end and we change it to null, and left child is null, that expression will evaluate to true, which we don't want.
	      Node<T> *curr_next = *(it1 + 1);
	      
	      
	      Node<T> *curr_parent;
	      if(it1 == path.begin()) {
		curr_parent = NULL;
	      } else {
		curr_parent = *(it1-1);
	      }

	      
	      // left left
	      // note that short circuit evaluation is used here.
	      if (balance < 1 && left_child->GetBalance() <= 0)
		//	      if (left_child == curr_next && data < left_child->GetData())
		{
		  // note that pointer values of left and right child of curr and curr_parent have changed
		  Node<T> * h = this->rightRotate(curr);
		  if (curr_parent == NULL) {
		     return h;
		  } else {
		    if(curr_parent->GetLeft() == curr ) {
		      curr_parent->SetLeft(h);
		    } else {
		      curr_parent->SetRight(h);
		    }
		    if(!FULL_PATH) {
		      return path[0];
		    }
		  }
		}
	      // left right
	      else if (balance < 1 && left_child->GetBalance() > 0)
		//	      else if (left_child == curr_next && data > left_child->GetData())
		{
		  curr->SetLeft(leftRotate(left_child));
		  Node<T> *h = this->rightRotate(curr);
		  if (curr_parent == NULL) {
		    return h;
		  } else {
		    if(curr_parent->GetLeft() == curr ) {
		      curr_parent->SetLeft(h);
		    } else {
		      curr_parent->SetRight(h);
		    }
		    if(!FULL_PATH) {
		      return path[0];
		    }
		  }
		}
	      //right left
	      else if (balance > 1 && right_child->GetBalance() < 0)
		//	      else if (right_child == curr_next && data < right_child->GetData())
		{
		  curr->SetRight(rightRotate(right_child));
		  Node<T> *h = this->leftRotate(curr);
		  if (curr_parent == NULL) {
		    return h;
		  } else {
		    if(curr_parent->GetLeft() == curr ) {
		      curr_parent->SetLeft(h);
		    } else {
		      curr_parent->SetRight(h);
		    }
		    if(!FULL_PATH) {
		      return path[0];
		    }
		  }
		}
	      //right right
	      else if (balance > 1 && right_child->GetBalance() >= 0)
		//	      else if (right_child == curr_next && data > right_child->GetData())
		{
		  Node<T> *h = this->leftRotate(curr);
		  if (curr_parent == NULL) {
		    return h;
		  } else {
		    if(curr_parent->GetLeft() == curr ) {
		      curr_parent->SetLeft(h);
		    } else {
		      curr_parent->SetRight(h);
		    }
		    if(!FULL_PATH) {
		      return path[0];
		    }
		  }
		}
	    }
	}
	return path[0];
  }


  vector<Node<T>*> findpath(T data, Node<T> *rt) {
    
    vector<Node<T> *> path;

    if (rt == NULL) {
      return path;
    }

    // find path
    Node<T> *ptr = rt;

    while(ptr != NULL) {
      T cdata = ptr->GetData();
      path.push_back(ptr);
      if (cdata == data) {
	return path;
      } else if (cdata > data) {
	ptr = ptr->GetLeft();
      } else {
	ptr = ptr->GetRight();
      }
    }

    return {};
  }

  vector<Node<T>*> findpath(T data) {
    return this->findpath(data,this->root);
  }


  void Delete(T data)
  {

    if(this->root == NULL) {
      return;
    }
    
    vector<Node<T>*> path = this->findpath(data);

    if(path.empty()) {
      return;
    }

    
    Node<T> *ptr = path.back();
    path.pop_back();
    int pathsize = path.size();
    Node<T> *parent;
    if (pathsize == 0) {
      parent = NULL;
    } else {
      parent = path[pathsize-1];
    }


    while(true) {

      if(ptr->GetLeft() == NULL && ptr->GetRight() == NULL) {
	if (parent == NULL) {
	  return;
	} else {
	  if(parent->GetLeft() == ptr) {
	    parent->SetLeft(NULL);
	    break;
	  } else {
	    parent->SetRight(NULL);
	    break;
	  }
	}
      }
      else if (ptr->GetRight() == NULL) {
	if (parent == NULL) {
	  this->root = ptr->GetLeft();
	  return;
	} else {
	  if(parent->GetLeft() == ptr) {
	    parent->SetLeft(ptr->GetLeft());
	    break;
	  } else {
	    parent->SetRight(ptr->GetLeft());
	    break;
	  }
	}
      }
      else if (ptr->GetLeft() == NULL) {
	if (parent == NULL) {
	  this->root = ptr->GetRight();
	} else {
	  if(parent->GetLeft() == ptr) {
	    parent->SetLeft(ptr->GetRight());
	    break;
	  } else {
	    parent->SetRight(ptr->GetRight());
	    break;
	  }
	}
      } else {
	Node<T> * succ = ptr->GetRight();
	Node<T> * succ_parent = ptr;
	path.push_back(succ_parent);
	while(succ->GetLeft() != NULL) {
	  succ_parent = succ;
	  succ = succ->GetLeft();
	  path.push_back(succ_parent);
	}
	ptr->SetData(succ->GetData());

	parent = succ_parent;
	ptr = succ;
      }

      
    }

    // update heights
      this->root = this->rebalancePath(path, true); 

  }  
  
};

template<class T>
ostream& operator<<(ostream& os, const AVLTree<T>& t)
{
  vector<int> v = t.InOrder();
  
  vector<int>::iterator it = v.begin();
  
  while(it != v.end()) {
    os << *it << " ";
    it++; 
  }
  return os;
}



#endif
