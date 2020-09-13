// Author is Dr. Amit Shesh from Northeastern University

#ifndef _POLYGONMESH_H_
#define _POLYGONMESH_H_

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include "math.h"

using namespace std;

namespace util
{


/*
 * This class represents a polygon mesh. This class works with any
 * representation of vertex attributes that implements the
 * @link{IVertexData} interface.
 *
 * It stores a polygon mesh as follows:
 *
 * <ul>
 *     <li>A list of vertices: Each vertex is represented by an object that
 *         stores various attributes of that vertex like position, normal,
 *         texture coordinates and others.</li>
 *     <li>A list of indices: these are indices into the above array.
 *         This is called indexed representation and allows us to share vertices
 *         between polygons efficiently.</li>
 *     <li>Data about how to interpret the indices (e.g. read 3 at a time to
 *         form a triangle, read to make a triangle fan, etc.)</li>
 *     <li>How many indices make a polygon (2 for line, 3 for triangle,
 *         4 for quad, etc.)</li>
 * </ul>
 */

template <class VertexType>
class PolygonMesh
{


public:
    PolygonMesh();
    ~PolygonMesh();
    /*
     * Set the primitive type. The primitive type is represented by an integer.
     * For example in OpenGL, these would be GL_TRIANGLES, GL_TRIANGLE_FAN,
     * GL_QUADS, etc.
     * \param v
     */
    void setPrimitiveType(int v);
    int getPrimitiveType() const;
    /*
     * Sets how many indices make up a primitive.
     * \param s
     */
    void setPrimitiveSize(int s);
    int getPrimitiveSize() const;
    int getPrimitiveCount() const;
    int getVertexCount() const;

