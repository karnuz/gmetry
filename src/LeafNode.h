#ifndef _LEAFNODE_H
#define _LEAFNODE_H

#include <string>
#include "Material.h"
#include "SGNode.h"

//template <typename VertexType> class SGNode;

template <typename VertexType>
class LeafNode : public SGNode<VertexType>
{
  
private:
  string meshName;
  util::Material material;
  float eps = 0.0000001;
  string bspTreeName;
  
public:
  LeafNode(string mname, Scenegraph<VertexType>* graph, string name): SGNode<VertexType>(graph, name)
  {
    this->meshName = mname;
  }
  LeafNode(string mname, string bspName,Scenegraph<VertexType>* graph, string name): SGNode<VertexType>(graph, name)
  {
    this->meshName = mname;
    this->bspTreeName = bspName;
  }
  
  void setMaterial(util::Material mat) {
    this->material = mat;
  }

  util::Material getMaterial() {
    return this->material;
  }

  SGNode<VertexType>* clone() {
    SGNode<VertexType>* newclone = new LeafNode(this->meshName, this->scenegraph, this->name);
    newclone->setMaterial(this->getMaterial());
    return newclone;								   
  }

  void draw(ScenegraphRenderer<VertexType>* context, stack<glm::mat4> modelview) {
    if(this->active) {
      if(this->meshName.size() > 0) {
	context->drawMesh(this->meshName, this->material, modelview.top());
      }
    }
  }

  string getMeshName() {
    return this->meshName;
  }

  HitRecord raycast(glm::vec4 origin, glm::vec4 direction, stack<glm::mat4> modelview) {

    HitRecord h = *(new HitRecord());
    h.setT(-1);
    
    glm::mat4 mv = modelview.top();
    glm::mat4 viewModel = glm::inverse(mv);
    glm::mat4 viewModelT = glm::transpose(viewModel);
    origin = viewModel*origin;
    direction = viewModel*direction;


    
    if(!rayHitMesh(origin,direction)) {
      //   cout << "didn't hit\n";
      return h;
    }

    //point and normal
    std::tuple<glm::vec4, glm::vec4, float> pos_norm = this->scenegraph->raycastTree(meshName,origin, direction);

    auto& r0 = get<0>(pos_norm);
    auto& r1 = get<1>(pos_norm); 
    auto t = get<2>(pos_norm);
    
    h.sethitCoord(r0);
    h.sethitNormal(r1);
    h.setmaterial(this->material);
    h.setT(t);
    
    //    h.set

    // set parameters of hit record h

    return h;
  }


  bool rayHitMesh(glm::vec4 origin, glm::vec4 direction) {
    bool t = this->scenegraph->rayHitMesh(meshName,origin, direction);
    //cout << t << " : ray hit mesh\n";
    bool s = true;
    //cout << s << " :s\n";
    return t;
  }
};

#endif
