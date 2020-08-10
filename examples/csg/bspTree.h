#ifndef BSPTREE_H
#define BSPTREE_H


#include <iostream>
#include <array>
#include <unistd.h>
#include <queue>
#include <math.h>
#include "exceptions.h"

template <class VertexType>
class BSPTree {
  
private:
  int pp = 0;
  
  typedef struct Node {
    std::array<float,4> plane; // the plane that intersects the space
    std::vector<unsigned int> triangles; // the triangles that are on this plane
    Node* behind = NULL; // all that is behind the plane (relative to normal of plane)
    Node* infront = NULL;// all that is in front of the plane

    Node(std::array<float,4> &pl) : plane(pl)
    {
    }

    Node() {
      plane = std::array<float,4>{{0,0,0,0}};
    }
    
    Node* addTriangle(std::array<unsigned, 3> triangle_indices) {
      triangles.push_back(triangle_indices[0]);
      triangles.push_back(triangle_indices[1]);
      triangles.push_back(triangle_indices[2]);
      return this;
    }

    vector<unsigned int> getIndices() {
      std::vector<unsigned int> indices = triangles;
      std::vector<unsigned int> behindindices;
      std::vector<unsigned int> infrontindices;
      if(behind != NULL) {
	behindindices = behind->getIndices();
	indices.insert(indices.end(),behindindices.begin(),behindindices.end());
      }
      if(infront != NULL) {
	infrontindices = infront->getIndices();
	indices.insert(indices.end(),infrontindices.begin(),infrontindices.end());
      }
      return indices;
    }

    Node* transform(glm::mat4 transform) {
      glm::mat4 inverse = glm::inverse(transform);
      glm::mat4 inversetranspose = glm::transpose(inverse);
      glm::vec4 pl = {this->plane[0], this->plane[1], this->plane[2], this->plane[3]};
      glm::vec4 newpl = inversetranspose*pl;
      std::array<float, 4> newplane = {newpl[0], newpl[1], newpl[2], newpl[3]};
      
      Node *newnode = new Node(newplane);
      newnode->triangles = this->triangles;
      newnode->behind = this->behind;
      newnode->front = this->front;
    }

    Node* deeptransform(glm::mat4 transform) {
      glm::mat4 inverse = glm::inverse(transform);
      glm::mat4 inversetranspose = glm::transpose(inverse);
      glm::vec4 pl = {this->plane[0], this->plane[1], this->plane[2], this->plane[3]};
      glm::vec4 newpl = inversetranspose*pl;
      std::array<float, 4> newplane = {newpl[0], newpl[1], newpl[2], newpl[3]};

      Node *newnode = new Node(newplane);
      newnode->triangles = this->triangles;
      if(this->behind != NULL) {
	newnode->behind = this->behind->deeptransform(transform);
      }
      if(this->infront != NULL) {
	newnode->infront = this->infront->deeptransform(transform);
      }
      return newnode;
    }

    

  } Node;
  
  Node* root = NULL;
  vector<VertexType> vertices;
  vector<unsigned int> polygon_indices;  
  
public:

  util::PolygonMesh<VertexType>* getMesh() {
    util::PolygonMesh<VertexType>* mesh = new util::PolygonMesh<VertexType>();
    mesh->setVertexData(vertices);
    cout << root << "\n"; 
    vector<unsigned int> indices = root->getIndices();
    cout << "Vector size After:" << vertices.size() << "\n";
    cout << "Indices size After:" << polygon_indices.size() << "\n";

    mesh->setPrimitives(indices);
    return mesh;
  }


  BSPTree() {
  }

  /*
  BSPTree(util::PolygonMesh<VertexType>& mesh) {
    cout << root << "   this is root\n";
    vertices = mesh.getVertexAttributes();
    polygon_indices = mesh.getPrimitives();
    int k = 0;

    cout << polygon_indices.size() << " :pol\n";
    //return;
    
    while(k < polygon_indices.size()) {
      std::array<unsigned int, 3> triangle_indices = {polygon_indices[k],polygon_indices[k+1],polygon_indices[k+2]};
      k += 3;
      addTriangle(triangle_indices);
      //usleep(100000);
      //cout << k << "/" << polygon_indices.size() << " :k\n";
      //pp = 0;
    }
  }
  */

  BSPTree<VertexType>* transformTree(glm::mat4 transform) {
    BSPTree<VertexType>* newtree = new BSPTree();
    newtree->vertices = this->vertices;
    newtree->polygon_indices = this->polygon_indices;
    for(auto& v: newtree->vertices) {
      v.transform(transform);
    }
    newtree->root = this->root->deeptransform(transform); 
    return newtree;
  }

  void addTriangle(std::array<unsigned int, 3> triangle_indices) {
    // if no root, create root with this triangle
    if(root == NULL) {
      //cout << "roootefd\n";
      std::array<VertexType, 3> triangle = {vertices[triangle_indices[0]], vertices[triangle_indices[1]], vertices[triangle_indices[2]]};

      /*
      for (auto v: triangle) {
	auto p = v.getData("position");
	auto q = v.getData("normal");
	cout << "p: (" << p[0] << "," << p[1] << "," << p[2] << ")";
	cout << "q: (" << q[0] << "," << q[1] << "," << q[2] << ")";
      }
      */
      

      std::array<float,4> plane = calculate_plane_from_points(triangle);
      //return;
      cout << "root plane:" << plane[0] << "," << plane[1] << "," << plane[2] << "," << plane[3] << "\n"; 
      root = new Node(plane);
      root->addTriangle(triangle_indices);
      return;
    }
    else {
      addTriangleWhile(root, triangle_indices);
      return;
    }
  }


  BSPTree(util::PolygonMesh<VertexType>& mesh) {
    vertices = mesh.getVertexAttributes();
    polygon_indices = mesh.getPrimitives();
    int k = 0;

    root = new Node();
    root = addTriangleEff(root, polygon_indices);
  }
      
  Node * addTriangleEff(Node* inode, std::vector<unsigned int> in_polygon_indices) {

    int p = 0;
    

    std::queue<std::tuple<Node* , std::vector<unsigned int>>> ntv;
    ntv.push(std::tuple<Node*,std::vector<unsigned int>>(inode, in_polygon_indices));

    while(ntv.size() > 0) {
      std::vector<unsigned int> front;
      std::vector<unsigned int> back;
      std::vector<unsigned int> coplanar;

    p += 1;
      auto e = ntv.front();
      ntv.pop();
      auto node = get<0>(e);
      auto polygon_indices = get<1>(e);
      //cout << polygon_indices.size() << " :size\n";
      
      int k = 0;
      if(node->plane == std::array<float,4>{{0,0,0,0}}) {
	while(true) {
	  std::array<unsigned int, 3> triangle_indices = {polygon_indices[k],polygon_indices[k+1],polygon_indices[k+2]};
	  std::array<VertexType, 3> triangle = {vertices[triangle_indices[0]], vertices[triangle_indices[1]], vertices[triangle_indices[2]]};
	  k += 3;
	  std::array<float,4> pl;
	  try {
	    pl = calculate_plane_from_points(triangle); // add try block around this
	  }
	  catch (NormalZero_Exception& e) {
	    cout << "zero exception occured\n";
	    continue;
	  }
	  node->plane = pl;
	  root->addTriangle(triangle_indices);
	  break;
	}
      }

      std::vector<unsigned int> next;
      next.insert(next.end(), polygon_indices.begin() + k, polygon_indices.end());

      splitList(next, front, back, coplanar, node->plane);
      node->triangles = coplanar;

      //      cout << front.size() << " :front size\n";
      //     cout << back.size() << " :back size\n";
      //cout << coplanar.size() << " :coplanar size\n";

      
      if(front.size() > 0) {
	if (node->infront == NULL) {
	  node->infront = new Node();
	}
	ntv.push(std::tuple<Node*,std::vector<unsigned int>>(node->infront, front));
      }
      if(back.size() > 0) {
	if (node->behind == NULL) {
	  node->behind = new Node();
	}
	ntv.push(std::tuple<Node*,std::vector<unsigned int>>(node->behind, back));
      }
    }
    return inode;
  }


