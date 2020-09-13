#ifndef BSPTREE_H
#define BSPTREE_H

#include <iostream>
#include <array>
#include <unistd.h>
#include <queue>
#include <math.h>
#include "exceptions.h"


template <class VertexType>
class BSPTree;

template <class VertexType>
class BSPLeafNode;


/*
  This class represents node of the BSP tree
 */
template <class VertexType>
class BSPNode {
public:
  
  BSPTree<VertexType>* tree = NULL;        // pointer to the parent tree
  int level ;                              // indicates the leve/depth of the node in the tree

  BSPNode() {
  }
  
protected:

  /*
    This constructor creates a node with the specified level and ponter to parent tree
    @params t : parent tree
    @params l : level of the node
   */
  BSPNode(BSPTree<VertexType>* t, int l) {
    level = l;
    tree = t;
  }

public:

  /*
    This method adds new triangle to the node
    @params triangle_indices : indices representing the triangle. the actual vertices
                               are present in the tree pointed to by 'tree' field of 
                               this node.
    @returns: current node
   */
  virtual BSPNode<VertexType>* addTriangles(std::vector<std::array<unsigned int,3>>
					    triangle_indices) = 0;

  /*
    The method returns the vector of indices of mesh represented by subtree of this node.
    @returns: the vector of triangle indices of subtree mesh
  */
  virtual std::vector<unsigned int> getIndices() = 0;


  /*
    the method transforms the plane of the node as per
    parameter 'transform'
    @param transform: transform matrix for transforming the node plane
    @returns: new transformed node
  */
  virtual BSPNode<VertexType>* transform(glm::mat4 transform) = 0;

  /*
    this method transforms the current node and its sub tree
    as per the parameter 'transform'
    @param transform: transform matrix for transforming the sub tree
    @returns: new node of transformed tree represented by this node
  */
  virtual BSPNode<VertexType>* deeptransform(glm::mat4 transform) = 0;

  /*
    This method takes a ray as input and returns the hit coordinates, normal to hit
    coordinates and 't' parameter of hit. For computing this, it searches the sub tree
    represented by this node.
    @params origin: origin of the ray
    @direction: directin of the ray
    @returns : tuple of hitcoordinate, normal at hit coordinate and 't' parameter
   */
  virtual std::tuple<glm::vec4, glm::vec4, float> raycast(glm::vec3 origin,
							  glm::vec3 direction) =0;

  /*
    This method computes the partitioing plane given reference to a list of triangles.
    The method computes the average of median of all triangles in the coordinate correspodning
    to level%3.
    @params triangle_indices : indices of trinagles whose partitioning plane is to be computed
    @params level : level specifying whether the partitioing plane is parallel to x, y or z plane
    @returns: coefficients of equation of partitioning plane
   */
  std::array<float, 4> findPartitionPlane(std::vector<std::array<unsigned int,3>> triangle_indices, int level) {
    std::array<float, 4> plane = {0.0,0.0,0.0,0.0};
    
    int d_idx = level%3;

    if (d_idx == 0) {
      plane[0] = 1;
    }
    else if (d_idx == 1) {
      plane[1] = 1;
    }
    else if (d_idx == 2) {
      plane[2] = 1;
    }

    float d = 0;
    for(auto& t : triangle_indices) {
      auto v1 = tree->vertices[t[0]].getData("position");
      auto v2 = tree->vertices[t[1]].getData("position");
      auto v3 = tree->vertices[t[2]].getData("position");
      d += (v1[d_idx] + v2[d_idx] + v3[d_idx])/3;
    }
    d = d/triangle_indices.size();
    plane[3] = -d;
    return plane;
  }