    glm::vec4 getMinimumBounds() const;
    glm::vec4 getMaximumBounds() const;
    vector<VertexType> getVertexAttributes() const;
    vector<unsigned int> getPrimitives() const;
    void setVertexData(const vector<VertexType>& vp);
    void setPrimitives(const vector<unsigned int>& t);
    /*
     * Compute vertex normals in this polygon mesh using Newell's method, if
     * position data exists
     */
    void computeNormals();
    /*
     * Compute the bounding box of this polygon mesh, if there is position data
     */
    void computeBoundingBox();
  PolygonMesh<VertexType> getTransformedMesh(glm::mat4 transform);
  bool rayHitMesh(glm::vec4 origin, glm::vec4 direction);
  void cleanMesh();

protected:
    vector<VertexType> vertexData;
    vector<unsigned int> primitives;
    int primitiveType;
    int primitiveSize;
    glm::vec4 minBounds,maxBounds; //bounding box

};

template<class VertexType>
PolygonMesh<VertexType>::PolygonMesh()
{

}

template<class VertexType>
PolygonMesh<VertexType>::~PolygonMesh()
{

}


template<class VertexType>
void PolygonMesh<VertexType>::setPrimitiveType(int v)
{
    primitiveType = v;
}

template<class VertexType>
int PolygonMesh<VertexType>::getPrimitiveType() const { return primitiveType;}

template<class VertexType>
void PolygonMesh<VertexType>::setPrimitiveSize(int s)
{
    primitiveSize = s;
}

template<class VertexType>
int PolygonMesh<VertexType>::getPrimitiveSize() const
{
    return primitiveSize;
}

template<class VertexType>
int PolygonMesh<VertexType>::getPrimitiveCount() const
{
    return primitives.size();
}

template<class VertexType>
int PolygonMesh<VertexType>::getVertexCount() const
{
    return vertexData.size();
}

template<class VertexType>
glm::vec4 PolygonMesh<VertexType>::getMinimumBounds() const
{
    return glm::vec4(minBounds);
}

template<class VertexType>
glm::vec4 PolygonMesh<VertexType>::getMaximumBounds() const
{
    return glm::vec4(maxBounds);
}



template<class VertexType>
vector<VertexType> PolygonMesh<VertexType>::getVertexAttributes() const
{
    return vector<VertexType>(vertexData);
}

template<class VertexType>
vector<unsigned int> PolygonMesh<VertexType>::getPrimitives() const
{
    return vector<unsigned int>(primitives);
}

template <class VertexType>
void PolygonMesh<VertexType>::setVertexData(const vector<VertexType>& vp)
{
    vertexData = vector<VertexType>(vp);
    computeBoundingBox();
}

template<class VertexType>
void PolygonMesh<VertexType>::setPrimitives(const vector<unsigned int>& t)
{
    primitives = vector<unsigned int>(t);
 
}


template<class VertexType>
void PolygonMesh<VertexType>::computeBoundingBox()
{
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
 * Compute vertex normals in this polygon mesh using Newell's method, if
 * position data exists
 */

template<class VertexType>
void PolygonMesh<VertexType>::computeNormals()
{
  std::cout << "bool: ";
    int i,j,k;

    cout << vertexData[0].hasData("position") << "," << vertexData[0].hasData("normal") << "\n";

    if (vertexData.size()<=0)
        return;

    if (!vertexData[0].hasData("position"))
    {
        return;
    }


    vector<glm::vec4> positions;

    
    for (i=0;i<vertexData.size();i++) {
        vector<float> data = vertexData[i].getData("position");
        glm::vec4 pos;
        switch (data.size()) {
        case 4: pos.w = data[3];
        case 3: pos.z = data[2];
        case 2: pos.y = data[1];
        case 1: pos.x = data[0];
        }
        positions.push_back(pos);
    }
    vector<glm::vec4> normals;

    for (i=0;i<positions.size();i++)
    {
        normals.push_back(glm::vec4(0.0f,0.0f,0.0f,0.0f));
    }

    cout << "sizes:" << primitives.size() << "," << primitiveSize << "\n";
    for (i=0;i<primitives.size();i+=primitiveSize)
    {
        glm::vec4 norm = glm::vec4(0.0f,0.0f,0.0f,0.0f);



        //compute the normal of this triangle
        vector<unsigned int> v;

        for (k=0;k<primitiveSize;k++)
        {
	      
	  v.push_back(primitives[i+k]);

        }

        //the newell's method to calculate normal

        for (k=0;k<primitiveSize;k++)
        {

	  /*
	  cout << "this: ";
	  cout << v[k] << "\n";
	  cout << positions.size() << "\n";
	  auto aa = positions[v[k]];
	  cout << "suno\n";
	  cout << "idhar aaya kya: " << aa[0] <<  "\n" ;
	  */
	  
            norm[0] += (positions[v[k]][1]-positions[v[(k+1)%primitiveSize]][1])*
                      (positions[v[k]][2]+positions[v[(k+1)%primitiveSize]][2]);

	    
            norm[1] += (positions[v[k]][2]-positions[v[(k+1)%primitiveSize]][2])*
                      (positions[v[k]][0]+positions[v[(k+1)%primitiveSize]][0]);

	    norm[2] += (positions[v[k]][0]-positions[v[(k+1)%primitiveSize]][0])*
                      (positions[v[k]][1]+positions[v[(k+1)%primitiveSize]][1]);
        }
	//	cout << "computing normals" << norm[0] << "," << norm[1] << "," << norm[2] << "\n";
        norm = glm::normalize(norm);

        for (k=0;k<primitiveSize;k++)
        {
            normals[v[k]] = normals[v[k]] + norm;
        }
    }


    for (i=0;i<normals.size();i++)
    {

        normals[i] = glm::normalize(normals[i]);
    }
    for (i=0;i<vertexData.size();i++) {
        vector<float> n;

        n.push_back(normals[i].x);
        n.push_back(normals[i].y);
        n.push_back(normals[i].z);
        n.push_back(normals[i].w);
        vertexData[i].setData("normal",n);
    }
}

template<class VertexType>
PolygonMesh<VertexType> PolygonMesh<VertexType>::getTransformedMesh(glm::mat4 transform) {
  PolygonMesh<VertexType> result = *(new PolygonMesh<VertexType>());
  
  result.primitives = primitives;
  result.primitiveType = primitiveType;
  result.primitiveSize = primitiveSize;
  result.vertexData = vertexData;
  for(auto& v : result.vertexData) {
    v.transform(transform);
  }
  return result;
}


template<class VertexType>
bool PolygonMesh<VertexType>::rayHitMesh(glm::vec4 origin, glm::vec4 direction) {

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

  /*
  for(int i = 0; i < 3; i++) {
    cout << minbounds[i] << ",";
  }
  cout << " : min bounds\n";
  for(int i = 0; i < 3; i++) {
    cout << maxbounds[i] << ",";
  }
  cout << " : max bounds\n";
  for(int i = 0; i < 3; i++) {
    cout << origin[i] << ",";
  }
  cout << " : origin\n";
  for(int i = 0; i < 3; i++) {
    cout << direction[i] << ",";
  }
  cout << " : direction\n";
  */
  
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

  //  cout << "tmin: " << tmin << "  tmax: " << tmax << "\n";
  if(tmin > tmax || tmax < 0) {
    return false;
  } else {
    return true;
  }
  
}

template<class VertexType>
void PolygonMesh<VertexType>::cleanMesh() {
  vector<unsigned int> new_primitives;
  //cout << primitives.size() << "\n";
  //cout << primitiveSize << "  :primitiveSize\n";
  for (int i = 0; i < primitives.size(); i += primitiveSize) {
    bool valid = true;
    for(int j = 0; j < primitiveSize-1; j += 1) {
      for(int k = j+1; k < primitiveSize; k+= 1) {
	//	cout << i+j << " " << i + k << " comparing\n";
	auto a = vertexData[primitives[i+j]].getData("position");
	auto b = vertexData[primitives[i+k]].getData("position");
	//cout << a[0] << " " << a[1] << " " << a[2] << " " << b [0] << " " << b[1]
	//     << " " << b[2] << "\n";
	if(abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2]) < 0.0001 ) {
	  valid = false;
	  break;
	}
      }
      if(!valid) {
	break;
      }
    }
    if(valid) {
      for(int m = 0; m < primitiveSize; m++) {
	new_primitives.push_back(primitives[i+m]);
      }
    }
  }
  primitives = new_primitives;
  //cout << "primitive size: " << primitives.size() << "\n";
}

}
#endif
