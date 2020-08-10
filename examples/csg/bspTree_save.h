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

template <class VertexType>
class BSPNode {
public:
  BSPTree<VertexType>* tree = NULL;
  int level ;

  BSPNode() {
  }
  
protected:

  BSPNode(BSPTree<VertexType>* t, int l) {
    level = l;
    tree = t;
  }

public:
  virtual BSPNode<VertexType>* addTriangles(std::vector<std::array<unsigned int,3>> triangle_indices) = 0;
  virtual std::vector<unsigned int> getIndices() = 0;
  virtual BSPNode<VertexType>* transform(glm::mat4 transform) = 0;
  virtual BSPNode<VertexType>* deeptransform(glm::mat4 transform) = 0;
  
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



  std::tuple<std::vector<std::array<unsigned int,3>>,std::vector<std::array<unsigned int,3>>>
  splitMesh(const std::vector<std::array<unsigned int,3>> i_triangle_indices,
	    const std::array<float, 4> plane,
	    std::vector<VertexType>& vertices) {


    std::vector<std::array<unsigned int,3>> front;
    std::vector<std::array<unsigned int,3>> back;

    for(auto& triangle_indices: i_triangle_indices) {

      std::array<VertexType, 3> triangle = {vertices[triangle_indices[0]], vertices[triangle_indices[1]], vertices[triangle_indices[2]]};  
      //cout << "triangle indices: " << triangle_indices[0] << ", " << triangle_indices[1] << "," << triangle_indices[2] << "\n";

      //auto p1 = vertices[triangle_indices[0]].getData("position");
      //auto p2 = vertices[triangle_indices[1]].getData("position");
      //auto p3 = vertices[triangle_indices[2]].getData("position");
      
      //cout << "triangle vertices: " << triangle_indices[0] << ", " << triangle_indices[1] << "," << triangle_indices[2] << "\n";
      //      cout << p1[0] << "," << p1[1] << "," << p1[2] << " v1\n";
      //cout << p2[0] << "," << p2[1] << "," << p2[2] << " v2\n";
      //cout << p3[0] << "," << p3[1] << "," << p3[2] << " v3\n";
      
						
      auto& v1 = vertices[triangle_indices[0]];
      auto& v2 = vertices[triangle_indices[1]];
      auto& v3 = vertices[triangle_indices[2]];
      
      int a = tree->vertex_orientation_wrt_plane(v1, plane);
      int b = tree->vertex_orientation_wrt_plane(v2, plane);
      int c = tree->vertex_orientation_wrt_plane(v3, plane);
      unsigned int ab_index, bc_index, ca_index;

      //      cout << "abc: " << a << "," << b << "," << c << "\n";

      
      if(a*b == -1) {
	auto ab = tree->findIntersection(plane, triangle[0], triangle[1]);
	vertices.push_back(ab);
	ab_index = vertices.size() - 1;
      }
      if(b*c == -1) {
	auto bc = tree->findIntersection(plane, triangle[1], triangle[2]);
	vertices.push_back(bc);
	bc_index = vertices.size() - 1;
      }
      if(c*a == -1) {
	auto ca = tree->findIntersection(plane, triangle[2], triangle[0]);
	vertices.push_back(ca);
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
    // 12 splitting cases here
      else if (a == 1 && b == -1 && c == 0) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	front.push_back(t1_indices);
	back.push_back(t2_indices);
      }
      else if (a == 1 && b == -1 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index ,triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};

	front.push_back(t1_indices);
	front.push_back(t2_indices);
	back.push_back(t3_indices);
      }
      else if (a == 1 && b == -1 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};

	front.push_back(t1_indices);
	back.push_back(t2_indices);
	back.push_back(t3_indices);
      }
      else if (a == -1 && b == 1 && c == 0) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};//behind
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};//front	

	front.push_back(t2_indices);
	back.push_back(t1_indices);
      }
      else if (a == -1 && b == 1 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, ca_index};
	std::array<unsigned int, 3> t2_indices = {ab_index, triangle_indices[1], triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[2], ca_index};

	front.push_back(t2_indices);
	front.push_back(t3_indices);
	back.push_back(t1_indices);
      }
      else if (a == -1 && b == 1 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], ab_index, triangle_indices[2]};
	std::array<unsigned int, 3> t2_indices = {ab_index, bc_index, triangle_indices[2]};
	std::array<unsigned int, 3> t3_indices = {ab_index, triangle_indices[1], bc_index};

	front.push_back(t3_indices);
	back.push_back(t1_indices);
	back.push_back(t2_indices);
      }
      else if (a == 0 && b == -1 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//front

	front.push_back(t2_indices);
	back.push_back(t1_indices);
      }
      else if (a == 0 && b == 1 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};//front
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, triangle_indices[2]};//behind

	front.push_back(t1_indices);
	back.push_back(t2_indices);
      }      
      else if (a == -1 && b == 0 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};//behind
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};//front

	front.push_back(t2_indices);
	back.push_back(t1_indices);
      }
      else if (a == 1 && b == 0 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[1], triangle_indices[2], ca_index};

	front.push_back(t1_indices);
	back.push_back(t2_indices);
      }
      else if (a == 1 && b == 1 && c == -1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], ca_index};
	std::array<unsigned int, 3> t2_indices = {ca_index, triangle_indices[1], bc_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};

	front.push_back(t1_indices);
	front.push_back(t2_indices);
	back.push_back(t3_indices);
      }
      else if (a == -1 && b == -1 && c == 1) {
	std::array<unsigned int, 3> t1_indices = {triangle_indices[0], triangle_indices[1], bc_index};
	std::array<unsigned int, 3> t2_indices = {triangle_indices[0], bc_index, ca_index};
	std::array<unsigned int, 3> t3_indices = {ca_index, bc_index, triangle_indices[2]};

	front.push_back(t3_indices);
	back.push_back(t1_indices);
	back.push_back(t2_indices);
      }
      else {
	cout << "ye kya hun";
      }


    }// for loop ends here

    //    cout << back.size() << "," << front.size() << " :size fr b\n";
    
    return std::tuple<std::vector<std::array<unsigned int,3>>,std::vector<std::array<unsigned int,3>>>(back, front);
  }// splitMesh ends here

  
};