  void splitList(std::vector<unsigned int> indices, std::vector<unsigned int>& front, std::vector<unsigned int>& back, std::vector<unsigned int>& coplanar, std::array<float,4> plane) {

    //    cout << indices.size() << "isss\n";
    for(int j = 0; j < indices.size(); j += 3 ) {

      std::array<unsigned int, 3> triangle_indices = {indices[j+0], indices[j+1], indices[j+2]};
      std::array<VertexType, 3> triangle = {vertices[triangle_indices[0]], vertices[triangle_indices[1]], vertices[triangle_indices[2]]};
      
      int a = vertex_orientation_wrt_plane(triangle[0], plane);
      int b = vertex_orientation_wrt_plane(triangle[1], plane);
      int c = vertex_orientation_wrt_plane(triangle[2], plane); 
      
      VertexType ab;
      VertexType bc;
      VertexType ca;
      unsigned int ab_index;
      unsigned int bc_index;
      unsigned int ca_index;
      
      //cout << "abc: " << a << "," << b << "," << c << "\n";
      
      if(a*b == -1) {
	//throw runtime_error("ab less than one\n");
	ab = findIntersection(plane, triangle[0], triangle[1]);
	vertices.push_back(ab);
	ab_index = vertices.size() - 1;
      }
      if(b*c == -1) {
	//throw runtime_error("bc less than one\n");
	bc = findIntersection(plane, triangle[1], triangle[2]);
	vertices.push_back(bc);
	bc_index = vertices.size() - 1;
      }
      if(c*a == -1) {
	//throw runtime_error("ca less than one\n");
	ca = findIntersection(plane, triangle[2], triangle[0]);
	vertices.push_back(ca);
	ca_index = vertices.size() - 1;
      }
      
      if(a == 0 && b == 0 && c == 0) {
	coplanar.push_back(triangle_indices[0]);
	coplanar.push_back(triangle_indices[1]);
	coplanar.push_back(triangle_indices[2]);
      }
    
      else if ((a <= 0 && b <=0 && c <= 0)) {
	back.push_back(triangle_indices[0]);
	back.push_back(triangle_indices[1]);
	back.push_back(triangle_indices[2]);
      }
      else if (a >= 0 && b >= 0 && c >= 0) {
	front.push_back(triangle_indices[0]);
	front.push_back(triangle_indices[1]);
	front.push_back(triangle_indices[2]);
      }
      // 12 splitting cases here
      else if (a == 1 && b == -1 && c == 0) {
	std::array<VertexType,3> t1 = {triangle[0], ab, triangle[2]};//front
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2]};//behind

	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};

	front.push_back(t1_indices[0]);
	front.push_back(t1_indices[1]);
	front.push_back(t1_indices[2]);

	back.push_back(t2_indices[0]);
	back.push_back(t2_indices[1]);
	back.push_back(t2_indices[2]);
	
      }
      else if (a == 1 && b == -1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, bc}; //front
	std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; // front
	std::array<VertexType,3> t3 = {ab, triangle[1], bc}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index ,triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};

	front.push_back(t1_indices[0]);
	front.push_back(t1_indices[1]);
	front.push_back(t1_indices[2]);

	front.push_back(t2_indices[0]);
	front.push_back(t2_indices[1]);
	front.push_back(t2_indices[2]);

	back.push_back(t3_indices[0]);
	back.push_back(t3_indices[1]);
	back.push_back(t3_indices[2]);
      }
      else if (a == 1 && b == -1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, ca}; //front
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2] }; // behind
	std::array<VertexType,3> t3 = {ab, triangle[2], ca}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};

	front.push_back(t1_indices[0]);
	front.push_back(t1_indices[1]);
	front.push_back(t1_indices[2]);

	back.push_back(t2_indices[0]);
	back.push_back(t2_indices[1]);
	back.push_back(t2_indices[2]);

	back.push_back(t3_indices[0]);
	back.push_back(t3_indices[1]);
	back.push_back(t3_indices[2]);

      }
      else if (a == -1 && b == 1 && c == 0) {
	std::array<VertexType,3> t1 = {triangle[0], ab, triangle[2]}; //behind
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2]}; //front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};//behind
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};//front	

	front.push_back(t2_indices[0]);
	front.push_back(t2_indices[1]);
	front.push_back(t2_indices[2]);

	back.push_back(t1_indices[0]);
	back.push_back(t1_indices[1]);
	back.push_back(t1_indices[2]);

      }
      else if (a == -1 && b == 1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, ca}; //behind
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2] }; //front
	std::array<VertexType,3> t3 = {ab, triangle[2], ca}; //front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};

	front.push_back(t2_indices[0]);
	front.push_back(t2_indices[1]);
	front.push_back(t2_indices[2]);

	front.push_back(t3_indices[0]);
	front.push_back(t3_indices[1]);
	front.push_back(t3_indices[2]);

	back.push_back(t1_indices[0]);
	back.push_back(t1_indices[1]);
	back.push_back(t1_indices[2]);
      }
      else if (a == -1 && b == 1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, triangle[2]}; //behind
	std::array<VertexType,3> t2 = {ab, bc, triangle[2] }; // behind
	std::array<VertexType,3> t3 = {ab, triangle[1], bc}; //front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, bc_index, triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};

	front.push_back(t3_indices[0]);
	front.push_back(t3_indices[1]);
	front.push_back(t3_indices[2]);

	back.push_back(t1_indices[0]);
	back.push_back(t1_indices[1]);
	back.push_back(t1_indices[2]);

	back.push_back(t2_indices[0]);
	back.push_back(t2_indices[1]);
	back.push_back(t2_indices[2]);
      }
      else if (a == 0 && b == -1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //front

	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//front

	front.push_back(t2_indices[0]);
	front.push_back(t2_indices[1]);
	front.push_back(t2_indices[2]);

	back.push_back(t1_indices[0]);
	back.push_back(t1_indices[1]);
	back.push_back(t1_indices[2]);
      }
      else if (a == 0 && b == 1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //front
	std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//front
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//behind

	front.push_back(t1_indices[0]);
	front.push_back(t1_indices[1]);
	front.push_back(t1_indices[2]);

	back.push_back(t2_indices[0]);
	back.push_back(t2_indices[1]);
	back.push_back(t2_indices[2]);
      }      
      else if (a == -1 && b == 0 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// behind
	std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// front

	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};//front

	front.push_back(t2_indices[0]);
	front.push_back(t2_indices[1]);
	front.push_back(t2_indices[2]);

	back.push_back(t1_indices[0]);
	back.push_back(t1_indices[1]);
	back.push_back(t1_indices[2]);
      }
      else if (a == 1 && b == 0 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// front
	std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};

	front.push_back(t1_indices[0]);
	front.push_back(t1_indices[1]);
	front.push_back(t1_indices[2]);

	back.push_back(t2_indices[0]);
	back.push_back(t2_indices[1]);
	back.push_back(t2_indices[2]);
      }
      else if (a == 1 && b == 1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca}; //front
	std::array<VertexType,3> t2 = {ca, triangle[1], bc }; // front
	std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	std::array<unsigned int, 3> t2_indices = {ca_index, triangle_indices[1], bc_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};

	front.push_back(t1_indices[0]);
	front.push_back(t1_indices[1]);
	front.push_back(t1_indices[2]);

	front.push_back(t2_indices[0]);
	front.push_back(t2_indices[1]);
	front.push_back(t2_indices[2]);

	back.push_back(t3_indices[0]);
	back.push_back(t3_indices[1]);
	back.push_back(t3_indices[2]);
      }
      else if (a == -1 && b == -1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	std::array<VertexType,3> t2 = {triangle[0], bc, ca }; // behind
	std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; // front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, ca_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};

	front.push_back(t3_indices[0]);
	front.push_back(t3_indices[1]);
	front.push_back(t3_indices[2]);

	back.push_back(t1_indices[0]);
	back.push_back(t1_indices[1]);
	back.push_back(t1_indices[2]);

	back.push_back(t2_indices[0]);
	back.push_back(t2_indices[1]);
	back.push_back(t2_indices[2]);
      }
      else {
	cout << "yekyahua\n";
      }      

    }
    
  }

