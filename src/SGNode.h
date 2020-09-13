#ifndef SGNODE_H
#define SGNODE_H

//#include "Scenegraph.h"
#include <string>
#include "Light.h"
#include <vector>
#include "IVertexData.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include "ScenegraphRenderer.h"
#include <Material.h>
#include <stack>
#include "HitRecord.h"

using namespace std;

template <typename VertexType> class ScenegraphRenderer;
template <typename VertexType> class Scenegraph;

template <typename VertexType>
class SGNode
{
public:
  SGNode(Scenegraph<VertexType>* graph, string name) {
    this->parent = NULL;
    this->scenegraph = graph;
    this->setName(name);
    this->active = true;
  }
  
  SGNode* getNode(string name) {
    if(this->name == name) {
      return this;
    }
    return NULL;
  }
  
  void setParent(SGNode* parent) {
    this->parent = parent;
  }
  
  void setScenegraph(Scenegraph<VertexType>* graph) {
    this->scenegraph = graph;
    graph->addNode(this->name,this);
  }

  void setName(string name) {
    this->name = name;
  }
  
  string getName() {
    return this->name;
  }
  
  virtual void draw(ScenegraphRenderer<VertexType>* context, stack<glm::mat4> modelView) = 0;
  //virtual HitRecord raycast(vec4 origin, vec4 direction, Stack<mat4> modelView) = 0;
  virtual SGNode* clone() = 0;
  virtual HitRecord raycast(glm::vec4 origin, glm::vec4 direction, stack<glm::mat4> modelview) =0;
  
  virtual void setTransform(glm::mat4 transform) {
    throw runtime_error("setTransform Not Supported");
  }
  
  virtual void setAnimationTransform(glm::mat4 transform) {
    throw runtime_error("set AnimationTransform Not Supported");
  }
  
  virtual void setMaterial(util::Material mat) {
    throw runtime_error("set Material Not Supported");
  }
  
  virtual util::Material getMaterial() {
        throw runtime_error("Not Supported");
  }
  
  virtual vector<util::Light> collectLights(stack<glm::mat4> modelView) {
    vector<util::Light> lt;
    if((this->lights).size() > 0) {
      for(auto l : this->lights) {
	util::Light m = *(new util::Light());
	
	m.setAmbient(l.getAmbient());
	m.setDiffuse(l.getDiffuse());
	m.setSpecular(l.getSpecular());
	glm::vec4 p = l.getPosition();
	p = modelView.top() * p;
	m.setPosition(p);
	glm::vec4 d = l.getSpotDirection();
	d = modelView.top() * d;
	m.setSpotDirection(d[0],d[1],d[2]);
	m.setSpotAngle(l.getSpotCutoff());
	lt.push_back(m);
      }      
    }
    return lt;
  }
  
  void addLight(util::Light l) {
    this->lights.push_back(l);
    }
  
  void toggleActive() {
    this->active = !this->active;
  }
  
  
protected:
  string name;
  bool active;
  vector<util::Light> lights;
  SGNode* parent;
  Scenegraph<VertexType>* scenegraph;
};

#endif // SGNODE_H
