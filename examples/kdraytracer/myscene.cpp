
#include "myscene.h"
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
  in_file.open("../../models/torus.obj");
  *mesh_a = (objImp.importFile(in_file, false));
  mesh_a->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  mesh_a->setPrimitiveSize(3); //3 vertices per polygon
  in_file.close();
  in_file.clear();
  s->addPolygonMesh("mesh_a", mesh_a, true);

  util::PolygonMesh<VertexAttrib>* mesh_b = new util::PolygonMesh<VertexAttrib>();
  in_file.open("../../models/cone.obj");
  *mesh_b = (objImp.importFile(in_file, false));
  mesh_b->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  mesh_b->setPrimitiveSize(3); //3 vertices per polygon
  in_file.close();
  in_file.clear();
  s->addPolygonMesh("mesh_b", mesh_b, true);

  util::PolygonMesh<VertexAttrib>* mesh_c = new util::PolygonMesh<VertexAttrib>();
  in_file.open("../../models/aeroplane.obj");
  *mesh_c = (objImp.importFile(in_file, false));
  mesh_c->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  mesh_c->setPrimitiveSize(3); //3 vertices per polygon
  in_file.close();
  in_file.clear();
  s->addPolygonMesh("mesh_c", mesh_c, true);

  LeafNode<VertexAttrib>* l1 = new LeafNode<VertexAttrib>("mesh_a",s,"leaf1");
  LeafNode<VertexAttrib>* l2 = new LeafNode<VertexAttrib>("mesh_a",s,"leaf2");
  LeafNode<VertexAttrib>* l3 = new LeafNode<VertexAttrib>("mesh_b",s,"leaf3");
  LeafNode<VertexAttrib>* l4 = new LeafNode<VertexAttrib>("mesh_c",s,"leaf4");

  TransformNode<VertexAttrib>* t1 = new TransformNode<VertexAttrib>(s,"trans1");
  glm::mat4 trans1a = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -4.0f, 0.0f));
  glm::mat4 trans1b = glm::scale(glm::mat4(1.f), glm::vec3(3.0f, 1.0f, 3.0f));
  glm::mat4 trans1 = trans1a*trans1b;
  t1->setTransform(trans1);

  TransformNode<VertexAttrib>* t2 = new TransformNode<VertexAttrib>(s,"trans2");
  glm::mat4 trans2a = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 4.0f, 0.f));
  glm::mat4 trans2b = glm::scale(glm::mat4(1.f), glm::vec3(3.0f, 1.0f, 3.0f));
  glm::mat4 trans2 = trans2a*trans2b;
  t2->setTransform(trans2);

  TransformNode<VertexAttrib>* t3 = new TransformNode<VertexAttrib>(s,"trans3");
  glm::mat4 trans3a = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -25.0f, 0.f));
  glm::mat4 trans3b = glm::scale(glm::mat4(1.f), glm::vec3(3.0f, 12.0f, 3.0f));
  glm::mat4 trans3 = trans3a*trans3b;
  t3->setTransform(trans3);

  
  util::Material* m1 = new util::Material();
  m1->setAmbient(0.9,0.0,0.0);
  m1->setDiffuse(0.9,0.0,0.0);
  m1->setSpecular(0.9,0.0,0.0);
  m1->setShininess(10.0);

  util::Material* m2 = new util::Material();
  m2->setAmbient(0.9,0.0,0.0);
  m2->setDiffuse(0.9,0.0,0.0);
  m2->setSpecular(0.9,0.0,0.0);
  m2->setShininess(10.0);

  util::Material* m3 = new util::Material();
  m3->setAmbient(0.9,0.0,0.0);
  m3->setDiffuse(0.9,0.0,0.0);
  m3->setSpecular(0.9,0.0,0.0);
  m3->setShininess(10.0);

  util::Material* m4 = new util::Material();
  m4->setAmbient(0.2,0.2,0.8);
  m4->setDiffuse(0.2,0.2,0.8);
  m4->setSpecular(0.2,0.2,0.8);
  m4->setShininess(10.0);

  l1->setMaterial(*m1);
  l2->setMaterial(*m2);
  l3->setMaterial(*m3);
  l4->setMaterial(*m4);

  
  GroupNode<VertexAttrib>* g1 = new GroupNode<VertexAttrib>(s, "group1");
  GroupNode<VertexAttrib>* g2 = new GroupNode<VertexAttrib>(s, "group2");
  
  s->addNode("group1", g1);
  s->addNode("leaf1", l1);
  s->addNode("leaf2", l2);
  s->addNode("leaf3", l3);
  s->addNode("leaf3", l4);
  s->addNode("trans1", t1);
  s->addNode("trans2", t2);
  s->addNode("trans3", t3);
  
  g1->addChild(t1);
  g1->addChild(t2);
  g1->addChild(t3);
  t1->addChild(l1);
  t2->addChild(l2);
  t3->addChild(l3);
  g1->addChild(l4);

  util::Light l;
  l.setAmbient(1.0,1.0,1.0);
  l.setDiffuse(1.0,1.0,1.0);
  l.setSpecular(1.0,1.0,1.0);
  l.setPosition(-5.0,0.0,0.0);
  l.setSpotDirection(1.0,0.0,0.0);
  l.setSpotAngle(120.0);

  l1->addLight(l);
    
  s->makeScenegraph(g1);

    
  return s;
}


