#ifndef GROUPNODE_H
#define GROUPNODE_H

#include <string>
#include "SGNode.h"


template <typename VertexType>
class GroupNode : public SGNode<VertexType>
{
private:
  vector<SGNode<VertexType>*> children;
public:
  GroupNode(Scenegraph<VertexType>* graph, string name): SGNode<VertexType>(graph, name)
  {
  }

  SGNode<VertexType> *getNode(string name) {
    if(name == this->name) {
      return this;
    }
    SGNode<VertexType> *answer = NULL;
    int i = 0;
    while(i < children.size() && answer == NULL){
      answer = this->children[i]->getNode(name);
      i++;
    }
    return answer;
  }


  void setScenegraph(Scenegraph<VertexType>* graph) {
    SGNode<VertexType>::setScenegraph(graph);
    for(auto c: children) {
      c->setScenegraph(graph);
    }
  }

  void draw(ScenegraphRenderer<VertexType>* context, stack<glm::mat4> modelView) {
    for(auto c: children) {
      c->draw(context, modelView);
    }
  }

  vector<util::Light> collectLights(stack<glm::mat4> modelView) {
    vector<util::Light> lt = SGNode<VertexType>::collectLights(modelView);
    //cout << "child size: " << children.size() << "\n";
    for(auto c: children) {
      //      cout << "going to child\n";
      vector<util::Light> m = c->collectLights(modelView);
      lt.insert(lt.end(),m.begin(),m.end());
    }
    return lt;
  }

  SGNode<VertexType>* clone() {
    GroupNode<VertexType> *n = new GroupNode<VertexType>(this->scenegraph, this->name);
    vector<SGNode<VertexType>*> newchildren;
    for(auto c: children) {
      n->addChild(c->clone());
    }
    return n;
  }

  void addChild(SGNode<VertexType>* child) {
    children.push_back(child);
    child->setParent(this);
  }

  vector<SGNode<VertexType>*> getChildren() {
    return this->children;
  }

  void addLight(util::Light l) {
    this->lights.push_back(l);
  }


  HitRecord raycast(glm::vec4 origin, glm::vec4 direction, stack<glm::mat4> modelview) {
    std::vector<HitRecord> newh;

    for (auto c: children) {
      HitRecord h = c->raycast(origin, direction, modelview);
      if(h.getT() >=0 ) {
	newh.push_back(h);
      }
    }

    if(newh.size() == 0) {
      HitRecord h = *(new HitRecord());
      h.setT(-1);
      return h;
    }

    HitRecord r = newh[0];
    for(HitRecord h: newh) {
      if(h.getT() < r.getT()) {
	r = h;
      }
    }
    return r;
  }
  
};

#endif // GROUPNODE_H