  /*
    This method takes as input a vector of triangle indices, its vertices and a reference plane
    and returns tuple of three vectors of front, coplanar and behind triangle indices computed 
    from the input triangle indices
    @params i_triangle_indices: indices of triangle to be split wrt plane
    @params plane : plane wrt which triangles are to be split
    @params vertices : vector of vertices referre to by the indices.
    @returns : tuple of front, back and coplanar indices
    @modifies: if new vertices are to be added becuase any input triangle straddles across
               the partitioing plane, they are added to 'vertices' input.
   */
  std::tuple<std::vector<std::array<unsigned int,3>>,
	     std::vector<std::array<unsigned int,3>>,
	     std::vector<std::array<unsigned int,3>>>
  splitMesh(const std::vector<std::array<unsigned int,3>> i_triangle_indices,
	    const std::array<float, 4> plane,
	    std::vector<VertexType>& vertices) {


    std::vector<std::array<unsigned int,3>> front;
    std::vector<std::array<unsigned int,3>> back;
    std::vector<std::array<unsigned int,3>> both;

    for(auto& triangle_indices: i_triangle_indices) {

      std::array<VertexType, 3> triangle = {vertices[triangle_indices[0]], vertices[triangle_indices[1]], vertices[triangle_indices[2]]};  
						
      auto& v1 = vertices[triangle_indices[0]];
      auto& v2 = vertices[triangle_indices[1]];
      auto& v3 = vertices[triangle_indices[2]];
      
      int a = tree->vertex_orientation_wrt_plane(v1, plane);
      int b = tree->vertex_orientation_wrt_plane(v2, plane);
      int c = tree->vertex_orientation_wrt_plane(v3, plane);
      unsigned int ab_index, bc_index, ca_index;

      
      if(a*b == -1) {
	auto ab = tree->findIntersection(plane, triangle[0], triangle[1]);
	//	vertices.push_back(ab);
	ab_index = vertices.size() - 1;
      }
      if(b*c == -1) {
	auto bc = tree->findIntersection(plane, triangle[1], triangle[2]);
	//vertices.push_back(bc);
	bc_index = vertices.size() - 1;
      }
      if(c*a == -1) {
	auto ca = tree->findIntersection(plane, triangle[2], triangle[0]);
	//vertices.push_back(ca);
	ca_index = vertices.size() - 1;
      }
      
      
      if(a == 0 && b == 0 && c == 0) {
	front.push_back(triangle_indices);
      }
      else if ((a <= 0 && b <=0 && c <= 0)) {
	back.push_back(triangle_indices);
      }
      else if (a >= 0 && b >= 0 && c >= 0) {
	front.push_back(triangle_indices);
      }
      else if (true) {
	both.push_back(triangle_indices);
	front.push_back(triangle_indices);
	back.push_back(triangle_indices);
      }
    // 12 splitting cases here
      else if (a == 1 && b == -1 && c == 0) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						  triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1],
						  triangle_indices[2]};
	front.push_back(t1_indices);
	back.push_back(t2_indices);
      }
      else if (a == 1 && b == -1 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						  bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index ,
						  triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1],
						  bc_index};

	front.push_back(t1_indices);
	front.push_back(t2_indices);
	back.push_back(t3_indices);
      }
      else if (a == 1 && b == -1 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						  ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1],
						  triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2],
						  ca_index};

	front.push_back(t1_indices);
	back.push_back(t2_indices);
	back.push_back(t3_indices);
      }
      else if (a == -1 && b == 1 && c == 0) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						  triangle_indices[2]};//behind
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1],
						  triangle_indices[2]};//front	

	front.push_back(t2_indices);
	back.push_back(t1_indices);
      }
      else if (a == -1 && b == 1 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						  ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1],
						  triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2],
						  ca_index};

	front.push_back(t2_indices);
	front.push_back(t3_indices);
	back.push_back(t1_indices);
      }
      else if (a == -1 && b == 1 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index,
						  triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, bc_index,
						  triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1],
						  bc_index};

	front.push_back(t3_indices);
	back.push_back(t1_indices);
	back.push_back(t2_indices);
      }
      else if (a == 0 && b == -1 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						  triangle_indices[1],
						  bc_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0],
						  bc_index, triangle_indices[2]};//front

	front.push_back(t2_indices);
	back.push_back(t1_indices);
      }
      else if (a == 0 && b == 1 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						  triangle_indices[1],
						  bc_index};//front
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0],
						  bc_index,
						  triangle_indices[2]};//behind

	front.push_back(t1_indices);
	back.push_back(t2_indices);
      }      
      else if (a == -1 && b == 0 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						  triangle_indices[1],
						  ca_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1],
						  triangle_indices[2],
						  ca_index};//front

	front.push_back(t2_indices);
	back.push_back(t1_indices);
      }
      else if (a == 1 && b == 0 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						  triangle_indices[1],
						  ca_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1],
						  triangle_indices[2],
						  ca_index};

	front.push_back(t1_indices);
	back.push_back(t2_indices);
      }
      else if (a == 1 && b == 1 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						  triangle_indices[1],
						  ca_index};
	std::array<unsigned int, 3> t2_indices = {ca_index, triangle_indices[1],
						  bc_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index,
						  triangle_indices[2]};

	front.push_back(t1_indices);
	front.push_back(t2_indices);
	back.push_back(t3_indices);
      }
      else if (a == -1 && b == -1 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0],
						  triangle_indices[1], bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0],
						  bc_index, ca_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index,
						  triangle_indices[2]};

	front.push_back(t3_indices);
	back.push_back(t1_indices);
	back.push_back(t2_indices);
      }


    }      // for loop ends here

    return std::tuple<std::vector<std::array<unsigned int,3>>,
		      std::vector<std::array<unsigned int,3>>,
		      std::vector<std::array<unsigned int,3>>>(back, front, both);
  } // splitMesh ends here

};