template <class VertexType>
class BSPGroupNode: public BSPNode<VertexType>
{
public:
  BSPNode<VertexType>* behind = NULL;
  BSPNode<VertexType>* infront = NULL;
  std::array<float,4> plane;

  BSPGroupNode(BSPTree<VertexType>* t, std::array<float, 4> pl ,int l) : BSPNode<VertexType>(t,l) {
    plane = pl;
  }
  
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


  BSPNode<VertexType>* addTriangles(std::vector<std::array<unsigned int,3>> triangle_indices) {

    std::vector<std::array<unsigned int,3>> front;
    std::vector<std::array<unsigned int,3>> back;

    auto r = BSPNode<VertexType>::splitMesh(triangle_indices, this->plane, this->tree->vertices);
    back = get<0>(r);
    front = get<1>(r);

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
  
};

template <class VertexType>
class BSPLeafNode: public BSPNode<VertexType>
{
private:
  std::vector<std::array<unsigned int,3>> triangles;
public:

  BSPLeafNode(BSPTree<VertexType>* t, int l) : BSPNode<VertexType>(t,l) {
  }
  
  BSPNode<VertexType>* transform(glm::mat4 transform) {
    BSPNode<VertexType> *newnode = new BSPLeafNode<VertexType>(this->tree, this->level);
    return newnode;
  }

  BSPNode<VertexType>* deeptransform(glm::mat4 transform) {
    BSPNode<VertexType> *newnode = new BSPLeafNode<VertexType>(this->tree, this->level);
    return newnode;
  }


  std::vector<unsigned int> getIndices() {
    std::vector<unsigned int> ind;
    for(auto& t: triangles) {
      ind.push_back(t[0]);
      ind.push_back(t[1]);
      ind.push_back(t[2]);
      //cout << t[0] << "," << t[1] << "," << t[2];
    }
    //    cout << "leaf indices\n";
    return ind;
  }

  BSPNode<VertexType>* addTriangles(std::vector<std::array<unsigned int,3>> triangle_indices) {

    if(triangles.size() + triangle_indices.size() > 1 && this->level < 6) {
      triangle_indices.insert(triangle_indices.end(), triangles.begin(), triangles.end());
      std::array<float, 4> plane = BSPNode<VertexType>::findPartitionPlane(triangle_indices, this->level);
      //      cout << "\n\n\n" << plane[0] << "," << plane[1] << "," << plane[2] << "," << plane[3] << " :plane\n";
      BSPNode<VertexType>* newnode = new BSPGroupNode(this->tree, plane, this->level);
      newnode = newnode->addTriangles(triangle_indices);
      return newnode;
    } else {
      triangles.insert(triangles.end(), triangle_indices.begin(), triangle_indices.end());
      return this;
    }
  }

};


  
template <class VertexType>
class BSPTree {
  
public:
  BSPTree* tree;
  BSPNode<VertexType>* root = NULL;
  vector<VertexType> vertices;
  vector<unsigned int> polygon_indices;  
  
public:

  util::PolygonMesh<VertexType> *getMesh() {
    util::PolygonMesh<VertexType>* mesh = new util::PolygonMesh<VertexType>();
    mesh->setVertexData(vertices);
    vector<unsigned int> indices = root->getIndices();
    mesh->setPrimitives(indices);
    return mesh;
  }


  BSPTree() {
  }
  
  BSPTree(util::PolygonMesh<VertexType>& mesh) {
    vertices = mesh.getVertexAttributes();
    polygon_indices = mesh.getPrimitives();

    std::vector<std::array<unsigned int, 3>> triangle_indices;
    for(int i = 0; i < polygon_indices.size(); i += 3) {
      std::array<unsigned int, 3> t = {polygon_indices[i+0],polygon_indices[i+1], polygon_indices[i+2]};
      triangle_indices.push_back(t);

      for(int k : t) {
	auto v = vertices[k].getData("position");
	cout << v[0] << "," << v[1] << "," << v[2] << " : ";
      }
      cout << " all vertices\n";
    }
    
    root = new BSPLeafNode<VertexType>(this, 0);
    root = root->addTriangles(triangle_indices);
    
  }

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
    //cout << "orientation plane: " << plane[0] << "," << plane[1] << "," << plane[2] << "," << plane[3] << "\n";
    //cout << "vertex orientation: " << c[0] << "," << c[1] << "," << c[2] << "\n";
    return sign(c[0]*plane[0] + c[1]*plane[1] + c[2]*plane[2] + plane[3]);
  }

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

    
    if(norm == 0) {
      throw NormalZero_Exception();
    }
    
    normal[0] = normal[0]/norm;
    normal[1] = normal[1]/norm;
    normal[2] = normal[2]/norm;

    if(triangle[0].hasData("normal")) {
      vector<float> vnormal = triangle[0].getData("normal");
      double d = vnormal[0]*normal[0] + vnormal[1]*normal[1] + vnormal[2]*normal[2];
      if(d < 0) {
	normal[0] = -normal[0];
	normal[1] = -normal[1];
	normal[2] = -normal[2];
      }
    }

    
    float d = -normal[0]*p[0] - normal[1]*p[1] - normal[2]*p[2];
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
