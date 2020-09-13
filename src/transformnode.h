#ifndef _TRANSFORMNODE_H
#define _TRANSFORMNODE_H

#include "SGNode.h"
#include <string>

template <typename VertexType>
class TransformNode : public SGNode<VertexType>
{
protected:
  glm::mat4 transform;
  glm::mat4 animationtransform;
  SGNode<VertexType>* child = NULL;
  
public:
  TransformNode(Scenegraph<VertexType>* graph, string name): SGNode<VertexType>(graph, name)
  {
  }


  void setScenegraph(Scenegraph<VertexType>* graph) {
    SGNode<VertexType>::setScenegraph(graph);
    if(this->child != NULL) {
      this->child->setScenegraph(graph);
    }
  }

  void draw(ScenegraphRenderer<VertexType>* context, stack<glm::mat4> modelview) {

    glm::mat4 last = modelview.top();
    last = last*animationtransform;
    last = last*transform;
    modelview.push(last);

    if (this->child != NULL) {
      this->child->draw(context, modelview);
    }
  }

  vector<util::Light> collectLights(stack<glm::mat4> modelview) {
    vector<util::Light> lt = SGNode<VertexType>::collectLights(modelview);

    glm::mat4 last = modelview.top();
    last = last*animationtransform;
    last = last*transform;
    modelview.push(last);
    
    if(this->child != NULL) {
      vector<util::Light> ltchild = this->child->collectLights(modelview);
      lt.insert(lt.end(), ltchild.begin(), ltchild.end());
    }

    modelview.pop();

    return lt;
    
  }

  SGNode<VertexType>* clone() {
    SGNode<VertexType>* n;
    if(this->child != NULL) {
      n = this->child->clone();
    } else {
      n = NULL;
    }

    TransformNode<VertexType>* newtransform = new TransformNode<VertexType>(this->scenegraph, this->name);
    newtransform->setTransform(this->transform);
    newtransform->setAnimationTransform(this->animationtransform);

    if (n != NULL) {
      newtransform->addChild(n);
    }
    
    return newtransform;
  }

  void setAnimationTransform(glm::mat4 at) {
    this->animationtransform = at;
  }

  glm::mat4 getTransform() {
    return this->transform;
  }

  void setTransform(glm::mat4 t) {
    this->transform = t;
  }

  glm::mat4 getAnimationTransform() {
    return this->animationTransform;
  }


  SGNode<VertexType>* getNode(string name) {
    if(name == this->name) {
      return this;
    }
    if(this->child != NULL) {
      return this->child->getNode(name);
    }
    return NULL;
  }

  void addChild(SGNode<VertexType>* child) {
    if(this->child != NULL) {
      throw std::runtime_error("transformation node already has a child\n");
    }
    this->child = child;
    this->child->setParent(this);
  }

  HitRecord raycast(glm::vec4 origin, glm::vec4 direction, stack<glm::mat4> modelview) {
    glm::mat4 last = modelview.top();
    last = last*animationtransform;
    last = last*transform;
    modelview.push(last);

    HitRecord h = *(new HitRecord());
    h.setT(-1);
    if(this->child != NULL) {
      h = this->child->raycast(origin, direction, modelview);
    }
    modelview.pop();
    return h;
  }


  
};

#endif // GROUPNODE_H