/*
This class derives from BSPNode. It represents a group node in the BSP Tree which stores 
a partitiong plane.
 */
template <class VertexType>
class BSPGroupNode: public BSPNode<VertexType>
{
public:
  BSPNode<VertexType>* behind = NULL;             // pointer to child node behind
  BSPNode<VertexType>* infront = NULL;            // pointer to child node in front
  std::array<float,4> plane;                      // partitioning plane of this node

  /*
    The constructor takes the pointer to parent tree, partitioing plane and level of the node
    @params t: pointer to parent tree
    @params pl: partitioning plane for this node
    @params l: level/depth of this node
   */
  BSPGroupNode(BSPTree<VertexType>* t, std::array<float, 4> pl ,int l) : BSPNode<VertexType>(t,l) {
    plane = pl;
  }

  /*
    the method transforms the plane of the node as per
    parameter 'transform'
    @param transform: transform matrix for transforming the node plane
    @returns: new transformed node
  */
  BSPNode<VertexType>* transform(glm::mat4 transform) {
    glm::mat4 inverse = glm::inverse(transform);
    glm::mat4 inversetranspose = glm::transpose(inverse);
    glm::vec4 pl = {this->plane[0], this->plane[1], this->plane[2], this->plane[3]};
    glm::vec4 newpl = inversetranspose*pl;
    std::array<float, 4> newplane = {newpl[0], newpl[1], newpl[2], newpl[3]};
    
    BSPGroupNode<VertexType> *newnode = new BSPGroupNode<VertexType>(this->tree, newplane, this->level);
    newnode->behind = this->behind;
    newnode->infront = this->infront;
    return newnode;
  }

  /*
    this method transforms the current node and its sub tree
    as per the parameter 'transform'
    @param transform: transform matrix for transforming the sub tree
    @returns: new node of transformed tree represented by this node
  */  
  BSPNode<VertexType>* deeptransform(glm::mat4 transform) {
    glm::mat4 inverse = glm::inverse(transform);
    glm::mat4 inversetranspose = glm::transpose(inverse);
    glm::vec4 pl = {this->plane[0], this->plane[1], this->plane[2], this->plane[3]};
    glm::vec4 newpl = inversetranspose*pl;
    std::array<float, 4> newplane = {newpl[0], newpl[1], newpl[2], newpl[3]};
    
    BSPGroupNode<VertexType> *newnode = new BSPGroupNode<VertexType>(this->tree, newplane, this->level);
    if(this->behind != NULL) {
      newnode->behind = this->behind->deeptransform(transform);
    }
    if(this->infront != NULL) {
      newnode->infront = this->infront->deeptransform(transform);
    }
    return newnode;
  }

  /*
    This method adds new triangle to the node
    @params triangle_indices : indices representing the triangle. the actual vertices
                               are present in the tree pointed to by 'tree' field of 
                               this node.
    @returns: current node
   */

