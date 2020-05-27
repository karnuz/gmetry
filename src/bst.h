// http://code-stream.blogspot.com/2012/03/generic-binary-tree-implementation-in-c.html
// Program: Generic Binary Tree Implementation in C++.
//   Program allows you to insert and delete nodes from Binary Tree.
//   Program also demonstrates In-order, Pre-Order and Post-Order
//   traversal of Binary Tree using recursive and non- recursive algo.
// Author : Kapil Sunil Zad

#ifndef _BST_H_
#define _BST_H_

#include <iostream>
#include <stack>
#include "node.h"

using namespace std;

/*
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

*/

template<class T>
class BinaryTree
{

public:
  Node<T> * root;

    void Inorder(Node<T> * r);
    void Preorder(Node<T> * r);
    void Postorder(Node<T> * r);

public:
    BinaryTree()
    {
        root = NULL;
    }

  bool isEmpty() {
    if(root == NULL) {
      return true;
    }
    return false;
  }

  void setRoot(Node<T> *node) {
    this->root = node;
  }

  Node<T>* getRoot() {
    return this->root;
  }
  
    void Insert(T data)
    {
        Node<T> *node = new Node<T>(data);

        if(root == NULL)
        {
            root = node;
            return;
        }

        Node<T> *ptr = root;

        while(ptr != NULL)
        {
            if(ptr->GetData() == data)
            {
                delete node;
                return;
            }
            else if(ptr->GetData() > data)
            {
                if(!ptr->IsLeft())
                {
                    ptr->SetLeft(node);
                    break;
                }
                ptr = ptr->GetLeft();
            }
            else if(ptr->GetData() < data)
            {
                if(!ptr->IsRight())
                {
                    ptr->SetRight(node);
                    break;
                }
                ptr = ptr->GetRight();
            }
        }
    }




    void Delete(T data)
    {
        if(root == NULL)
        {
            cout<<"Tree is Empty.\n";
            return;
        }

        Node<T> * ptr = root;
        Node<T> *parent = NULL;

        while(ptr)
        {
            if(ptr->GetData() == data)
            {
                break;
            }
            else if(ptr->GetData() > data)
            {
                parent = ptr;
                ptr = ptr->GetLeft();
            }
            else if(ptr->GetData() < data)
            {
                parent = ptr;
                ptr = ptr->GetRight();
            }
        }

        if(ptr == NULL)
        {
            cout<<"Data not found.\n";
            return;
        }

        while(1)
        {
            if(!ptr->IsLeft() && !ptr->IsRight())//case 1
            {
                if(parent == NULL)
                {
                    root = NULL;    
                }
                else
                {
                    if(parent->GetLeft() == ptr)
                        parent->SetLeft(NULL);
                    else
                        parent->SetRight(NULL);
                }

                delete ptr;
                break;
            }

            else if(ptr->IsLeft() && !ptr->IsRight())//case 2.1
            {
                if(parent == NULL)
                {
                    root = ptr->GetLeft();
                    delete ptr;
                    break;
                }
                else
                {
                    if(parent->GetLeft() == ptr)
                    {
                        parent->SetLeft(ptr->GetLeft());
                        delete ptr;
                        break;
                    }
                    else if(parent->GetRight() == ptr)
                    {
                        parent->SetRight(ptr->GetLeft());
                        delete ptr;
                        break;
                    }
                }
            }
            else if(!ptr->IsLeft() && ptr->IsRight())//case 2.2
            {
                if(parent == NULL)
                {
                    root = ptr->GetRight();
                    delete ptr;
                    break;
                }
                else
                {
                    if(parent->GetLeft() == ptr)
                    {
                        parent->SetLeft(ptr->GetRight());
                        delete ptr;
                        break;
                    }
                    else if(parent->GetRight() == ptr)
                    {
                        parent->SetRight(ptr->GetRight());
                        delete ptr;
                        break;
                    }
                }
            }
                
            Node<T> * succ = ptr->GetRight();//case 3
            Node<T> *succparent = ptr;

            while(succ->IsLeft())
            {
                succparent = succ;
                succ = succ->GetLeft();
            }

            T data = ptr->GetData();
            ptr->SetData(succ->GetData());
            succ->SetData(data);

            ptr = succ;
            parent = succparent;
        }
    }
    

