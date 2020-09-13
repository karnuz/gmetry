#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include <map>
#include <string>
#include <vector>
//#include "SGNode.h"
#include <stack>
#include "Light.h"
#include "ScenegraphRenderer.h"
#include "bspTree.h"

using namespace std;

template <typename VertexType> class SGNode;

template <typename VertexType>
class Scenegraph
{
 protected:
  SGNode<VertexType>* root = NULL;
  std::map<string, util::PolygonMesh<VertexType>*> meshes;
  std::map<string, BSPTree<VertexType>*> bspTrees;
  std::map<string, SGNode<VertexType>*> nodes;
  std::map<string, string> textures;
  ScenegraphRenderer<VertexType>* renderer = NULL;
  bool bspmesh = false;
  
 public:
  // no constructor needed

  void dispose() {
    this->renderer->dispose();
  }

  void setRenderer(ScenegraphRenderer<VertexType>* renderer) {
    this->renderer = renderer;

    cout << "adding mesh to renderer head\n";

    for(auto const& element : meshes) {
      cout << "adding mesh to renderer\n";
      this->renderer->addMesh(element.first, *element.second);
    }
    //    for(auto const& [texturename, path] : textures) {
    //  this->renderer->addTexture(texturename, path);
    //}
  }

  void makeScenegraph(SGNode<VertexType>* node) {
    this->root = node;
    this->root->setScenegraph(this);
  }

  void draw(stack<glm::mat4> modelView) {
    if ((this->root != NULL) && (this->renderer != NULL)) {
      this->renderer->draw(this->root, modelView);
    }
  }

  void renderLights(stack<glm::mat4> modelview) {
    if ((this->root != NULL) && (this->renderer != NULL)) {
      vector<util::Light> lights = this->collectLights(modelview);
      this->renderer->renderLights(lights);
    } else {
      return;
    }
  }


  vector<util::Light> collectLights(stack<glm::mat4> modelview) {
    if(this->root != NULL) {
      //      cout << "collecting lights\n";
      return this->root->collectLights(modelview);
    }
    else {
      return vector<util::Light>();
    }
  }

  void addPolygonMesh(string meshname, util::PolygonMesh<VertexType>* mesh, bool bsp = false) {
    this->meshes.insert({meshname,  mesh});
    if(bsp == true) {
      BSPTree<VertexAttrib>* bsp_a = new BSPTree<VertexAttrib>(*mesh);
      this->addBSPTree(meshname, bsp_a);
    }
  }

  void addBSPTree(string bspname, BSPTree<VertexType>* tree) {
    this->bspTrees.insert({bspname,  tree});
  }

  void animate(float time) {
    // lets see
  }

  void toggleNode(string name) {
    this->nodes[name]->toggleActive();
  }
  
  void addNode(string name, SGNode<VertexType>* node) {
    this->nodes[name] =  node;
  }

  SGNode<VertexType>* getRoot() {
    return this->root;
  }

  map<string, util::PolygonMesh<VertexType>*> getPolygonMeshes() {
    return this->meshes;
  }

  map<string, BSPTree<VertexType>*> getBSPTrees() {
    return this->bspTrees;
  }

  map<string, SGNode<VertexType>*> getNodes() {
    return this->nodes;
  }

  map<string, string> getTextures() {
    return this->textures;
  }

  void addTexture(string texturename, string path) {
    this->textures.set(texturename, path);
  }

  void setTexturemap(map<string,string> nameurls) {
    this->textures = nameurls;
  }

  HitRecord raycast(glm::vec4 origin, glm::vec4 direction, stack<glm::mat4> modelview) {
    HitRecord h = this->root->raycast(origin, direction, modelview);
    return h;
  }

  std::tuple<glm::vec4, glm::vec4, float> raycastTree(string name, glm::vec4 origin, glm::vec4 direction) {
    auto r = bspTrees[name]->raycast(origin, direction);
    return r;
  }

  bool rayHitMesh(string meshName, glm::vec4 origin, glm::vec4 direction) {
    return this->meshes[meshName]->rayHitMesh(origin,direction);
  }
  
};
#endif