  BSPNode<VertexType>* addTriangles(std::vector<std::array<unsigned int,3>> triangle_indices) {

    std::vector<std::array<unsigned int,3>> front;
    std::vector<std::array<unsigned int,3>> back;
    std::vector<std::array<unsigned int,3>> both;

    auto r = BSPNode<VertexType>::splitMesh(triangle_indices, this->plane, this->tree->vertices);
    back = get<0>(r);
    front = get<1>(r);
    both = get<2>(r);
    
    if(both.size() > 0.2*back.size() || both.size() > 0.2*front.size()) {
      throw CannotSplitNode_Exception();
    }

    if(infront == NULL) {
      infront = new BSPLeafNode<VertexType>(this->tree, this->level + 1);
      infront = infront->addTriangles(front);
    } else {
      infront = infront->addTriangles(front);
    }
    
    if(behind == NULL) {
      behind = new BSPLeafNode<VertexType>(this->tree, this->level + 1);
      behind = behind->addTriangles(back);
    } else {
      behind = behind->addTriangles(back);
    }

    return this;
    
  }// addTriangle method ends here


  /*
    The method returns the vector of indices of mesh represented by subtree of this node.
    @returns: the vector of triangle indices of subtree mesh
  */
  std::vector<unsigned int> getIndices() {
    std::vector<unsigned int> front_ind;
    std::vector<unsigned int> back_ind;
    
    if(infront != NULL) {
      front_ind = this->infront->getIndices();
    }
    if(behind != NULL) {
      back_ind = this->behind->getIndices();
    }
    front_ind.insert(front_ind.end(), back_ind.begin(), back_ind.end());
    return front_ind;
  }


  /*
    This method takes a ray as input and returns the hit coordinates, normal to hit
    coordinates and 't' parameter of hit. For computing this, it searches the sub tree
    represented by this node.
    @params origin: origin of the ray
    @direction: directin of the ray
    @returns : tuple of hitcoordinate, normal at hit coordinate and 't' parameter
   */
  std::tuple<glm::vec4, glm::vec4, float> raycast(glm::vec3 origin, glm::vec3 direction) {
    std::tuple<glm::vec4, glm::vec4, float> r;
    int o = BSPNode<VertexType>::tree->vertex_orientation_wrt_plane(origin, this->plane);
    if(o >= 0) {
      r = infront->raycast(origin, direction);
      auto t = get<2>(r);
      if(t < 0) {
	r = behind->raycast(origin, direction);
      }
      return r;
    } else {
      r = behind->raycast(origin, direction);
      auto t = get<2>(r);
      if(t < 0) {
	r = infront->raycast(origin, direction);
      }
      return r;
    }
  }
  
}; // group node ends here






template <class VertexType>
class BSPLeafNode: public BSPNode<VertexType>
{
private:
  std::vector<std::array<unsigned int,3>> triangles;
  glm::vec4 minBounds,maxBounds; //bounding box
public:

  BSPLeafNode(BSPTree<VertexType>* t, int l) : BSPNode<VertexType>(t,l) {
  }
  
  /*
    This method just clones this node as transform do not effect the triangle indices
    @param transform: transform matrix for transforming the sub tree
    @returns: new node of transformed tree represented by this node
  */
  BSPNode<VertexType>* transform(glm::mat4 transform) {
    BSPNode<VertexType> *newnode = new BSPLeafNode<VertexType>(this->tree, this->level);
    return newnode;
  }

  /*
    This method just clones this node as transform do not effect the triangle indices
    @param transform: transform matrix for transforming the sub tree
    @returns: new node of transformed tree represented by this node
  */
  BSPNode<VertexType>* deeptransform(glm::mat4 transform) {
    BSPNode<VertexType> *newnode = new BSPLeafNode<VertexType>(this->tree, this->level);
    return newnode;
  }

  /*
    The method returns the vector of triangle indices in this node.
    @returns: the vector of triangle indices
  */
  std::vector<unsigned int> getIndices() {
    std::vector<unsigned int> ind;
    for(auto& t: triangles) {
      ind.push_back(t[0]);
      ind.push_back(t[1]);
      ind.push_back(t[2]);
    }
    return ind;
  }

  /*
    This method adds triangle to this node and  splits the node if size of 
    triangles stored in this node this grows large. Currently the limit is set to 15.
    Also it does not split the node if the level of this node is > 100.
    @params triangle_indices : vector of triangles to be stored in this node.
    @returns : this node
  */

