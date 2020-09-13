#ifndef BSPTREE_H
#define BSPTREE_H

#include <iostream>
#include <array>
#include <unistd.h>
#include <queue>
#include <math.h>
#include "exceptions.h"


/*
This class defines a BSP Tree for triangular meshes.
The partitioning is done by picking a triangle from the mesh at random and
making its plane as the partitioning plane.


NOTE:: The tree stores the list of all vertices, while nodes store the indices
to the vertices.
*/

template <class VertexType>
class BSPTree {
  
private:

  // This struct defines the node of the bsp tree.
  
  typedef struct Node {
    std::array<float,4> plane; // the plane that intersects the space
    std::vector<unsigned int> triangles; // the triangles that are on this plane
    Node* behind = NULL; // all that is behind the plane (relative to normal of plane)
    Node* infront = NULL;// all that is in front of the plane
    BSPTree<VertexType>* tree = NULL;

  public:
    Node(std::array<float,4> &pl) : plane(pl)
    {
    }

    Node() {
      plane = std::array<float,4>{{0,0,0,0}};
    }

    /*
    creates a new Node with plane corresponding to triangle referred
    by triangle indices

    @param tree: tree to which this node belongs
    @param triangle_indices: the triangle_indices that will make the plane of this node
    */
    Node(BSPTree<VertexType>* tree, const std::array<unsigned int, 3> triangle_indices)
    {
      this->tree = tree;
      std::array<VertexType, 3> triangle = {tree->vertices[triangle_indices[0]],
					    tree->vertices[triangle_indices[1]],
					    tree->vertices[triangle_indices[2]]};
	
      this->plane = calculate_plane_from_points(triangle);
      this->addTriangle(triangle_indices);
    }

    /*
    The method takes the triangle indices as input and adds them to
    current node.
    
    @param triangle_indices: array of size three representing indices
                             to be added
    @returns: the current node
    */
    Node* addTriangle(std::array<unsigned int, 3> triangle_indices) {
      triangles.push_back(triangle_indices[0]);
      triangles.push_back(triangle_indices[1]);
      triangles.push_back(triangle_indices[2]);
      return this;
    }

    /*
      The method returns the vector of indices of mesh represented by subtree of this node.
      @returns: the vector of triangle indices of subtree mesh
    */
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


    /*
    the method transforms the plane of the node as per
    parameter 'transform'
    @param transform: transform matrix for transforming the node plane
    @returns: new transformed node
    */
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


    /*
    this method transforms the current node and its sub tree
    as per the parameter 'transform'
    @param transform: transform matrix for transforming the sub tree
    @returns: new node of transformed tree represented by this node
    */
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

    // this method flips the subtree represented by this node.
    // the method is not in use currently.
    Node* flip() {
      // flip plane
      auto newplane = std::array<float,4>{{-(this->plane[0]),
					   -(this->plane[1]),
					   -(this->plane[2]),
					   -(this->plane[3])}};
      Node *newnode = new Node(newplane);

      // flip triangle vertices orientation to make them
      // anti-clockwise
      std::vector<unsigned int> new_triangles(this->triangles.size(), 0.0);
      for(int i = 0; i < this->triangles.size(); i += 3) {
      	new_triangles[i] = triangles[i+2];
      	new_triangles[i+2] = triangles[i];
	new_triangles[i+1] = triangles[i+1];
      }
      newnode->triangles = new_triangles;

      if(behind != NULL) {
	newnode->infront = this->behind->flip();
      }
      if(infront != NULL) {
	newnode->behind = this->infront->flip();
      }
      return newnode;
    }

  } Node;

  // Fields of the tree
  Node* root = NULL;                     // root node of the tree 

  vector<VertexType> vertices;           // vertices of the mesh represented by this tree

  vector<unsigned int> polygon_indices;  // polygon_indices that were fed as
                                         // input while constructing the tree

  float epsilon = 0.000001;              // tolerance limit used to measure equality of two
                                         // floats
  
public:

  BSPTree() {
  }


