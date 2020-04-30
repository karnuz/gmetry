#include <iostream>
#include "avl.h"
#include <stdlib.h>
#include "time.h"
#include "assert.h"
#include <unordered_set>
#include <vector>
#include <algorithm>

using namespace std;



void test_AVLTree() {

  srand(time(0));
  int N = 100;
  int M = 50;
  vector<int> vecOfRandomNums(N);
  
  int k = 0;
  for(k = 0; k < 100; k++) {

    AVLTree<int> t;
    generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), []()
							     {
							       return rand() % 500;
							     });
    
    unordered_set<int> numset(vecOfRandomNums.begin(), vecOfRandomNums.end());
    vector<int> veccopy(numset.begin(), numset.end());
    
    for(int i:vecOfRandomNums) {
      t.insert(i);
    }

    sort (veccopy.begin(), veccopy.end());
    vector<int> after_insert = t.InOrder();    
    assert(after_insert == veccopy);

    vector<int> deleted;
    int j = 0;
    while(j < N) {
      t.Delete(vecOfRandomNums[j]);
      deleted.push_back(vecOfRandomNums[j]);
      j = j+2;
    }

    vector<int> after_del_expected;
    for(auto k: vecOfRandomNums) {
      if(count(deleted.begin(), deleted.end(), k))
	{
	  continue;
	}
	after_del_expected.push_back(k);
    }

    
    unordered_set<int> numset_del(after_del_expected.begin(), after_del_expected.end());
    vector<int> veccopy_del(numset_del.begin(), numset_del.end());
    sort(veccopy_del.begin(), veccopy_del.end());


    
    vector<int> after_delete = t.InOrder();
    assert(veccopy_del == after_delete);
    
  }
 
}


int main()
{
  test_AVLTree();
  return 0;
}
    //    vector<int> vecOfRandomNums {4, 8, 9, 12, 16, 18, 19, 21, 26, 29, 35, 36, 38, 56, 66, 78, 93, 100, 108, 111, 132, 143, 150, 153, 163, 167, 173, 174, 175, 180, 184, 186, 196, 198, 201, 207, 215, 222, 224, 228, 236, 237, 238, 240, 251, 253, 255, 257, 260, 263, 272, 277, 290, 293, 304, 307, 309, 315, 324, 326, 329, 330, 331, 342, 344, 347, 357, 358, 360, 366, 374, 387, 390, 394, 395, 399, 400, 403, 404, 410, 416, 419, 423, 430, 431, 457, 461, 462, 467, 470, 473, 477, 481, 483, 494};

    //vector<int> vecOfRandomNums {143, 199, 368, 199, 430, 313, 362, 373, 308, 175, 394, 493, 22, 257, 260, 239, 362, 282, 299, 278, 54, 205, 287, 453, 460, 123, 127, 363, 400, 257, 101, 248, 328, 156, 347, 402, 100, 480, 297, 82, 455, 226, 435, 212, 441, 376, 277, 58, 326, 220, 460, 439, 131, 477, 492, 466, 238, 230, 10, 5, 61, 158, 13, 370, 305, 109, 91, 348, 76, 435, 300, 124, 462, 169, 55, 224, 215, 54, 398, 364, 257, 26, 273, 2, 97, 94, 262, 110, 30, 201, 436, 271, 7, 151, 285, 245, 265, 229, 497, 85};

    //    vector<int> vecOfRandomNums {345, 387, 148, 434, 431, 344, 138, 279, 290, 176}
    // vector<int> vecOfRandomNums {475, 33, 222, 480, 75, 384, 363, 185, 393, 138};
    
    //    vector<int> v = t.PreOrder();

    //vector<int>::iterator it = v.begin();
    
    //while(it != v.end()) {
    //  cout << *it << "\n";
    //  it++; 
    //}


    /*
  
  int option, data;

  int x = 4;
  int y = 8;
  int z = 7;
  int w = 9;
  int w2 = 12;
  t.insert(x);
  t.insert(y);
  t.insert(z);
  t.insert(w);
  t.insert(w2);
  
    //t.Delete(7);
    t.Delete(9);

    v = t.PreOrder();
    it = v.begin();
    
    while(it != v.end()) {
      cout << *it << "\n";
      it++; 
    }
    */

    
    /*
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
    */
  