  BSPNode<VertexType>* addTriangles(std::vector<std::array<unsigned int,3>> triangle_indices) {
    if(triangles.size() + triangle_indices.size() > 15 && this->level < 100) {
      triangle_indices.insert(triangle_indices.end(), triangles.begin(), triangles.end());
      std::array<float, 4> plane = BSPNode<VertexType>::findPartitionPlane(triangle_indices, this->level);
      BSPNode<VertexType>* newnode = new BSPGroupNode(this->tree, plane, this->level);
      try {
	newnode = newnode->addTriangles(triangle_indices);
	return newnode;
      } catch (CannotSplitNode_Exception& e) {
	triangles.insert(triangles.end(), triangle_indices.begin(), triangle_indices.end());
	computeBoundingBox();
	return this;
      }
    } else {
      triangles.insert(triangles.end(), triangle_indices.begin(), triangle_indices.end());
      computeBoundingBox();
      return this;
    }
  }

  /*
    This method adds triangle to this node and does not split the node even if size of 
    triangles stored in this grows large
    @params triangle_indices : vector of triangles to be stored in this node.
    @returns : this node
   */
  BSPNode<VertexType>* addTrianglesWithoutSplit(std::vector<std::array<unsigned int,3>>
						triangle_indices) {
    triangles.insert(triangles.end(), triangle_indices.begin(), triangle_indices.end());
    return this;
  }


  /*
    This method computes the bonding box of all the triangles in this node
    and stores it in the field of this object.
   */

  void computeBoundingBox() {
    std::vector<VertexType> vertexData;
    for(auto t: triangles) {
      vertexData.push_back(BSPNode<VertexType>::tree->vertices[t[0]]);
      vertexData.push_back(BSPNode<VertexType>::tree->vertices[t[1]]);
      vertexData.push_back(BSPNode<VertexType>::tree->vertices[t[2]]);
    }

    unsigned int i,j;

    if (vertexData.size()<=0)
        return;

    if (!vertexData[0].hasData("position"))
    {
        return;
    }

    vector<glm::vec4> positions;

    for (i=0;i<vertexData.size();i++)
    {
        vector<float> data = vertexData[i].getData("position");
        glm::vec4 pos = glm::vec4(0,0,0,1);
        switch (data.size()) {
        case 4: pos.w = data[3];
        case 3: pos.z = data[2];
        case 2: pos.y = data[1];
        case 1: pos.x = data[0];
        }
        positions.push_back(pos);
    }

    minBounds = glm::vec4(positions[0]);
    maxBounds = glm::vec4(positions[0]);

    for (j=0;j<positions.size();j++)
    {
      glm::vec4 p = positions[j];
      
      if (p.x<minBounds.x)
        {
	  minBounds.x = p.x;
        }
      
      if (p.x>maxBounds.x)
        {
	  maxBounds.x = p.x;
        }
      
      if (p.y<minBounds.y)
        {
	  minBounds.y = p.y;
        }
      
      if (p.y>maxBounds.y)
        {
	  maxBounds.y = p.y;
        }
      
      if (p.z<minBounds.z)
        {
	  minBounds.z = p.z;
        }
      
      if (p.z>maxBounds.z)
	{
	  maxBounds.z = p.z;
	}
    }
  }


  /*
    This method takes a ray as input and returns the hit coordinates, normal to hit
    coordinates and 't' parameter of hit. For computing this, it searches all the triangles
    represented by this node.
    @params origin: origin of the ray
    @direction: directin of the ray
    @returns : tuple of hitcoordinate, normal at hit coordinate and 't' parameter
   */
  
  std::tuple<glm::vec4, glm::vec4, float> raycast(glm::vec3 origin, glm::vec3 direction) {

    glm::vec4 pt;
    glm::vec4 normal;
    std::array<unsigned int, 3> min_t_indices;
    float min_t = 99999999;
    bool intersect = false;

    if(!rayHitMesh(origin, direction)) {
      return std::tuple<glm::vec4, glm::vec4, float>(glm::vec4(-1.0), glm::vec4(-1.0), -1);
    }
    
    for(const auto& t_ind: triangles) {
      float t = findTriangleRayIntersection(t_ind,origin,direction);
      if(t == -1) {
	//	cout << "here ????\n";
	continue;
      }
      else if (t >= 0 && t < min_t) {
	min_t = t;
	min_t_indices = t_ind;
      }
    }

    if(abs(min_t - 99999999) < 0.00001) {
      min_t = -1;
    }
    else if(min_t >= 0) {
      pt[0] = origin[0] + min_t*direction[0];
      pt[1] = origin[1] + min_t*direction[1];
      pt[2] = origin[2] + min_t*direction[2];
      pt[3] = 1.0;
      std::array<VertexType,3> triangle = {BSPNode<VertexType>::tree->vertices[min_t_indices[0]],
					   BSPNode<VertexType>::tree->vertices[min_t_indices[1]],
					   BSPNode<VertexType>::tree->vertices[min_t_indices[2]]};
      auto normalarr = BSPNode<VertexType>::tree->calculate_plane_from_points(triangle);
      normal[0] = normalarr[0];
      normal[1] = normalarr[1];
      normal[2] = normalarr[2];
      normal[3] = 0;

    }

    return std::tuple<glm::vec4, glm::vec4, float>(pt, normal, min_t);
    
  }