  vector<T> InOrder() const// using stack
    {
        stack<Node<T> *> s;

        s.push(NULL);

        Node<T> * ptr = root;

	vector<T> out;
	  
        while(ptr!=NULL)
        {    
            while(ptr != NULL)
            {
                s.push(ptr);
                ptr = ptr->GetLeft();
            }

            ptr = s.top();
            s.pop();

            while(ptr != NULL)
            {
	      out.push_back(ptr->GetData());
		
	      //                cout<<ptr->GetData()<<" ";

                if(ptr->IsRight())
                {
                    ptr = ptr->GetRight();
                    break;
                }

                ptr = s.top();
                s.pop();
            }
         }
	return out;
    }

  vector<T> PreOrder() const //using stack
    {
        stack<Node<T> *> s;
        Node<T> *ptr = root;
	vector<T> out;
	
        s.push(NULL);

        while(ptr != NULL)
        {
	  out.push_back(ptr->GetData());

            if(ptr->IsRight())
                s.push(ptr->GetRight());

            if(ptr->IsLeft())
                ptr = ptr->GetLeft();
            else
            {
                ptr = s.top();
                s.pop();
            }
        }

	return out;
    }


  void Inorder()
  {
    if(root == NULL)
      cout<<"Empty."<<endl;
    else
      Inorder(root);
  }
  
  
  void Preorder()
  {
    if(root == NULL)
      cout<<"Empty."<<endl;
    else
      Preorder(root);
  }
  
  void Postorder()
  {
    if(root == NULL)
      cout<<"Empty."<<endl;
    else
      Postorder(root);
  }
};

template<class T>
void BinaryTree<T>::Inorder(Node<T> *r)
{
    if(r->IsLeft())
        Inorder(r->GetLeft());

    cout<<r->GetData()<<" ";

    if(r->IsRight())
        Inorder(r->GetRight());
}

template<class T>
void BinaryTree<T>::Preorder(Node<T> *r)
{
    cout<<r->GetData()<<" ";

    if(r->IsLeft())
        Preorder(r->GetLeft());

    if(r->IsRight())
        Preorder(r->GetRight());
}

template<class T>
void BinaryTree<T>::Postorder(Node<T> *r)
{
    if(r->IsLeft())
        Postorder(r->GetLeft());

    if(r->IsRight())
        Postorder(r->GetRight());

    cout<<r->GetData()<<" ";
}

#endif


/*
int main()
{
    BinaryTree<int> t;
    int option, data;

    do
    {
        cout<<"\n1. Insert\n";
        cout<<"2. Inorder\n";
        cout<<"3. Inorder (using stack)\n";
        cout<<"4. Preorder\n";
        cout<<"5. Preorder (using stack)\n";
        cout<<"6. PostOrder\n";
        cout<<"7. Delete\n";
        cout<<"8. Exit\n>> ";
        cin>>option;

        switch(option)
        {
        case 1:
            cout<<"Enter data: ";
            cin>>data;
            t.Insert(data);
            break;

        case 2:
            cout<<"Inorder: \n";
            t.Inorder();
            cout<<endl;
            break;
        
        case 3:
            cout<<"Inorder (using stack): \n";
            t.InOrder();
            cout<<endl;
            break;


        case 4:
            cout<<"Preorder: \n";
            t.Preorder();
            cout<<endl;
            break;

        case 5:
            cout<<"Preorder (using stack): \n";
            t.PreOrder();
            cout<<endl;
            break;


        case 6:
            cout<<"Postorder: \n";
            t.Postorder();
            cout<<endl;
            break;

        case 7:
            cout<<"Enter data to delete: ";
            cin>>data;
            t.Delete(data);
            break;

        case 8:
            break;

        default:
            cout<<"Invalid option.\n";
            break;
        }    
    
    }while(option != 8);

    return 0;
}

*/