  /*
  the constructor takes a PolygonMesh as input and creates a BSP tree
  for the mesh.
  @params mesh: PolygonMesh whose BSP Tree has to be created
  */
  BSPTree(util::PolygonMesh<VertexType>& mesh) {
    vertices = mesh.getVertexAttributes();
    polygon_indices = mesh.getPrimitives();

    // right now just selecting the first triangle for partitioning plane.
    // There can be better techniques
    std::array<unsigned int, 3> t = {polygon_indices[0],
				     polygon_indices[1],
				     polygon_indices[2]};
    root = new Node(this, t);
    polygon_indices.erase(polygon_indices.begin(), polygon_indices.begin() + 3);

    // both methods addTriangle and addTriangleRec work.
    root = addTriangle(root, polygon_indices);
  }

  // ray cast method is not supported for this tree
  std::tuple<glm::vec4, glm::vec4, float> raycast(glm::vec4 origin, glm::vec4 direction) {
    throw runtime_error("Not supported\n");
  }


  /*
  the method takes as input the triangle indices and returns
  a vector of reference wrapper to the corresponding vertices
  @params triangle_indices: triangle indices whose triangle is desired
  @returns references to array of triangle vertices
  */
  const std::array<const std::reference_wrapper<VertexType>, 3>
  getTriangle(const std::array<unsigned int, 3> triangle_indices) {

    return std::array<const std::reference_wrapper<VertexType>, 3>
      {{vertices[triangle_indices[0]],
	    vertices[triangle_indices[1]],
	    vertices[triangle_indices[2]] }};
  }
  

  /*
  The method takes input node and vector of triangle indices representing a mesh.
  The mesh is than partitioned as per partitioning plane of this node
  and added to the node, front child or behind child.
  New child nodes are created if necessary.
  @params inode: node to which triangles are to be added
  @returns: current node
  */
  Node* addTriangle(Node* inode, std::vector<unsigned int>& in_triangle_indices) {

    std::queue<std::tuple<Node* , std::vector<unsigned int>>> ntv;
    ntv.push(std::tuple<Node*, std::vector<unsigned int>>(inode, in_triangle_indices));

    while(ntv.size() > 0) {
      std::vector<unsigned int> front;
      std::vector<unsigned int> back;
      std::vector<unsigned int> coplanar;

      auto e = ntv.front();
      ntv.pop();
      auto node = get<0>(e);
      std::vector<unsigned int>& polygon_indices = get<1>(e);
      splitList(polygon_indices, node->plane, front, back, coplanar);
      node->triangles.insert(node->triangles.end(), coplanar.begin(),
			     coplanar.end());

      if(front.size() > 0) {
	if (node->infront == NULL) {
	  std::array<unsigned int, 3> t = {front[0],
					   front[1],
					   front[2]};
	  node->infront = new Node(this, t);
	  front.erase(front.begin(), front.begin() + 3);
	}
	ntv.push(std::tuple<Node*, std::vector<unsigned int>>
		 (node->infront, front));
      }
      if(back.size() > 0) {
	if (node->behind == NULL) {
	  std::array<unsigned int, 3> t = {back[0],
					   back[1],
					   back[2]};
	  node->behind = new Node(this, t);
	  back.erase(back.begin(), back.begin() + 3);
	}
	ntv.push(std::tuple<Node*, std::vector<unsigned int>>
		 (node->behind, back));
      }
    }
    return inode;
    
  }