  /*
    This is a method to compute triangle ray intersection. 
    @params t_ind : input triangle
    @params orig : origin of ray
    @params dir : direction of ray
    @returns : parameter 't' such that orig + t*dir lies on triangle. If 
               there is no intersection, returns -1
   */
  float findTriangleRayIntersection(std::array<unsigned int,3> t_ind, glm::vec3 orig,
				    glm::vec3 dir) {
    std::vector<float> origin = {orig[0],orig[1],orig[2]};
    std::vector<float> direction = {dir[0],dir[1],dir[2]};
    std::vector<float> v1 = BSPNode<VertexType>::tree->vertices[t_ind[0]].getData("position");
    std::vector<float> v2 = BSPNode<VertexType>::tree->vertices[t_ind[1]].getData("position");
    std::vector<float> v3 = BSPNode<VertexType>::tree->vertices[t_ind[2]].getData("position");
    std::vector<float> e1 = {v2[0]-v1[0], v2[1] - v1[1], v2[2] - v1[2]};
    std::vector<float> e2 = {v3[0]-v2[0], v3[1] - v2[1], v3[2] - v2[2]};
    std::vector<float> e3 = {v1[0]-v3[0], v1[1] - v3[1], v1[2] - v3[2]};
    std::vector<float> normal = crossProduct(e1,e2);
    
    std::vector<float> h = crossProduct(direction, e2);
    float par = dotProduct(h, e1);
    if(abs(par) < 0.0000001) {
      return -1;
    }

    float d = -dotProduct(normal, v1);
    float t = -(dotProduct(normal, origin) + d)/(dotProduct(normal,direction));
    if (t < 0) return -1;
    
    std::vector<float> pt = {origin[0] + t*direction[0],origin[1] + t*direction[1],origin[2] + t*direction[2]};

    std::vector<float> p = {pt[0] - v1[0], pt[1] - v1[1], pt[2] - v1[2]};
    float test = dotProduct(normal,crossProduct(e1,p));
    if(test < -0.0000000001) return -1;
    std::vector<float> q = {pt[0] - v2[0], pt[1] - v2[1], pt[2] - v2[2]};
    test = dotProduct(normal,crossProduct(e2,q));
    if(test < -0.0000000001) return -1;
    std::vector<float> r = {pt[0] - v3[0], pt[1] - v3[1], pt[2] - v3[2]};
    test = dotProduct(normal,crossProduct(e3,r));
    if(test < -0.0000000001) return -1;
    return t;
  }

  /*
    returns dot product of two vectors
    @params a : first vector
    @params b: second vector
    @returns : dot product of a and b
   */
  float dotProduct(vector<float> a, vector<float> b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
  }

  /*
    This method computes norm of a vector
    @params a : vector whose norm is to be computed
    @returns : norm of a
   */
  float norm(glm::vec3 a) {
    return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
  }
  
  /*
    returns cross product of two vectors
    @params a : first vector
    @params b: second vector
    @returns : cross product of a and b
   */
  std::vector<float> crossProduct(vector<float> a, vector<float> b) {

    vector<float> normal(3,0.0);
    normal[0] = a[1] * b[2] - a[2] * b[1];
    normal[1] = -a[0] * b[2] + a[2] * b[0];
    normal[2] = a[0] * b[1] - a[1] * b[0];
    return normal;
  }


  /*
    @returns: minimum bound in x,y and z direction as a vector.
              last element of vector is 1.
   */
  glm::vec4 getMinimumBounds() const
  {
    return glm::vec4(minBounds);
  }

