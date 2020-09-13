//Author is Dr. Amit Shesh from Northeastern University

#ifndef _VERTEXATTRIB_H_
#define _VERTEXATTRIB_H_

#include <glm/glm.hpp>
#include "IVertexData.h"
#include <iostream>
#include <sstream>
#include "exceptions.h"
#include <array>

/*
 * This class represents the attributes of a single vertex. It is useful in building
 * PolygonMesh objects for many examples.
 *
 * It implements the IVertexData interface so that it can be converted into an array
 * of floats, to work with OpenGL buffers
 */
class VertexAttrib:public util::IVertexData
{


public:
    VertexAttrib()
    {
        position = glm::vec4(0,0,0,1);

    }

    ~VertexAttrib(){}



  bool hasData(string attribName) const
  {
    if (attribName == "position") {
      return hasPosition;
    }
    else if (attribName == "normal") {
      return hasNormal;
    }
    else if (attribName == "texcoord") {
      return hasTexcoord;
    }
    else
      {
	return false;
      }
  }
  
  vector<float> getData(string attribName) const
  {
    vector<float> result;
    stringstream message;
    
    
    if (attribName == "position")
      {
	result.push_back(position.x);
	result.push_back(position.y);
	result.push_back(position.z);
	result.push_back(position.w);
      }
    else if (attribName == "normal")
      {
	result.push_back(normal.x);
	result.push_back(normal.y);
	result.push_back(normal.z);
	result.push_back(normal.w);
      }
    else if (attribName == "texcoord")
      {
	result.push_back(texcoord.x);
	result.push_back(texcoord.y);
	result.push_back(texcoord.z);
	result.push_back(texcoord.w);
      }
    else
      {
	message << "No attribute: " << attribName << " found!";
	throw runtime_error(message.str());
      }
    
    return result;
  }
  
  void setData(string attribName, const vector<float>& data)
  {
    stringstream message;
    
    if (attribName == "position")
      {
	hasPosition = true;
	position = glm::vec4(0,0,0,1);
	switch (data.size()) {
	case 4: position.w = data[3];
	case 3: position.z = data[2];
	case 2: position.y = data[1];
	case 1: position.x = data[0];
	  break;
	default:
	  message << "Too much data for attribute: " << attribName;
	  throw runtime_error(message.str());
	}
      }
    else if (attribName == "normal")
      {
	hasNormal = true;
	normal = glm::vec4(0,0,0,1);
	switch (data.size()) {
	case 4: normal.w = data[3];
	case 3: normal.z = data[2];
	case 2: normal.y = data[1];
	case 1: normal.x = data[0];
	  break;
	default:
	  message << "Too much data for attribute: " << attribName;
	  throw runtime_error(message.str());
	}
      }
    else if (attribName == "texcoord")
      {
	hasTexcoord = true;
	texcoord = glm::vec4(0,0,0,1);
	switch (data.size()) {
	case 4: texcoord.w = data[3];
	case 3: texcoord.z = data[2];
	case 2: texcoord.y = data[1];
	case 1: texcoord.x = data[0];
	  break;
	default:
	  message << "Too much data for attribute: " << attribName;
	  throw runtime_error(message.str());
	}
      }
    else
      {
	message << "Attribute: " << attribName << " unsupported!";
	throw runtime_error(message.str());
      }
  }
  
  vector<string> getAllAttributes()
  {
    vector<string> attributes;
    
    attributes.push_back("position");
    attributes.push_back("normal");
    attributes.push_back("texcoord");
    return attributes;
  }

  void transform(glm::mat4 transform) {
    position = transform*position;
    glm::mat4 inverse = glm::inverse(transform);
    glm::mat4 inversetranspose = glm::transpose(inverse);
    normal = inversetranspose*normal;
  }

  void flipNormal() {
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
  }
  
private:
  glm::vec4 position;
  glm::vec4 normal; 
  glm::vec4 texcoord;
  bool hasPosition = false;
  bool hasNormal = false;
  bool hasTexcoord = false;
};


static std::array<float,4> calculate_plane_from_points(const std::array<const std::reference_wrapper<VertexAttrib>,3> triangle) {

  vector<float> p = triangle[0].get().getData("position");
  vector<float> q = triangle[1].get().getData("position");
  vector<float> r = triangle[2].get().getData("position");
  
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

  
  if(triangle[0].get().hasData("normal")) {
    vector<float> vnormal = triangle[0].get().getData("normal");
    double d = vnormal[0]*normal[0] + vnormal[1]*normal[1] + vnormal[2]*normal[2];
    if(d < -0.000001) {
      normal[0] = -normal[0];
      normal[1] = -normal[1];
      normal[2] = -normal[2];
    }
  }
  
  
  
  
  float d = -normal[0]*p[0] - normal[1]*p[1] - normal[2]*p[2];
  
  return std::array<float,4>{{normal[0], normal[1], normal[2], d}};

}
static std::array<float,4> calculate_plane_from_points(const std::array<VertexAttrib,3> triangle) {
  
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
    if(d < -0.000001) {
      normal[0] = -normal[0];
      normal[1] = -normal[1];
      normal[2] = -normal[2];
    }
  }
  
  
  
  float d = -normal[0]*p[0] - normal[1]*p[1] - normal[2]*p[2];
  
  return std::array<float,4>{{normal[0], normal[1], normal[2], d}};
  
}

static std::array<float,4> calculate_plane_from_points(const VertexAttrib& v1,
						       const VertexAttrib& v2,
						       const VertexAttrib& v3) {
  
  
  vector<float> p = v1.getData("position");
  vector<float> q = v2.getData("position");
  vector<float> r = v3.getData("position");
  
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
  
  
  float d = -normal[0]*p[0] - normal[1]*p[1] - normal[2]*p[2];
  
  return std::array<float,4>{{normal[0], normal[1], normal[2], d}};
  
}


inline ostream& operator<<(ostream& os, const VertexAttrib& t)
{
  const std::vector<float> o = t.getData("position");
  os << o[0] << ", " << o[1] << "," << o[2];
  return os;
}

#endif