  /*
  The method takes input node and vector of triangle indices representing a mesh.
  The mesh is than partitioned as per partitioning plane of this node
  and added to the node, front child or behind child.
  New child nodes are created if necessary.
  It is a recursive implementation of void addTriangle
  @params inode: node to which triangles are to be added
  @returns: current node
  */
  void addTriangleRec(Node* node, std::vector<unsigned int>& in_triangle_indices) {

    std::vector<unsigned int> front;
    std::vector<unsigned int> back;
    std::vector<unsigned int> coplanar;

    splitList(in_triangle_indices, node->plane, front, back, coplanar);
    node->triangles.insert(node->triangles.end(), coplanar.begin(), coplanar.end());
    
    if(front.size() > 0) {
      if (node->infront == NULL) {
	std::array<unsigned int, 3> t = {front[0],
					 front[1],
					 front[2]};
	node->infront = new Node(this, t);
	front.erase(front.begin(), front.begin() + 3);
      }
      addTriangleRec(node->infront, front);
    }
    if(back.size() > 0) {
      if (node->behind == NULL) {
	std::array<unsigned int, 3> t = {back[0],
					 back[1],
					 back[2]};
	node->behind = new Node(this, t);
	back.erase(back.begin(), back.begin() + 3);
      }
      addTriangleRec(node->behind, back);
    }    
  }


  /*
  this method flips the tree by flipping the normal of partitioing plane
  of each node and swapping front and behind nodes. the method is not in use now.
  @returns: flipped tree
  */
  BSPTree<VertexType>* flipTree() {
    BSPTree<VertexType>* newtree = new BSPTree();
    newtree->vertices = this->vertices;
    for(auto& v: newtree->vertices) {
      v.flipNormal();
    }
    
    std::vector<unsigned int> new_polygon_indices = this->polygon_indices;
    
    if(root != NULL) {
      cout << " yes not null\n";
      newtree->root = this->root->flip();
    }
    return newtree;
  }