Node* addTriangleWhile(Node* inode, std::array<unsigned int, 3> itriangle_indices) {
    
  std::queue<std::tuple<Node* , std::array<unsigned int, 3>>> ntv;
  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(inode, itriangle_indices));
      
  while(ntv.size() > 0) {
    auto e = ntv.front();
    ntv.pop();
    auto node = get<0>(e);
    auto triangle_indices = get<1>(e);
    std::array<VertexType, 3> triangle = {vertices[triangle_indices[0]], vertices[triangle_indices[1]], vertices[triangle_indices[2]]};

    

    //    cout << "indices: " << triangle_indices[0] << " " << triangle_indices[1] << " " << triangle_indices[2] << "\n";
    
    /*
    for (auto v: triangle) {
      auto p = v.getData("position");
      cout <<"triangle :(" << p[0] << "," << p[1] << "," << p[2] << ")\n";
      pp += 1;
    }
    */
    
    std::array<float,4> plane;
    try {
      plane = calculate_plane_from_points(triangle);
    }
    catch (NormalZero_Exception& e) {
      cout << "zero exception occured\n";
      continue;
    }

    int a = vertex_orientation_wrt_plane(triangle[0], node->plane);
    int b = vertex_orientation_wrt_plane(triangle[1], node->plane);
    int c = vertex_orientation_wrt_plane(triangle[2], node->plane); 

    VertexType ab;
    VertexType bc;
    VertexType ca;
    unsigned int ab_index;
    unsigned int bc_index;
    unsigned int ca_index;

    //cout << "abc: " << a << "," << b << "," << c << "\n";
    
    if(a*b == -1) {
      //throw runtime_error("ab less than one\n");
      ab = findIntersection(node->plane, triangle[0], triangle[1]);
      vertices.push_back(ab);
      ab_index = vertices.size() - 1;
    }
    if(b*c == -1) {
      //throw runtime_error("bc less than one\n");
      bc = findIntersection(node->plane, triangle[1], triangle[2]);
      vertices.push_back(bc);
      bc_index = vertices.size() - 1;
    }
    if(c*a == -1) {
      //throw runtime_error("ca less than one\n");
      ca = findIntersection(node->plane, triangle[2], triangle[0]);
      vertices.push_back(ca);
      ca_index = vertices.size() - 1;
    }
    
    if(a == 0 && b == 0 && c == 0) {
      node->addTriangle(triangle_indices);
    }
    
    else if ((a <= 0 && b <=0 && c <= 0)) {
      if (node->behind == NULL) {
	
	node->behind = new Node(plane);
	node->behind->addTriangle(triangle_indices);
      } else {
	//	cout << "calling now\n";
	ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,triangle_indices));
      }
    }
    else if (a >= 0 && b >= 0 && c >= 0) {
      if (node->infront == NULL) {
	node->infront = new Node(plane);
	node->infront->addTriangle(triangle_indices);
      } else {
	ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,triangle_indices));
      }
    }
    // 12 splitting cases here
    else {
      

      if (a == 1 && b == -1 && c == 0) {
	std::array<VertexType,3> t1 = {triangle[0], ab, triangle[2]};//front
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2]};//behind

	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t1_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t2_indices));
	}
      }
      else if (a == 1 && b == -1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, bc}; //front
	std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; // front
	std::array<VertexType,3> t3 = {ab, triangle[1], bc}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index ,triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	  node->infront->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t1_indices));
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t2_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t3_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t3_indices));
	}
      }
      else if (a == 1 && b == -1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, ca}; //front
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2] }; // behind
	std::array<VertexType,3> t3 = {ab, triangle[2], ca}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t1_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t2_indices);
	  node->behind->addTriangle(t3_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t2_indices));
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t3_indices));
	}
      }
      else if (a == -1 && b == 1 && c == 0) {
	std::array<VertexType,3> t1 = {triangle[0], ab, triangle[2]}; //behind
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2]}; //front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};//behind
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};//front	

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t2_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t1_indices));
	}
      }
      else if (a == -1 && b == 1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, ca}; //behind
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2] }; //front
	std::array<VertexType,3> t3 = {ab, triangle[2], ca}; //front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t2_indices);
	  node->infront->addTriangle(t3_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t2_indices));
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t3_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t1_indices));
	}
      }

      else if (a == -1 && b == 1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, triangle[2]}; //behind
	std::array<VertexType,3> t2 = {ab, bc, triangle[2] }; // behind
	std::array<VertexType,3> t3 = {ab, triangle[1], bc}; //front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, bc_index, triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t3_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t3_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	  node->behind->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t1_indices));
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t2_indices));
	}
      }
      else if (a == 0 && b == -1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //front

	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//front

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t2_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t1_indices));
	}
      }
      else if (a == 0 && b == 1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //front
	std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//front
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//behind

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t1_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t2_indices));
	}
      }
      
      else if (a == -1 && b == 0 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// behind
	std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// front

	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};//front

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t2_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t1_indices));
	}
      }

      else if (a == 1 && b == 0 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// front
	std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t1_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t2_indices));
	}
      }

      else if (a == 1 && b == 1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca}; //front
	std::array<VertexType,3> t2 = {ca, triangle[1], bc }; // front
	std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	std::array<unsigned int, 3> t2_indices = {ca_index, triangle_indices[1], bc_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	  node->infront->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t1_indices));
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t2_indices));
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t3_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t3_indices));
	}
      }
      else if (a == -1 && b == -1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	std::array<VertexType,3> t2 = {triangle[0], bc, ca }; // behind
	std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; // front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, ca_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};

	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	  node->behind->addTriangle(t2_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t1_indices));
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind,t2_indices));
	}
	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t3_indices);
	} else {
	  ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront,t3_indices));
	}
      }

      else {
	cout << "yekyahua\n";
      }      
      //return node;
    }
  }
  return inode;
  }

  Node* addTriangle(Node* node, std::array<unsigned int, 3> triangle_indices) {
    

    cout << "adding triangle\n";
    
    std::array<VertexType, 3> triangle = {vertices[triangle_indices[0]], vertices[triangle_indices[1]], vertices[triangle_indices[2]]};    

    
    for (auto v: triangle) {
      auto p = v.getData("position");
      cout << pp <<" :(" << p[0] << "," << p[1] << "," << p[2] << ")";
      pp += 1;
    }
    

    int a = vertex_orientation_wrt_plane(triangle[0], node->plane);
    int b = vertex_orientation_wrt_plane(triangle[1], node->plane);
    int c = vertex_orientation_wrt_plane(triangle[2], node->plane); 

    VertexType ab;
    VertexType bc;
    VertexType ca;
    unsigned int ab_index;
    unsigned int bc_index;
    unsigned int ca_index;

    //    cout << "abc: " << a << "," << b << "," << c << "\n";
    
    if(a*b == -1) {
      //throw runtime_error("ab less than one\n");
      ab = findIntersection(node->plane, triangle[0], triangle[1]);
      vertices.push_back(ab);
      ab_index = vertices.size() - 1;
    }
    if(b*c == -1) {
      //throw runtime_error("bc less than one\n");
      bc = findIntersection(node->plane, triangle[1], triangle[2]);
      vertices.push_back(bc);
      bc_index = vertices.size() - 1;
    }
    if(c*a == -1) {
      //throw runtime_error("ca less than one\n");
      ca = findIntersection(node->plane, triangle[2], triangle[0]);
      vertices.push_back(ca);
      ca_index = vertices.size() - 1;
    }
    
    if(a == 0 && b == 0 && c == 0) {
      node->addTriangle(triangle_indices);
      return node;
    }
    
    else if ((a <= 0 && b <=0 && c <= 0)) {
      if (node->behind == NULL) {
	std::array<float,4> plane = calculate_plane_from_points(triangle);
	node->behind = new Node(plane);
	node->behind->addTriangle(triangle_indices);
      } else {
	//	cout << "calling now\n";
	node->behind = addTriangle(node->behind, triangle_indices);
      }
      return node;
    }
    else if (a >= 0 && b >= 0 && c >= 0) {
      if (node->infront == NULL) {
	std::array<float,4> plane = calculate_plane_from_points(triangle);
	node->infront = new Node(plane);
	node->infront->addTriangle(triangle_indices);
      } else {
	node->infront = addTriangle(node->infront, triangle_indices);
      }
      return node;
    }
    // 12 splitting cases here
    else {
      std::array<float,4> plane = calculate_plane_from_points(triangle);

      if (a == 1 && b == -1 && c == 0) {
	std::array<VertexType,3> t1 = {triangle[0], ab, triangle[2]};//front
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2]};//behind

	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	} else {
	  node->infront = addTriangle(node->infront, t1_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t2_indices);
	} else {
	  node->behind = addTriangle(node->behind, t2_indices);
	}
      }
      else if (a == 1 && b == -1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, bc}; //front
	std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; // front
	std::array<VertexType,3> t3 = {ab, triangle[1], bc}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index ,triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	  node->infront->addTriangle(t2_indices);
	} else {
	  node->infront = addTriangle(node->infront, t1_indices);
	  node->infront = addTriangle(node->infront, t2_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t3_indices);
	} else {
	  node->behind = addTriangle(node->behind, t3_indices);
	}
      }
      else if (a == 1 && b == -1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, ca}; //front
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2] }; // behind
	std::array<VertexType,3> t3 = {ab, triangle[2], ca}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	} else {
	  node->infront = addTriangle(node->infront, t1_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t2_indices);
	  node->behind->addTriangle(t3_indices);
	} else {
	  node->behind = addTriangle(node->behind, t2_indices);
	  node->behind = addTriangle(node->behind, t3_indices);
	}
      }
      else if (a == -1 && b == 1 && c == 0) {
	std::array<VertexType,3> t1 = {triangle[0], ab, triangle[2]}; //behind
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2]}; //front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};//behind
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};//front	

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t2_indices);
	} else {
	  node->infront = addTriangle(node->infront, t2_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	} else {
	  node->behind = addTriangle(node->behind, t1_indices);
	}
      }
      else if (a == -1 && b == 1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, ca}; //behind
	std::array<VertexType,3> t2 = {ab, triangle[1], triangle[2] }; //front
	std::array<VertexType,3> t3 = {ab, triangle[2], ca}; //front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t2_indices);
	  node->infront->addTriangle(t3_indices);
	} else {
	  node->infront = addTriangle(node->infront, t2_indices);
	  node->infront = addTriangle(node->infront, t3_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	} else {
	  node->behind = addTriangle(node->behind, t1_indices);
	}
      }

      else if (a == -1 && b == 1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], ab, triangle[2]}; //behind
	std::array<VertexType,3> t2 = {ab, bc, triangle[2] }; // behind
	std::array<VertexType,3> t3 = {ab, triangle[1], bc}; //front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, bc_index, triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t3_indices);
	} else {
	  node->infront = addTriangle(node->infront, t3_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	  node->behind->addTriangle(t2_indices);
	} else {
	  node->behind = addTriangle(node->behind, t1_indices);
	  node->behind = addTriangle(node->behind, t2_indices);
	}
      }
      else if (a == 0 && b == -1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //front

	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//front

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t2_indices);
	} else {
	  node->infront = addTriangle(node->infront, t2_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	} else {
	  node->behind = addTriangle(node->behind, t1_indices);
	}
      }
      else if (a == 0 && b == 1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //front
	std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//front
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//behind

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	} else {
	  node->infront = addTriangle(node->infront, t1_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t2_indices);
	} else {
	  node->behind = addTriangle(node->behind, t2_indices);
	}
      }
      
      else if (a == -1 && b == 0 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// behind
	std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// front

	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};//front

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t2_indices);
	} else {
	  node->infront = addTriangle(node->infront, t2_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	} else {
	  node->behind = addTriangle(node->behind, t1_indices);
	}
      }

      else if (a == 1 && b == 0 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// front
	std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	} else {
	  node->infront = addTriangle(node->infront, t1_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t2_indices);
	} else {
	  node->behind = addTriangle(node->behind, t2_indices);
	}
      }

      else if (a == 1 && b == 1 && c == -1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca}; //front
	std::array<VertexType,3> t2 = {ca, triangle[1], bc }; // front
	std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; //behind
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	std::array<unsigned int, 3> t2_indices = {ca_index, triangle_indices[1], bc_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};

	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t1_indices);
	  node->infront->addTriangle(t2_indices);
	} else {
	  node->infront = addTriangle(node->infront, t1_indices);
	  node->infront = addTriangle(node->infront, t2_indices);
	}
	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t3_indices);
	} else {
	  node->behind = addTriangle(node->behind, t3_indices);
	}
      }
      else if (a == -1 && b == -1 && c == 1) {
	std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	std::array<VertexType,3> t2 = {triangle[0], bc, ca }; // behind
	std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; // front
	
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, ca_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};

	if (node->behind == NULL) {
	  node->behind = new Node(plane);
	  node->behind->addTriangle(t1_indices);
	  node->behind->addTriangle(t2_indices);
	} else {
	  node->behind = addTriangle(node->behind, t1_indices);
	  node->behind = addTriangle(node->behind, t2_indices);
	}
	if (node->infront == NULL) {
	  node->infront = new Node(plane);
	  node->infront->addTriangle(t3_indices);
	} else {
	  node->infront = addTriangle(node->infront, t3_indices);
	}
      }

      else {
	cout << "yekyahua\n";
      }      
      return node;
    }
  }
  
  std::vector<unsigned int> getFrontMesh(Node* node, std::vector<VertexType>& vertices, const std::vector<unsigned int> triangle_indices) {

    /*
    auto pl = node->plane;
    cout << "plane: ";
    cout << "(" << pl[0] << "," << pl[1] << "," << pl[2] << "," << pl[3] << ")";  
    cout << "\n";

    cout << "Triangle:\n";
    for(int i = 0 ; i < node->triangles.size(); i++) {
      auto v = vertices[(node->triangles)[i]];
      auto p = v.getData("position");
      cout << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
    }
    cout << "\n";
    */
    
    /*
    cout << "Vertices:\n";
    for(int i = 0 ; i < vertices.size(); i++) {
      auto v = (vertices)[i];
      auto p = v.getData("position");
      cout << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
    }
    cout << "\n";
    */    

    std::vector<unsigned int> front;
    std::vector<unsigned int> back;
    std::vector<unsigned int> coplanar;

    int k = 0;
    while(k < triangle_indices.size()) {
      const std::array<VertexType, 3> triangle = {vertices[triangle_indices[k+0]], vertices[triangle_indices[k+1]], vertices[triangle_indices[k+2]]};

      int a = vertex_orientation_wrt_plane(triangle[0], node->plane);
      int b = vertex_orientation_wrt_plane(triangle[1], node->plane);
      int c = vertex_orientation_wrt_plane(triangle[2], node->plane); 
      VertexType ab;
      VertexType bc;
      VertexType ca;
      unsigned int ab_index;
      unsigned int bc_index;
      unsigned int ca_index;

      /*
      auto pl = node->plane;
      cout << "plane failed: ";
      cout << "(" << pl[0] << "," << pl[1] << "," << pl[2] << "," << pl[3] << ")";

      */
      //cout << "Front mesh abc:";
      //cout << a << "," << b << "," << c << "\n";
      
      
      if(a*b == -1) {
	//throw runtime_error("ab less than one\n");
	ab = findIntersection(node->plane, triangle[0], triangle[1]);
	vertices.push_back(ab);
	ab_index = vertices.size()-1;
      }
      if(b*c == -1) {
	//throw runtime_error("bc less than one\n");
	bc = findIntersection(node->plane, triangle[1], triangle[2]);
	vertices.push_back(bc);
	bc_index = vertices.size()-1;
      }
      if(c*a == -1) {
	//throw runtime_error("ca less than one\n");
	ca = findIntersection(node->plane, triangle[2], triangle[0]);
	vertices.push_back(ca);
	ca_index = vertices.size()-1;
      }
      
      
      if(a == 0 && b == 0 && c == 0) {
	//	cout << "coplanar\n";
	coplanar.push_back(triangle_indices[k+0]);
	coplanar.push_back(triangle_indices[k+1]);
	coplanar.push_back(triangle_indices[k+2]);
      }
      else if ((a <= 0 && b <=0 && c <= 0)) {
	back.push_back(triangle_indices[k+0]);
	back.push_back(triangle_indices[k+1]);
	back.push_back(triangle_indices[k+2]);
      }
      else if (a >= 0 && b >= 0 && c >= 0) {
	front.push_back(triangle_indices[k+0]);
	front.push_back(triangle_indices[k+1]);
	front.push_back(triangle_indices[k+2]);
      }
      // 12 splitting cases here
      else {
	if (a == 1 && b == -1 && c == 0) { 
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], ab_index, triangle_indices[k+2]};
	  std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[k+1], triangle_indices[k+2]};

	  front.push_back(t1_indices[0]);
	  front.push_back(t1_indices[1]);
	  front.push_back(t1_indices[2]);
	  
	  back.push_back(t2_indices[0]);
	  back.push_back(t2_indices[1]);
	  back.push_back(t2_indices[2]);
	}
	else if (a == 1 && b == -1 && c == 1) {
	  std::array<VertexType,3> t1 = {triangle[0], ab, bc}; //front
	  std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; // front
	  std::array<VertexType,3> t3 = {ab, triangle[1], bc}; //behind
	  
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], ab_index, bc_index};
	  std::array<unsigned int, 3> t2_indices = {triangle_indices[k+0], bc_index ,triangle_indices[k+2]};
	  std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[k+1], bc_index};
	  
	  front.push_back(t1_indices[0]);
	  front.push_back(t1_indices[1]);
	  front.push_back(t1_indices[2]);
	  
	  front.push_back(t2_indices[0]);
	  front.push_back(t2_indices[1]);
	  front.push_back(t2_indices[2]);
	  
	  back.push_back(t3_indices[0]);
	  back.push_back(t3_indices[1]);
	  back.push_back(t3_indices[2]);
	  
	}
	else if (a == 1 && b == -1 && c == -1) {
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], ab_index, ca_index};
	  std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[k+1], triangle_indices[k+2]};
	  std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[k+2], ca_index};
	  
	  
	  front.push_back(t1_indices[0]);
	  front.push_back(t1_indices[1]);
	  front.push_back(t1_indices[2]);
	  
	  back.push_back(t2_indices[0]);
	  back.push_back(t2_indices[1]);
	  back.push_back(t2_indices[2]);
	  
	  back.push_back(t3_indices[0]);
	  back.push_back(t3_indices[1]);
	  back.push_back(t3_indices[2]);
	  
	}
	else if (a == -1 && b == 1 && c == 0) {
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], ab_index, triangle_indices[k+2]};//behind
	  std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[k+1], triangle_indices[k+2]};//front	
	  
	  front.push_back(t2_indices[0]);
	  front.push_back(t2_indices[1]);
	  front.push_back(t2_indices[2]);
	  
	  back.push_back(t1_indices[0]);
	  back.push_back(t1_indices[1]);
	  back.push_back(t1_indices[2]);
	}
	else if (a == -1 && b == 1 && c == 1) {	
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], ab_index, ca_index};
	  std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[k+1], triangle_indices[k+2]};
	  std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[k+2], ca_index};

	  
	  front.push_back(t2_indices[0]);
	  front.push_back(t2_indices[1]);
	  front.push_back(t2_indices[2]);
	  
	  front.push_back(t3_indices[0]);
	  front.push_back(t3_indices[1]);
	  front.push_back(t3_indices[2]);
	  
	  back.push_back(t1_indices[0]);
	  back.push_back(t1_indices[1]);
	  back.push_back(t1_indices[2]);

	}
	
	else if (a == -1 && b == 1 && c == -1) {
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], ab_index, triangle_indices[k+2]};
	  std::array<unsigned int, 3> t2_indices = {ab_index, bc_index, triangle_indices[k+2]};
	  std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[k+1], bc_index};
	  
	  front.push_back(t3_indices[0]);
	  front.push_back(t3_indices[1]);
	  front.push_back(t3_indices[2]);
	  
	  back.push_back(t1_indices[0]);
	  back.push_back(t1_indices[1]);
	  back.push_back(t1_indices[2]);
	  
	  back.push_back(t2_indices[0]);
	  back.push_back(t2_indices[1]);
	  back.push_back(t2_indices[2]);
	}
	else if (a == 0 && b == -1 && c == 1) {
	  std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	  std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //front
	  
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], triangle_indices[k+1], bc_index};//behind
	  std::array<unsigned int, 3> t2_indices = {triangle_indices[k+0], bc_index, triangle_indices[k+2]};//front
	  
	  front.push_back(t2_indices[0]);
	  front.push_back(t2_indices[1]);
	  front.push_back(t2_indices[2]);
	  
	  back.push_back(t1_indices[0]);
	  back.push_back(t1_indices[1]);
	  back.push_back(t1_indices[2]);
	}
	else if (a == 0 && b == 1 && c == -1) {
	  std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //front
	  std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //behind
	  
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], triangle_indices[k+1], bc_index};//front
	  std::array<unsigned int, 3> t2_indices = {triangle_indices[k+0], bc_index, triangle_indices[k+2]};//behind
	  
	  front.push_back(t1_indices[0]);
	  front.push_back(t1_indices[1]);
	  front.push_back(t1_indices[2]);
	  
	  back.push_back(t2_indices[0]);
	  back.push_back(t2_indices[1]);
	  back.push_back(t2_indices[2]);
	  
	}
	
	else if (a == -1 && b == 0 && c == 1) {
	  std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// behind
	  std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// front
	  
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], triangle_indices[k+1], ca_index};//behind
	  std::array<unsigned int, 3> t2_indices = {triangle_indices[k+1], triangle_indices[k+2], ca_index};//front
	  
	  front.push_back(t2_indices[0]);
	  front.push_back(t2_indices[1]);
	  front.push_back(t2_indices[2]);
	  
	  back.push_back(t1_indices[0]);
	  back.push_back(t1_indices[1]);
	  back.push_back(t1_indices[2]);
	}
	
	else if (a == 1 && b == 0 && c == -1) {
	  std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// front
	  std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// behind
	  
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], triangle_indices[k+1], ca_index};
	  std::array<unsigned int, 3> t2_indices = {triangle_indices[k+1], triangle_indices[k+2], ca_index};
	  front.push_back(t1_indices[0]);
	  front.push_back(t1_indices[1]);
	  front.push_back(t1_indices[2]);
	  
	  back.push_back(t2_indices[0]);
	  back.push_back(t2_indices[1]);
	  back.push_back(t2_indices[2]);
	}
	
	else if (a == 1 && b == 1 && c == -1) {
	  std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca}; //front
	  std::array<VertexType,3> t2 = {ca, triangle[1], bc }; // front
	  std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; //behind
	  
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], triangle_indices[k+1], ca_index};
	  std::array<unsigned int, 3> t2_indices = {ca_index, triangle_indices[k+1], bc_index};
	  std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[k+2]};
	  
	  front.push_back(t1_indices[0]);
	  front.push_back(t1_indices[1]);
	  front.push_back(t1_indices[2]);
	  
	  front.push_back(t2_indices[0]);
	  front.push_back(t2_indices[1]);
	  front.push_back(t2_indices[2]);
	  
	  back.push_back(t3_indices[0]);
	  back.push_back(t3_indices[1]);
	  back.push_back(t3_indices[2]);
	  
	}
	else if (a == -1 && b == -1 && c == 1) {
	  std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	  std::array<VertexType,3> t2 = {triangle[0], bc, ca }; // behind
	  std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; // front
	  
	  std::array<unsigned int, 3> t1_indices = {triangle_indices[k+0], triangle_indices[k+1], bc_index};
	  std::array<unsigned int, 3> t2_indices = {triangle_indices[k+0], bc_index, ca_index};
	  std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[k+2]};
	  
	  front.push_back(t3_indices[0]);
	  front.push_back(t3_indices[1]);
	  front.push_back(t3_indices[2]);
	  
	  back.push_back(t1_indices[0]);
	  back.push_back(t1_indices[1]);
	  back.push_back(t1_indices[2]);
	  
	  back.push_back(t2_indices[0]);
	  back.push_back(t2_indices[1]);
	  back.push_back(t2_indices[2]);
	}
      }
      k+=3;
    }// while loop ends here

    /*
    cout << "front\n";
    for(auto i: front) {
      cout << i << ",";
    }
    cout << "\n";

    cout << "back\n";
    for(auto i: back) {
      cout << i << ",";
    }
    cout << "\n";

    if(node->infront) {
      cout << "front is there\n";
    }
    */
    
    if(node->infront != NULL && front.size() > 0) {
      cout << "front is there\n";
      front = getFrontMesh(node->infront, vertices, front);
    }
    if(node->behind != NULL && back.size() > 0) {
      cout << "back is there\n";
      back = getFrontMesh(node->behind, vertices, back);
    } else {
      back.clear();
    }
    
    front.insert(front.end(), coplanar.begin(), coplanar.end());    
    front.insert(front.end(), back.begin(), back.end());

    return front;
    
  }//getFrontMesh ends here


  
  std::vector<unsigned int> getFrontMeshWhile(Node* node, std::vector<VertexType>& vertices, const std::vector<unsigned int> itriangle_indices) {
    
    /*
    auto pl = node->plane;
    cout << "plane: ";
    cout << "(" << pl[0] << "," << pl[1] << "," << pl[2] << "," << pl[3] << ")";  
    cout << "\n";

    cout << "Triangle:\n";
    for(int i = 0 ; i < node->triangles.size(); i++) {
      auto v = vertices[(node->triangles)[i]];
      auto p = v.getData("position");
      cout << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
    }
    cout << "\n";
    */
    
    /*
    cout << "Vertices:\n";
    for(int i = 0 ; i < vertices.size(); i++) {
      auto v = (vertices)[i];
      auto p = v.getData("position");
      cout << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
    }
    cout << "\n";
    */    
    std::vector<unsigned int> front_final;
    std::vector<unsigned int> front;
    std::vector<unsigned int> back;
    std::vector<unsigned int> coplanar;
    
    int k = 0;
    while(k < itriangle_indices.size()) {
      
      std::array<unsigned int, 3> tr = {itriangle_indices[k], itriangle_indices[k+1], itriangle_indices[k+2]};
      std::queue<std::tuple<Node* , std::array<unsigned int, 3>>> ntv;
      ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node, tr));
      
      while(ntv.size() > 0) {

	//cout << "entered ntv loop\n" ;
	auto e = ntv.front();
	ntv.pop();
	auto node = get<0>(e);
	auto triangle_indices = get<1>(e);
	//cout << "triangle_indices: " << triangle_indices[0] << " " << triangle_indices[1] << " " << triangle_indices[2] << "\n" ;

	std::array<VertexType, 3> triangle = {vertices[triangle_indices[0]], vertices[triangle_indices[1]], vertices[triangle_indices[2]]};
	
	
	
	int a = vertex_orientation_wrt_plane(triangle[0], node->plane);
	int b = vertex_orientation_wrt_plane(triangle[1], node->plane);
	int c = vertex_orientation_wrt_plane(triangle[2], node->plane); 
	VertexType ab;
	VertexType bc;
	VertexType ca;
	unsigned int ab_index;
	unsigned int bc_index;
	unsigned int ca_index;
	
	/*
	  auto pl = node->plane;
	  cout << "plane failed: ";
	  cout << "(" << pl[0] << "," << pl[1] << "," << pl[2] << "," << pl[3] << ")";
	*/
	  
	//  cout << "Front mesh abc:";
	//  cout << a << "," << b << "," << c << "\n";
	
	
	if(a*b == -1) {
	  //throw runtime_error("ab less than one\n");
	  ab = findIntersection(node->plane, triangle[0], triangle[1]);
	  vertices.push_back(ab);
	  ab_index = vertices.size()-1;
	}
	if(b*c == -1) {
	  //throw runtime_error("bc less than one\n");
	  bc = findIntersection(node->plane, triangle[1], triangle[2]);
	  vertices.push_back(bc);
	  bc_index = vertices.size()-1;
	}
	if(c*a == -1) {
	  //throw runtime_error("ca less than one\n");
	  ca = findIntersection(node->plane, triangle[2], triangle[0]);
	  vertices.push_back(ca);
	  ca_index = vertices.size()-1;
	}
	
	
	if(a == 0 && b == 0 && c == 0) {
	  //	cout << "coplanar\n";
	  coplanar.push_back(triangle_indices[0]);
	  coplanar.push_back(triangle_indices[1]);
	  coplanar.push_back(triangle_indices[2]);
	}
	else if ((a <= 0 && b <=0 && c <= 0)) {
	  back.push_back(triangle_indices[0]);
	  back.push_back(triangle_indices[1]);
	  back.push_back(triangle_indices[2]);
	}
	else if (a >= 0 && b >= 0 && c >= 0) {
	  front.push_back(triangle_indices[0]);
	  front.push_back(triangle_indices[1]);
	  front.push_back(triangle_indices[2]);
	}
	// 12 splitting cases here
	else {
	  if (a == 1 && b == -1 && c == 0) { 
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	    std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	    
	    front.push_back(t1_indices[0]);
	    front.push_back(t1_indices[1]);
	    front.push_back(t1_indices[2]);
	  
	    back.push_back(t2_indices[0]);
	    back.push_back(t2_indices[1]);
	    back.push_back(t2_indices[2]);
	  }
	  else if (a == 1 && b == -1 && c == 1) {
	    std::array<VertexType,3> t1 = {triangle[0], ab, bc}; //front
	    std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; // front
	    std::array<VertexType,3> t3 = {ab, triangle[1], bc}; //behind
	    
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, bc_index};
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index ,triangle_indices[2]};
	    std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};
	    
	    front.push_back(t1_indices[0]);
	    front.push_back(t1_indices[1]);
	    front.push_back(t1_indices[2]);
	    
	    front.push_back(t2_indices[0]);
	    front.push_back(t2_indices[1]);
	    front.push_back(t2_indices[2]);
	    
	    back.push_back(t3_indices[0]);
	    back.push_back(t3_indices[1]);
	    back.push_back(t3_indices[2]);
	    
	  }
	  else if (a == 1 && b == -1 && c == -1) {
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	    std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	    std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};
	    
	    
	    front.push_back(t1_indices[0]);
	    front.push_back(t1_indices[1]);
	    front.push_back(t1_indices[2]);
	    
	    back.push_back(t2_indices[0]);
	    back.push_back(t2_indices[1]);
	    back.push_back(t2_indices[2]);
	    
	    back.push_back(t3_indices[0]);
	    back.push_back(t3_indices[1]);
	    back.push_back(t3_indices[2]);
	    
	  }
	  else if (a == -1 && b == 1 && c == 0) {
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};//behind
	    std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};//front	
	    
	    front.push_back(t2_indices[0]);
	    front.push_back(t2_indices[1]);
	    front.push_back(t2_indices[2]);
	    
	    back.push_back(t1_indices[0]);
	    back.push_back(t1_indices[1]);
	    back.push_back(t1_indices[2]);
	  }
	  else if (a == -1 && b == 1 && c == 1) {	
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	    std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	    std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};
	    
	    
	    front.push_back(t2_indices[0]);
	    front.push_back(t2_indices[1]);
	    front.push_back(t2_indices[2]);
	    
	    front.push_back(t3_indices[0]);
	    front.push_back(t3_indices[1]);
	    front.push_back(t3_indices[2]);
	    
	    back.push_back(t1_indices[0]);
	    back.push_back(t1_indices[1]);
	    back.push_back(t1_indices[2]);
	    
	  }
	  
	  else if (a == -1 && b == 1 && c == -1) {
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	    std::array<unsigned int, 3> t2_indices = {ab_index, bc_index, triangle_indices[2]};
	    std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};
	    
	    front.push_back(t3_indices[0]);
	    front.push_back(t3_indices[1]);
	    front.push_back(t3_indices[2]);
	    
	    back.push_back(t1_indices[0]);
	    back.push_back(t1_indices[1]);
	    back.push_back(t1_indices[2]);
	    
	    back.push_back(t2_indices[0]);
	    back.push_back(t2_indices[1]);
	    back.push_back(t2_indices[2]);
	  }
	  else if (a == 0 && b == -1 && c == 1) {
	    std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	    std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //front
	    
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//behind
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//front
	    
	    front.push_back(t2_indices[0]);
	    front.push_back(t2_indices[1]);
	    front.push_back(t2_indices[2]);
	    
	    back.push_back(t1_indices[0]);
	    back.push_back(t1_indices[1]);
	    back.push_back(t1_indices[2]);
	  }

	  else if (a == 0 && b == 1 && c == -1) {
	    std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //front
	    std::array<VertexType,3> t2 = {triangle[0], bc, triangle[2]}; //behind
	    
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//front
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//behind
	    
	    front.push_back(t1_indices[0]);
	    front.push_back(t1_indices[1]);
	    front.push_back(t1_indices[2]);
	    
	    back.push_back(t2_indices[0]);
	    back.push_back(t2_indices[1]);
	    back.push_back(t2_indices[2]);
	    
	  }
	  
	  else if (a == -1 && b == 0 && c == 1) {
	    std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// behind
	    std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// front
	    
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};//behind
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};//front
	    
	    front.push_back(t2_indices[0]);
	    front.push_back(t2_indices[1]);
	    front.push_back(t2_indices[2]);
	    
	    back.push_back(t1_indices[0]);
	    back.push_back(t1_indices[1]);
	    back.push_back(t1_indices[2]);
	  }
	  
	  else if (a == 1 && b == 0 && c == -1) {
	    std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca};// front
	    std::array<VertexType,3> t2 = {triangle[1], triangle[2], ca};// behind
	    
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};
	    front.push_back(t1_indices[0]);
	    front.push_back(t1_indices[1]);
	    front.push_back(t1_indices[2]);
	    
	    back.push_back(t2_indices[0]);
	    back.push_back(t2_indices[1]);
	    back.push_back(t2_indices[2]);
	  }
	  
	  else if (a == 1 && b == 1 && c == -1) {
	    std::array<VertexType,3> t1 = {triangle[0], triangle[1], ca}; //front
	    std::array<VertexType,3> t2 = {ca, triangle[1], bc }; // front
	    std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; //behind
	    
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	    std::array<unsigned int, 3> t2_indices = {ca_index, triangle_indices[1], bc_index};
	    std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};
	    
	    front.push_back(t1_indices[0]);
	    front.push_back(t1_indices[1]);
	    front.push_back(t1_indices[2]);
	    
	    front.push_back(t2_indices[0]);
	    front.push_back(t2_indices[1]);
	    front.push_back(t2_indices[2]);
	    
	    back.push_back(t3_indices[0]);
	    back.push_back(t3_indices[1]);
	    back.push_back(t3_indices[2]);
	    
	  }
	  else if (a == -1 && b == -1 && c == 1) {
	    std::array<VertexType,3> t1 = {triangle[0], triangle[1], bc}; //behind
	    std::array<VertexType,3> t2 = {triangle[0], bc, ca }; // behind
	    std::array<VertexType,3> t3 = {ca, bc, triangle[2]}; // front
	    
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, ca_index};
	    std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};
	    
	    front.push_back(t3_indices[0]);
	    front.push_back(t3_indices[1]);
	    front.push_back(t3_indices[2]);
	    
	    back.push_back(t1_indices[0]);
	    back.push_back(t1_indices[1]);
	    back.push_back(t1_indices[2]);
	    
	    back.push_back(t2_indices[0]);
	    back.push_back(t2_indices[1]);
	    back.push_back(t2_indices[2]);
	  }
	}
	
	if(node->infront != NULL && (front.size() + coplanar.size()) > 0) {
	  //cout << "front is there\n";
	  for(int t = 0; t < front.size(); t += 3) {
	    ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront, std::array<unsigned int, 3>{{front[t+0], front[t+1], front[t+2]}}));
	  }
	  front.clear();
	  for(int t = 0; t < coplanar.size(); t += 3) {
	    ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->infront, std::array<unsigned int, 3>{{coplanar[t+0], coplanar[t+1], coplanar[t+2]}}));
	  }
	  coplanar.clear();
	  //	front = getFrontMesh(node->infront, vertices, front);
	} else {
	  front_final.insert(front_final.end(), front.begin(), front.end());
	  front_final.insert(front_final.end(), coplanar.begin(), coplanar.end());
	  front.clear();
	}

	
	if(node->behind != NULL && back.size() > 0) {
	  //cout << "back is there\n";
	  for(int t = 0; t < back.size(); t += 3) {
	    ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->behind, std::array<unsigned int, 3>{{back[t+0], back[t+1], back[t+2]}}));
	    //	    ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node->back, t));
	  }
	  back.clear();
	  //back = getFrontMesh(node->behind, vertices, back);
	} else {
	  back.clear();
	}
      }// inner while ends here

      /*
      cout << "front final inserting\n";
      front_final.insert(front_final.end(), front.begin(), front.end());
      front_final.insert(front_final.end(), coplanar.begin(), coplanar.end());    
      front_final.insert(front_final.end(), back.begin(), back.end());
      cout << "front final inserted\n";
      */
      k+=3;
    }// while loop ends here
    
    /*
      cout << "front\n";
      for(auto i: front) {
      cout << i << ",";
      }
      cout << "\n";
      
      cout << "back\n";
      for(auto i: back) {
      cout << i << ",";
      }
      cout << "\n";
      
      if(node->infront) {
      cout << "front is there\n";
      }
    */
    
    /*
      if(node->infront != NULL && front.size() > 0) {
      cout << "front is there\n";
      front = getFrontMesh(node->infront, vertices, front);
      }
      if(node->behind != NULL && back.size() > 0) {
      cout << "back is there\n";
      back = getFrontMesh(node->behind, vertices, back);
      } else {
      back.clear();
      }
    */
    
    
    return front_final;
    
  }//getFrontMeshWhile ends here
  
  // clip a to b	
  Node* clip(Node* a, Node* b, std::vector<VertexType>& a_vertices) {
    
    Node *newnode = new Node(a->plane);
    //    cout << "calling front mesh\n";
    std::vector<unsigned int> new_ind = getFrontMeshWhile(b, a_vertices, a->triangles);

    newnode->triangles = new_ind;
    if(a->infront != NULL) {
      newnode->infront = clip(a->infront, b, a_vertices);
    }
    if(a->behind != NULL) {
      newnode->behind = clip(a->behind, b, a_vertices);
    }

    return newnode;
  }


  BSPTree* clipTo(BSPTree* b) {
    std::vector<VertexType> vertices_copy = this->vertices;
    Node* newroot = clip(this->root, b->root, vertices_copy);
    cout << "got new root\n";
    BSPTree* new_bsp = new BSPTree();
    new_bsp->vertices = vertices_copy;
    new_bsp->root = newroot;
    return new_bsp;
  }

  


  VertexType findIntersection(std::array<float,4> plane, VertexType a, VertexType b) {
    vector<float> apos = a.getData("position");
    vector<float> bpos = b.getData("position");
    float r = -(plane[0]*apos[0] + plane[1]*apos[1] + plane[2]*apos[2] + plane[3])/(plane[0]*(bpos[0]-apos[0]) + plane[1]*(bpos[1]-apos[1]) + plane[2]*(bpos[2]-apos[2])); 
    vector<float> intersection{apos[0] + r*(bpos[0]-apos[0]), apos[1] + r*(bpos[1]-apos[1]), apos[2] + r*(bpos[2]-apos[2])};
    VertexType v;
    v.setData("position", intersection);
    return v;
  }

  
  int vertex_orientation_wrt_plane(VertexType v, std::array<float,4> plane) {
    vector<float> c = v.getData("position");
    return sign(c[0]*plane[0] + c[1]*plane[1] + c[2]*plane[2] + plane[3]);
  }

  std::array<float,4> calculate_plane_from_points(std::array<VertexType,3> triangle) {

    /*
    for (auto v: triangle) {
      auto p = v.getData("position");
      cout <<"normal triangle :(" << p[0] << "," << p[1] << "," << p[2] << ")\n";
      pp += 1;
    }
    */
    
    
    vector<float> p = triangle[0].getData("position");
    vector<float> q = triangle[1].getData("position");
    vector<float> r = triangle[2].getData("position");

    
    
    vector<float> a{q[0]-p[0], q[1]-p[1], q[2]-p[2]};
    vector<float> b{r[0]-p[0], r[1]-p[1], r[2]-p[2]};
    
    vector<float> normal(3,0.0);
    normal[0] = a[1] * b[2] - a[2] * b[1];
    normal[1] = -a[0] * b[2] + a[2] * b[0];
    normal[2] = a[0] * b[1] - a[1] * b[0];

    //cout << "not normalized: " << normal[0] << "," << normal[1] << "," << normal[2] << "\n";

    double norm = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);

    
    if(norm == 0) {
      /*
      for (auto v: triangle) {
	auto p = v.getData("position");
	cout <<"normal triangle :(" << p[0] << "," << p[1] << "," << p[2] << ")\n";
	//pp += 1;
      }
      */
      throw NormalZero_Exception();
    }
    

    //cout << "norm: " << norm << "\n";

    normal[0] = normal[0]/norm;
    normal[1] = normal[1]/norm;
    normal[2] = normal[2]/norm;
    
    //cout << "normalized: " << normal[0] << "," << normal[1] << "," << normal[2] << "\n";
    
    if(triangle[0].hasData("normal")) {
      //cout << "normal from file\n";
      vector<float> vnormal = triangle[0].getData("normal");
      double d = vnormal[0]*normal[0] + vnormal[1]*normal[1] + vnormal[2]*normal[2];
      if(d < 0) {
	normal[0] = -normal[0];
	normal[1] = -normal[1];
	normal[2] = -normal[2];
      }
    }

    
    float d = -normal[0]*p[0] - normal[1]*p[1] - normal[2]*p[2];

    
    //    cout << "normal final: " << normal[0] << "," << normal[1] << "," << normal[2] << "\n";

    return std::array<float,4>{{normal[0], normal[1], normal[2], d}};
    
  }

  float epsilon = 0.000001;
  int sign(float i)
  {
    if (i >  epsilon) return 1;
    if (i < -epsilon) return -1;
    return 0;
  }

};


