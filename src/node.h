#ifndef _NODE_H_
#define _NODE_H_

#include <iostream>
#include <stack>
#include <node.h>

using namespace std;

template <class T>
class Node
{
    Node<T> *left;
    T data;
    int height;
    Node<T> *right;

public:
    Node(T d)
    {
        data = d;
        left = right = NULL;
	height = 0;
    }

    void SetLeft(Node<T> *l)
    {
        this->left = l;
    }

    Node<T> *GetLeft()
    {
        return this->left;
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

    void SetRight(Node<T> *r)
    {
        this->right = r;
    }

    Node<T> *GetRight()
    {
        return right;
    }

    bool IsLeft()
    {
        if(left == NULL)
            return false;
        else
            return true;
    }

    bool IsRight()
    {
        if(this->right == NULL)
            return false;

        return true;
    }

        void SetHeight(int &h)
    {
        this->height = h;
    }

    int & GetHeight()
    {
        return this->height;
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

#endif