  /*
  the method takes a list of indices and a plane and three
  placeholders - front, back and coplanar. The three placeholders
  are filled with triangle indices from 'indices' as per whether they 
  are coplanar, behind or in front of input plane.
  @params indices : triangle indices which are to be sorted in front,
                    back, and coplanar list
  @params plane : plane for partitionig the triangles
  @params front : place holder for triangle indices that are in front of plane
  @params back : place holder for triangle indices that are behind plane
  @params coplanar : place holder for triangle indices that are coplanar plane
  */
  void splitList(const std::vector<unsigned int>& indices,
		 std::array<float,4> plane,
		 std::vector<unsigned int>& front,
		 std::vector<unsigned int>& back,
		 std::vector<unsigned int>& coplanar) {

    for(int j = 0; j < indices.size(); j += 3 ) {

      std::array<unsigned int, 3> triangle_indices =
	{indices[j+0], indices[j+1], indices[j+2]};
      std::array<VertexType, 3> triangle =
	{vertices[triangle_indices[0]],
	 vertices[triangle_indices[1]],
	 vertices[triangle_indices[2]]};
      
      int a = vertex_orientation_wrt_plane(triangle[0], plane);
      int b = vertex_orientation_wrt_plane(triangle[1], plane);
      int c = vertex_orientation_wrt_plane(triangle[2], plane); 
      
      VertexType ab;
      VertexType bc;
      VertexType ca;
      unsigned int ab_index;
      unsigned int bc_index;
      unsigned int ca_index;
      
      if(a*b == -1) {
	ab = findIntersection(plane, triangle[0], triangle[1]);
	vertices.push_back(ab);
	ab_index = vertices.size() - 1;
      }
      if(b*c == -1) {
	bc = findIntersection(plane, triangle[1], triangle[2]);
	vertices.push_back(bc);
	bc_index = vertices.size() - 1;
      }
      if(c*a == -1) {
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
    }
  }



  /*
    The method returns a new transformed tree as per input transform
    @params transform: transformation to be implemented on the tree
    @returns: pointer to new transformed tree
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


  /*
    This method returns a polygon mesh represented by this tree.
    @returns: PolygonMesh represented by this tree.
   */
  util::PolygonMesh<VertexType>* getMesh() {
    util::PolygonMesh<VertexType>* mesh = new util::PolygonMesh<VertexType>();
    mesh->setVertexData(vertices);
    cout << root << "\n"; 
    vector<unsigned int> indices = root->getIndices();
    mesh->setPrimitives(indices);
    return mesh;
  }

    

  /*
    This method clips a returns a vector representing triangles that lie in 
    front of the mesh represented by the node given as input.
    The method takes as input a node, vector of vertex and indices,
    flag for whether to include coplanar triangles
    and a flag of wheter the mesh represented by input node should be inverted
    before clipping the triangles.
    @params node : node representing the mesh with respect to which clipping is done
    @params vertices: list of vertices that are part of the mesh to be clipped
    @params itriangle_indices: vector of indices representing the mesh to be clipped
    @coplanar_indices: flag indicating whter coplanar triangles should be retained in 
                       clipped mesh
    @inverted: flag indicating if the reference mesh should be inverted before judging
               which triangle lies in front and which at back
    @returns: vector of triangle_indices that lie in front of mesh represented by node.
    @modifies: vertices : If a triangle is to be split and new vertices added, they are added
                          to the vector vertices. 
   */
  std::vector<unsigned int> getFrontMeshWhile(Node* node,
					      std::vector<VertexType>& vertices,
					      const std::vector<unsigned int>& itriangle_indices,
					      bool coplanar_include = false,
					      bool inverted = true) {
    
    std::vector<unsigned int> front_final;
    
    int k = 0;
    while(k < itriangle_indices.size()) {
      
      std::array<unsigned int, 3> tr = {itriangle_indices[k], itriangle_indices[k+1], itriangle_indices[k+2]};
      std::queue<std::tuple<Node* , std::array<unsigned int, 3>>> ntv;
      ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>(node, tr));
      
      while(ntv.size() > 0) {

	std::vector<unsigned int> front;
	std::vector<unsigned int> back;
	std::vector<unsigned int> coplanar;
	

	auto e = ntv.front();
	ntv.pop();
	auto node = get<0>(e);
	auto triangle_indices = get<1>(e);

	std::array<VertexType, 3>
	  triangle{{vertices[triangle_indices[0]], vertices[triangle_indices[1]],
		    vertices[triangle_indices[2]]}};
	
	int a = vertex_orientation_wrt_plane(triangle[0], node->plane);
	int b = vertex_orientation_wrt_plane(triangle[1], node->plane);
	int c = vertex_orientation_wrt_plane(triangle[2], node->plane); 
	
	if(inverted) {
	  a = -a;
	  b = -b;
	  c = -c;
	}
	
	VertexType ab;
	VertexType bc;
	VertexType ca;
	unsigned int ab_index;
	unsigned int bc_index;
	unsigned int ca_index;	
	
	if(a*b == -1) {
	  ab = findIntersection(node->plane, triangle[0], triangle[1]);
	  vertices.push_back(ab);
	  ab_index = vertices.size()-1;
	}
	if(b*c == -1) {
	  bc = findIntersection(node->plane, triangle[1], triangle[2]);
	  vertices.push_back(bc);
	  bc_index = vertices.size()-1;
	}
	if(c*a == -1) {
	  ca = findIntersection(node->plane, triangle[2], triangle[0]);
	  vertices.push_back(ca);
	  ca_index = vertices.size()-1;
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
	else {
	  if (a == 1 && b == -1 && c == 0) { 
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						      triangle_indices[2]};
	    std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1],
						      triangle_indices[2]};
	    
	    front.push_back(t1_indices[0]);
	    front.push_back(t1_indices[1]);
	    front.push_back(t1_indices[2]);
	  
	    back.push_back(t2_indices[0]);
	    back.push_back(t2_indices[1]);
	    back.push_back(t2_indices[2]);
	  }
	  else if (a == 1 && b == -1 && c == 1) {
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						      bc_index};
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index ,
						      triangle_indices[2]};
	    std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1],
						      bc_index};
	    
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
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						      ca_index};
	    std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1],
						      triangle_indices[2]};
	    std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2],
						      ca_index};
	    
	    
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
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						      triangle_indices[2]};//behind
	    std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1],
						      triangle_indices[2]};//front	
	    
	    front.push_back(t2_indices[0]);
	    front.push_back(t2_indices[1]);
	    front.push_back(t2_indices[2]);
	    
	    back.push_back(t1_indices[0]);
	    back.push_back(t1_indices[1]);
	    back.push_back(t1_indices[2]);
	  }
	  else if (a == -1 && b == 1 && c == 1) {	
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						      ca_index};
	    std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1],
						      triangle_indices[2]};
	    std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2],
						      ca_index};
	    
	    
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
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						      triangle_indices[2]};
	    std::array<unsigned int, 3> t2_indices = {ab_index, bc_index,
						      triangle_indices[2]};
	    std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1],
						      bc_index};
	    
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
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						      triangle_indices[1],
						      bc_index};//behind
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[0],
						      bc_index,
						      triangle_indices[2]};//front
	    
	    front.push_back(t2_indices[0]);
	    front.push_back(t2_indices[1]);
	    front.push_back(t2_indices[2]);
	    
	    back.push_back(t1_indices[0]);
	    back.push_back(t1_indices[1]);
	    back.push_back(t1_indices[2]);
	  }

	  else if (a == 0 && b == 1 && c == -1) {
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						      triangle_indices[1],
						      bc_index};//front
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[0],
						      bc_index,
						      triangle_indices[2]};//behind
	    
	    front.push_back(t1_indices[0]);
	    front.push_back(t1_indices[1]);
	    front.push_back(t1_indices[2]);
	    
	    back.push_back(t2_indices[0]);
	    back.push_back(t2_indices[1]);
	    back.push_back(t2_indices[2]);
	    
	  }
	  
	  else if (a == -1 && b == 0 && c == 1) {
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						      triangle_indices[1],
						      ca_index};//behind
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[1],
						      triangle_indices[2],
						      ca_index};//front
	    
	    front.push_back(t2_indices[0]);
	    front.push_back(t2_indices[1]);
	    front.push_back(t2_indices[2]);
	    
	    back.push_back(t1_indices[0]);
	    back.push_back(t1_indices[1]);
	    back.push_back(t1_indices[2]);
	  }
	  
	  else if (a == 1 && b == 0 && c == -1) {
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						      triangle_indices[1],
						      ca_index};
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[1],
						      triangle_indices[2],
						      ca_index};
	    front.push_back(t1_indices[0]);
	    front.push_back(t1_indices[1]);
	    front.push_back(t1_indices[2]);
	    
	    back.push_back(t2_indices[0]);
	    back.push_back(t2_indices[1]);
	    back.push_back(t2_indices[2]);
	  }
	  
	  else if (a == 1 && b == 1 && c == -1) {
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						      triangle_indices[1],
						      ca_index};
	    std::array<unsigned int, 3> t2_indices = {ca_index, triangle_indices[1],
						      bc_index};
	    std::array<unsigned int, 3> t3_indices = {ca_index, bc_index,
						      triangle_indices[2]};
	    
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
	    std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						      triangle_indices[1],
						      bc_index};
	    std::array<unsigned int, 3> t2_indices = {triangle_indices[0],
						      bc_index, ca_index};
	    std::array<unsigned int, 3> t3_indices = {ca_index, bc_index,
						      triangle_indices[2]};
	    
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

	if(!inverted) {
	  if(node->infront != NULL && (front.size() + coplanar.size()) > 0) {
	    for(int t = 0; t < front.size(); t += 3) {
	      ntv.push(std::tuple<Node*, std::array<unsigned int, 3>>
		       (node->infront,
			std::array<unsigned int, 3>
			{{front[t+0], front[t+1], front[t+2]}}));
	    }
	    front.clear();
	    
	    for(int t = 0; t < coplanar.size(); t += 3) {
	      ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>
		       (node->infront,
			std::array<unsigned int, 3>
			{{coplanar[t+0], coplanar[t+1], coplanar[t+2]}}));
	    }
	    coplanar.clear();

	  } else {
	    front_final.insert(front_final.end(), front.begin(), front.end());
	    if(coplanar_include) {
	      front_final.insert(front_final.end(), coplanar.begin(), coplanar.end());
	    }
	    front.clear();
	    coplanar.clear();
	  }
	  
	  if(node->behind != NULL && back.size() > 0) {
	    for(int t = 0; t < back.size(); t += 3) {
	      ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>
		       (node->behind, std::array<unsigned int, 3>
			{{back[t+0], back[t+1], back[t+2]}}));
	    }
	    back.clear();
	  } else {
	    back.clear();
	  }
	}

	
	if(inverted) {
	  if(node->infront != NULL && (back.size()) > 0) {
	    for(int t = 0; t < back.size(); t += 3) {
	      ntv.push(std::tuple<Node*, std::array<unsigned int, 3>>
		       (node->infront,
			std::array<unsigned int, 3>
			{{back[t+0], back[t+1], back[t+2]}}));
	    }
	    back.clear();
	    
	  } else {
	    back.clear();
	  }
	  
	  if(node->behind != NULL && (front.size() + coplanar.size()) > 0) {
	    for(int t = 0; t < front.size(); t += 3) {
	      ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>
		       (node->behind, std::array<unsigned int, 3>
			{{front[t+0], front[t+1], front[t+2]}}));
	    }
	    front.clear();
	    for(int t = 0; t < coplanar.size(); t += 3) {
	      ntv.push(std::tuple<Node*,std::array<unsigned int, 3>>
		       (node->behind,
			std::array<unsigned int, 3>
			{{coplanar[t+0], coplanar[t+1], coplanar[t+2]}}));
	    }
	    coplanar.clear();
	    
	  } else {
	    front_final.insert(front_final.end(), front.begin(), front.end());
	    if(coplanar_include) {
	      front_final.insert(front_final.end(), coplanar.begin(), coplanar.end());
	    }
	    front.clear();
	    coplanar.clear();
	  }
	  
	}	
      }                                   // inner while ends here
      k+=3;
    }                                     // outer while loop ends here
    return front_final;
  }                                        //getFrontMeshWhile ends here



  
  /*
    The method clips the mesh represented by subtree of Node 'a' with respect to mesh 
    represented by subtree of Node 'b'
    @params a : Node representing the mesh/subtree to be clipped
    @params b : Node representing the mesh/subtree with respect to which clipping is done
    @params a_vertices: The vector of vertices to which the sub tree of Node a contains 
                        references
    @params coplanar: flag to indicate if to retain coplanar triangles
    @params inverted: flag indicating if to invert the tree represented by 
                      Node 'b' before clipping.
    @returns : root node of the clipped tree
   */
  Node* clip(Node* a, Node* b, std::vector<VertexType>& a_vertices,
	     bool coplanar = true, bool inverted = false) {

    Node *newnode = new Node(a->plane);
    std::vector<unsigned int> new_ind = getFrontMeshWhile(b, a_vertices,
							  a->triangles,
							  coplanar, inverted);

    
    newnode->triangles = new_ind;
    if(a->infront != NULL) {
      newnode->infront = clip(a->infront, b, a_vertices, coplanar, inverted);
    }
    if(a->behind != NULL) {
      newnode->behind = clip(a->behind, b, a_vertices, coplanar, inverted);
    }
    return newnode;
  }


  /*
    The method clips the mesh represented by input tree with respect to this tree 
    @params b : Tree to be clipped
    @params coplanar: flag to indicate if to retain coplanar triangles
    @params inverted: flag indicating if to invert the tree represented by Node 'b' 
                      before clipping.
    @returns : pointer to the clipped tree
   */
  BSPTree* clipTo(BSPTree* b, bool coplanar = true, bool inverted = false) {
    std::vector<VertexType> vertices_copy = this->vertices;
    Node* newroot = clip(this->root, b->root, vertices_copy, coplanar, inverted);
    BSPTree* new_bsp = new BSPTree();
    new_bsp->vertices = vertices_copy;
    new_bsp->root = newroot;
    return new_bsp;
  }


  
  /*
    This method implements union of this BSP tree with input tree b and 
    returns a new tree.
    @params b : other bsp tree with which union is to be done
    @returns : new BSPTree containing the union
  */
  BSPTree* unionbsp(BSPTree<VertexAttrib>* b) {
    BSPTree<VertexAttrib>* bsp_c = this->clipTo(b, true, false);
    BSPTree<VertexAttrib>* bsp_d = b->clipTo(this, false, false);
    int d = bsp_c->vertices.size();
    bsp_c->vertices.insert(bsp_c->vertices.end(),
			   bsp_d->vertices.begin(), bsp_d->vertices.end());
    std::vector<unsigned int> indices = bsp_d->root->getIndices();
    for(int i = 0; i < indices.size(); i++) {
      indices[i] = indices[i] + d;
    }
    bsp_c->addTriangle(bsp_c->root, indices);
    return bsp_c;
  }
  
  /*
    This method implements difference of this BSP tree with input tree b and 
    returns a new tree.
    @params b : other bsp tree with which is to be subtracted froom this tree
    @returns : new BSPTree containing the this\b
  */
  BSPTree* subtractionbsp(BSPTree<VertexAttrib>* b) {
    BSPTree<VertexAttrib>* bsp_c = this->clipTo(b, false, false);
    BSPTree<VertexAttrib>* bsp_d = b->clipTo(this, false, true);
    int d = bsp_c->vertices.size();
    bsp_c->vertices.insert(bsp_c->vertices.end(),
			   bsp_d->vertices.begin(), bsp_d->vertices.end());
    std::vector<unsigned int> indices = bsp_d->root->getIndices();
    for(int i = 0; i < indices.size(); i++) {
      indices[i] = indices[i] + d;
    }
    bsp_c->addTriangle(bsp_c->root, indices);
    return bsp_c;
  }

  /*
    This method implements intersection of this BSP tree with input tree b and 
    returns a new tree.
    @params b : other bsp tree with which intersection is to be done
    @returns : new BSPTree containing the intersection
  */
  BSPTree* intersectionbsp(BSPTree<VertexAttrib>* b) {
    BSPTree<VertexAttrib>* bsp_c = this->clipTo(b, true, true);
    BSPTree<VertexAttrib>* bsp_d = b->clipTo(this, false, true);
    int d = bsp_c->vertices.size();
    bsp_c->vertices.insert(bsp_c->vertices.end(),
			   bsp_d->vertices.begin(), bsp_d->vertices.end());
    std::vector<unsigned int> indices = bsp_d->root->getIndices();
    for(int i = 0; i < indices.size(); i++) {
      indices[i] = indices[i] + d;
    }
    bsp_c->addTriangle(bsp_c->root, indices);
    return bsp_c;
  }


  /*
    This method computes and returns the intersection of a line represented by two vertices
    and a plane
    @params plane : plane of intersection
    @params a : first vertex
    @params b : second vertex
  */
  VertexType findIntersection(std::array<float,4>& plane, VertexType& a, VertexType& b) {
    vector<float> apos = a.getData("position");
    vector<float> bpos = b.getData("position");
    float r = -(plane[0]*apos[0] + plane[1]*apos[1] + plane[2]*apos[2] + plane[3])
      /(plane[0]*(bpos[0]-apos[0]) + plane[1]*(bpos[1]-apos[1]) + plane[2]*(bpos[2]-apos[2])); 

    vector<float> intersection{apos[0] + r*(bpos[0]-apos[0]), apos[1] + r*(bpos[1]-apos[1]),
			       apos[2] + r*(bpos[2]-apos[2])};
    VertexType v;
    v.setData("position", intersection);
    return v;
  }

  /*
    This method computes the vertex orientation with resepct to a plane
    @params v : vertex whose orientation is to be computed
    @params plane : plane with respect to which orientation is to be computed
    @returns : integer 0 for coplanar, 1 for front, -1 for behind
   */
  int vertex_orientation_wrt_plane(const VertexType& v, const std::array<float,4>& plane) {
    vector<float> c = v.getData("position");
    return sign(c[0]*plane[0] + c[1]*plane[1] + c[2]*plane[2] + plane[3]);
  }


  /*
    This method returns the sign of a float.
   */
  int sign(float i)
  {
    if (i >  epsilon) return 1;
    if (i < -epsilon) return -1;
    return 0;
  }

};


#endif
