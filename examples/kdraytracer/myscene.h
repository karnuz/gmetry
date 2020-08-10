#ifndef MYSCENE_H
#define MYSCENE_H


#include "View.h"
#include "VertexAttrib.h"
#include "PolygonMesh.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include <string>
#include "ObjImporter.h"
#include "bspTree.h"
#include "LeafNode.h"
#include "Groupnode.h"
#include "transformnode.h"


Scenegraph<VertexAttrib>* myScene() {

  Scenegraph<VertexAttrib>* s = new Scenegraph<VertexAttrib>();

  ifstream in_file;
  util::ObjImporter<VertexAttrib> objImp;
    
  util::PolygonMesh<VertexAttrib>* mesh_a = new util::PolygonMesh<VertexAttrib>();
  in_file.open("/Users/himanshusharma/karnuz/AlgoDS/geometry/models/sphere.obj");
  *mesh_a = (objImp.importFile(in_file, true));
  mesh_a->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  mesh_a->setPrimitiveSize(3); //3 vertices per polygon
  in_file.close();
  in_file.clear();
  s->addPolygonMesh("mesh_a", mesh_a);


  util::PolygonMesh<VertexAttrib>* mesh_a2 = new util::PolygonMesh<VertexAttrib>();
  *mesh_a2 = mesh_a->getTransformedMesh(glm::translate(glm::mat4(1.0f),glm::vec3(0.6,0,0)));
  mesh_a2->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  mesh_a2->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("mesh_a2", mesh_a2);

  
  BSPTree<VertexAttrib> bsp_a = *(new BSPTree<VertexAttrib>(*mesh_a));
  util::PolygonMesh<VertexAttrib>* meshbsp_a = new util::PolygonMesh<VertexAttrib>();
  meshbsp_a = bsp_a.getMesh();
  meshbsp_a->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  meshbsp_a->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("meshbsp_a", meshbsp_a);


  
  BSPTree<VertexAttrib> bsp_a2 = *bsp_a.transformTree(glm::translate(glm::mat4(1.0f),glm::vec3(0.0,0,0.6)));
  util::PolygonMesh<VertexAttrib>* meshbsp_a2 = new util::PolygonMesh<VertexAttrib>();
  meshbsp_a2 = bsp_a2.getMesh();
  meshbsp_a2->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  meshbsp_a2->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("meshbsp_a2", meshbsp_a2);

  /*
  BSPTree<VertexAttrib> bsp_a2 = *(new BSPTree<VertexAttrib>(*mesh_a2));
  util::PolygonMesh<VertexAttrib>* meshbsp_a2 = new util::PolygonMesh<VertexAttrib>();
  meshbsp_a2 = bsp_a2.getMesh();
  meshbsp_a2->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  meshbsp_a2->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("meshbsp_a2", meshbsp_a2);
  */

  /*
  BSPTree<VertexAttrib> bsp_c = *(bsp_a2.clipTo(&bsp_a));
  util::PolygonMesh<VertexAttrib>* meshbsp_c;
  meshbsp_c = bsp_c.getMesh();
  meshbsp_c->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  meshbsp_c->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("meshbsp_csg", meshbsp_c);
  */
  
  LeafNode<VertexAttrib>* l1 = new LeafNode<VertexAttrib>("meshbsp_a",s,"leaf1");
  LeafNode<VertexAttrib>* l2 = new LeafNode<VertexAttrib>("meshbsp_a2",s,"leaf2");
  //LeafNode<VertexAttrib>* l3 = new LeafNode<VertexAttrib>("meshbsp_csg",s,"leaf3");


  
  util::Material* m1 = new util::Material();
  m1->setAmbient(0.5,0.0,0.0);
  m1->setDiffuse(0.5,0.0,0.5);
  m1->setSpecular(0.5,0.0,0.5);
  m1->setShininess(10.0);
  util::Material* m2 = new util::Material();
  m2->setAmbient(0.0,0.5,0.0);
  m2->setDiffuse(0.0,0.5,0.5);
  m2->setSpecular(0.0,0.5,0.5);
  m2->setShininess(10.0);
  util::Material* m3 = new util::Material();
  m3->setAmbient(0.5,0.0,0.5);
  m3->setDiffuse(0.5,0.0,0.5);
  m3->setSpecular(0.5,0.0,0.5);
  m3->setShininess(10.0);

  l1->setMaterial(*m1);
  l2->setMaterial(*m2);
  //l3->setMaterial(*m3);
  
  
  GroupNode<VertexAttrib>* g1 = new GroupNode<VertexAttrib>(s, "group1");
  
  s->addNode("leaf1", l1);
  s->addNode("group1", g1);
  s->addNode("leaf2", l2);
  //s->addNode("leaf3", l3);
  
  g1->addChild(l1);
  g1->addChild(l2);
  //g1->addChild(l3);


  util::Light l;
  l.setAmbient(1.0,1.0,1.0);
  l.setDiffuse(1.0,1.0,1.0);
  l.setSpecular(1.0,1.0,1.0);
  l.setPosition(-2.0,-1.0,0.0);
  l.setSpotDirection(1.0,0.0,0.0);
  l.setSpotAngle(90.0);

  l1->addLight(l);
    
  s->makeScenegraph(g1);



  
  return s;
}


#endif