  /*
    @returns: maximum bound in x,y and z direction as a vector
              last element of vector is 1.
   */
  glm::vec4 getMaximumBounds() const
  {
    return glm::vec4(maxBounds);
  }


  /*
    computes if the ray might hit mesh, based on bounding box of this node
    @params orig : origin of ray
    @params direction : direction of ray
    @returns : bool if ray hit mesh or not
   */
  bool rayHitMesh(glm::vec3 origin, glm::vec3 direction) {
  
    float FLOAT_MAX = 99999999.0;
    float FLOAT_MIN = -99999999.0;
    
    float txmin;
    float tymin;
    float tzmin;
    float txmax;
    float tymax;
    float tzmax;
    
    glm::vec4 minbounds = getMinimumBounds();
    glm::vec4 maxbounds = getMaximumBounds();
    
    float xmin = minbounds[0];
    float ymin = minbounds[1];
    float zmin = minbounds[2];
    float xmax = maxbounds[0];
    float ymax = maxbounds[1];
    float zmax = maxbounds[2];
    
    float dx = direction[0];
    float dy = direction[1];
    float dz = direction[2];
    float ox = origin[0];
    float oy = origin[1];
    float oz = origin[2];
    
    if(abs(dx) <= 0.00000001) {
      if(ox < xmin || ox > xmax) {
	return false;
      } else {
	txmin = FLOAT_MIN;
	txmax = FLOAT_MAX;
      }
    } else {
      float a = (xmax - ox)/dx;
      float b = (xmin - ox)/dx;
      txmin = (a<=b)? a:b;
      txmax = (a>b)? a:b;
    }
    
    if(abs(dy) <= 0.00000001) {
      if(oy < ymin || oy > ymax) {
	return false;
      } else {
	tymin = FLOAT_MIN;
	tymax = FLOAT_MAX;
      }
    } else {
      float a = (ymax - oy)/dy;
      float b = (ymin - oy)/dy;
      tymin = (a<=b)? a:b;
      tymax = (a>b)? a:b;
    }
    
    if(abs(dz) <= 0.00000001) {
      if(oz < zmin || oz > zmax) {
	return false;
      } else {
	tzmin = FLOAT_MIN;
	tzmax = FLOAT_MAX;
      }
    } else {
      float a = (zmax - oz)/dz;
      float b = (zmin - oz)/dz;
      tzmin = (a<=b)? a:b;
      tzmax = (a>b)? a:b;
    }
    
    float tmax = txmax;
    if(tymax < tmax) tmax = tymax;
    if(tzmax < tmax) tmax = tzmax;
    
    float tmin = txmin;
    if(tymin > tmin) tmin = tymin;
    if(tzmin > tmin) tmin = tzmin;
    
    if(tmin > tmax || tmax < 0) {
      return false;
    } else {
      return true;
    }
    
  }
};



/*
This class represents a BSP Tree. The partitioning planes are parallel to x,y and z direction,
alternating as we go down the tree. The triangles that cross the partitioning plane are
stored in both the sub trees of the group node whose plane triangle straddles. Only leaf node
store the indices to triangles, while group node store the equation of partitioing plane.
 */
template <class VertexType>
class BSPTree {
  
public:
  BSPTree* tree; 
  BSPNode<VertexType>* root = NULL;                 // root node of the tree
  vector<VertexType> vertices;                      // vector of vertices present in the mesh
  vector<unsigned int> polygon_indices;             // triangle indices used to compute this tree
  
public:

  BSPTree() {
  }

  /*
    This constructor takes a mesh as input and constructs the BSP Tree for the mesh
   */
  BSPTree(util::PolygonMesh<VertexType>& mesh) {
    vertices = mesh.getVertexAttributes();
    polygon_indices = mesh.getPrimitives();

    std::vector<std::array<unsigned int, 3>> triangle_indices;
    for(int i = 0; i < polygon_indices.size(); i += 3) {
      std::array<unsigned int, 3> t = {polygon_indices[i+0],polygon_indices[i+1], polygon_indices[i+2]};
      triangle_indices.push_back(t);
    }
    
    root = new BSPLeafNode<VertexType>(this, 0);
    root = root->addTriangles(triangle_indices);
    
  }