#endif
    //for (auto v: vertices) {
    //  auto p = v.getData("position");
      //cout << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
    //}

    /*
    cout << "\n\n";
    for (auto v: polygon_indices) {
      cout << v << ",";
    }
    cout << "\n\n";
    */

    //    cout << "Vector size:" << vertices.size() << "\n";
    //cout << "Indices size:" << polygon_indices.size() << "\n";

    //    cout << "pppolygon indices size:" << polygon_indices.size() << "  k: " << k <<"\n";




    /*
    cout << "Vertices:\n";
    for(int i = 0 ; i < a_vertices.size(); i++) {
      //cout << [i] << ",";
      auto v = (a_vertices)[i];
      auto p = v.getData("position");
      cout << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
    }
    cout << "\n";
    */



    //    float norm = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
    //normal[0] = normal[0]/norm;
    //normal[1] = normal[1]/norm;
    //normal[2] = normal[2]/norm;


	  //cout << "\n\ntindices:\n\n\n";
	  //cout << t1_indices[0] << "," << t1_indices[1] << "," << t1_indices[2] << "\n";
	  //cout << t2_indices[0] << "," << t2_indices[1] << "," << t2_indices[2] << "\n";
	  //cout << t3_indices[0] << "," << t3_indices[1] << "," << t3_indices[2] << "\n\n";


    //    cout << "\n\nsize measure: " << (*vertices_p).size() << " " << triangle_indices.size() << "\n";

    //    cout << "indices:\n";
    //for(auto i : triangle_indices) {
    //  cout << i << ",";
    //}
    //cout << "\n";
    
    //    std::vector<VertexType> vertices = vertices_p;


      //auto tt = node->triangles;
      //std::array<VertexType, 3> tt_triangle = {vertices[tt[0]], vertices[tt[1]], vertices[tt[2]]};
      //cout << "vertex failed: ";
      //for (auto v: tt_triangle) {
      //	auto p = v.getData("position");
      //	cout << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
      //}
      //auto pl = node->plane;
      //cout << "plane failed: ";
      //cout << "(" << pl[0] << "," << pl[1] << "," << pl[2] << "," << pl[3] << ")";
      
      
      //cout << "\n";
      


    //cout << "primitieves:\n";
    //for(auto i: indices) {
    //  cout << i << ",";
    //}


      //      cout << "polygon indices size:" << polygon_indices.size() << "\n";
      //cout << 
      //cout << k <<" - input face indeices: ";
      //cout << triangle_indices[0] << "," << triangle_indices[1] << "," << triangle_indices[2] << "\n";
      
      // call addTriangle(std::array<VertexType, 3> triangle)

      /*
      cout << "root vertex: ";
      for (auto v: triangle) {
	auto p = v.getData("position");
	cout << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
      }
      cout << "\n";
      */



//for(auto i: triangle_indices) {
      //	cout << i << ",";
      //}
      //cout << "root indices\n";



      //cout << "getIndices: ";
      //cout << triangles.size() <<"\n";
      //for(auto i : triangles) {
      //	cout << i << ",";
      //}
      //cout << "\n";