  /*
    This method returns the mesh of this BSP Tree computed after collecting 
    all the triangles from all nodes. This is different from the mesh used to construct the tree.
    @returns : PolygonMesh represented by this BSP Tree.
   */
  util::PolygonMesh<VertexType> *getMesh() {
    util::PolygonMesh<VertexType>* mesh = new util::PolygonMesh<VertexType>();
    mesh->setVertexData(vertices);
    vector<unsigned int> indices = root->getIndices();
    mesh->setPrimitives(indices);
    return mesh;
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
    This method computes and returns the intersection of a line represented by two vertices
    and a plane
    @params plane : plane of intersection
    @params a : first vertex
    @params b : second vertex
  */
  VertexType findIntersection(std::array<float,4> plane, VertexType a, VertexType b) {
    vector<float> apos = a.getData("position");
    vector<float> bpos = b.getData("position");
    float r = -(plane[0]*apos[0] + plane[1]*apos[1] + plane[2]*apos[2] + plane[3])/(plane[0]*(bpos[0]-apos[0]) + plane[1]*(bpos[1]-apos[1]) + plane[2]*(bpos[2]-apos[2])); 
    vector<float> intersection{apos[0] + r*(bpos[0]-apos[0]), apos[1] + r*(bpos[1]-apos[1]), apos[2] + r*(bpos[2]-apos[2])};
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
  int vertex_orientation_wrt_plane(glm::vec3 c, std::array<float,4> plane) {
    return sign(c[0]*plane[0] + c[1]*plane[1] + c[2]*plane[2] + plane[3]);
  }


  /*
    This method computes the vertex orientation with resepct to a plane
    @params v : vertex whose orientation is to be computed
    @params plane : plane with respect to which orientation is to be computed
    @returns : integer 0 for coplanar, 1 for front, -1 for behind
   */  
  int vertex_orientation_wrt_plane(VertexType v, std::array<float,4> plane) {
    vector<float> c = v.getData("position");
    return sign(c[0]*plane[0] + c[1]*plane[1] + c[2]*plane[2] + plane[3]);
  }


  /*
    Calculates the coefficient of equation of plane represented by three points/ triangle
    @params triangle : triangle whose plane equation is to be computed
    @returns : array of coefficients of equation of plane
   */
  std::array<float,4> calculate_plane_from_points(std::array<VertexType,3> triangle) {
  
    vector<float> p = triangle[0].getData("position");
    vector<float> q = triangle[1].getData("position");
    vector<float> r = triangle[2].getData("position");
    
    vector<float> a{q[0]-p[0], q[1]-p[1], q[2]-p[2]};
    vector<float> b{r[0]-p[0], r[1]-p[1], r[2]-p[2]};
    
    vector<float> normal(3,0.0);
    normal[0] = a[1] * b[2] - a[2] * b[1];
    normal[1] = -a[0] * b[2] + a[2] * b[0];
    normal[2] = a[0] * b[1] - a[1] * b[0];

    double norm = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
    
    normal[0] = normal[0]/norm;
    normal[1] = normal[1]/norm;
    normal[2] = normal[2]/norm;
    
    float d = -normal[0]*p[0] - normal[1]*p[1] - normal[2]*p[2];
    return std::array<float,4>{{normal[0], normal[1], normal[2], d}};
    }


  /*
    returns the sign of a float as per specified tolerance
   */
  float epsilon = 0.000001;
  int sign(float i)
  {
    if (i >  epsilon) return 1;
    if (i < -epsilon) return -1;
    return 0;
  }


  /*
    This method takes a ray as input and returns the hit coordinates, normal to hit
    coordinates and 't' parameter of hit. For computing this, it searches this tree.
    @params origin: origin of the ray
    @direction: directin of the ray
    @returns : tuple of hitcoordinate, normal at hit coordinate and 't' parameter
   */
  std::tuple<glm::vec4, glm::vec4, float> raycast(glm::vec4 origin, glm::vec4 direction) {
    if(this->root != NULL) {
      glm::vec3 origin3 = {origin[0], origin[1], origin[2]};
      glm::vec3 direction3 = {direction[0], direction[1], direction[2]};
      auto r =  root->raycast(origin3, direction3);
      return r;
    }
    else return std::tuple<glm::vec4, glm::vec4, float>();
  }

  
};

#endif
