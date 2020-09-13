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
#include <ctime>


Scenegraph<VertexAttrib>* myScene() {

  Scenegraph<VertexAttrib>* s = new Scenegraph<VertexAttrib>();

  ifstream in_file;
  util::ObjImporter<VertexAttrib> objImp;
  
  util::PolygonMesh<VertexAttrib>* mesh_a = new util::PolygonMesh<VertexAttrib>();
  in_file.open("../../models/cone.obj");
  *mesh_a = (objImp.importFile(in_file, true));
  mesh_a->cleanMesh();
  mesh_a->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  mesh_a->setPrimitiveSize(3); //3 vertices per polygon
  in_file.close();
  in_file.clear();
  s->addPolygonMesh("mesh_a", mesh_a);

  
  util::PolygonMesh<VertexAttrib>* mesh_b = new util::PolygonMesh<VertexAttrib>();
  in_file.open("../../models/cone.obj");
  *mesh_b = (objImp.importFile(in_file, true));
  mesh_b->cleanMesh();
  mesh_b->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  mesh_b->setPrimitiveSize(3); //3 vertices per polygon
  in_file.close();
  in_file.clear();
  s->addPolygonMesh("mesh_b", mesh_b);
  

  double duration = 0.0;
  std::clock_t start;
  start = std::clock();
  
  BSPTree<VertexAttrib> bsp_a = *(new BSPTree<VertexAttrib>(*mesh_a));
  util::PolygonMesh<VertexAttrib>* meshbsp_a = new util::PolygonMesh<VertexAttrib>();
  meshbsp_a = bsp_a.getMesh();
  meshbsp_a->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  meshbsp_a->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("meshbsp_a", meshbsp_a);

  BSPTree<VertexAttrib> bsp_b = *(new BSPTree<VertexAttrib>(*mesh_b));

  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  cout<< "Time taken in creating mesh: "<< duration <<"\n\n";

  
  bsp_b = *bsp_b.transformTree(glm::translate(glm::mat4(1.0f),glm::vec3(0.0,0.4,0.0)));
  util::PolygonMesh<VertexAttrib>* meshbsp_b = new util::PolygonMesh<VertexAttrib>();
  meshbsp_b = bsp_b.getMesh();
  meshbsp_b->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  meshbsp_b->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("meshbsp_b", meshbsp_b);
  
  cout << "clipping now \n";

  start = std::clock();

  BSPTree<VertexAttrib> bsp_c = *(bsp_a.subtractionbsp(&bsp_b));
  BSPTree<VertexAttrib> bsp_d = *(bsp_a.unionbsp(&bsp_b));
  BSPTree<VertexAttrib> bsp_e = *(bsp_a.intersectionbsp(&bsp_b));
  
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  cout<< "Time taken in clipping mesh: "<< duration <<"\n\n";

  util::PolygonMesh<VertexAttrib>* meshbsp_c;
  meshbsp_c = bsp_c.getMesh();
  meshbsp_c->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  meshbsp_c->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("meshbsp_csg", meshbsp_c);

  util::PolygonMesh<VertexAttrib>* meshbsp_d;
  meshbsp_d = bsp_d.getMesh();
  meshbsp_d->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  meshbsp_d->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("meshbsp_csg2", meshbsp_d);

  util::PolygonMesh<VertexAttrib>* meshbsp_e;
  meshbsp_e = bsp_e.getMesh();
  meshbsp_e->setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
  meshbsp_e->setPrimitiveSize(3); //3 vertices per polygon
  s->addPolygonMesh("meshbsp_csg3", meshbsp_e);
  
  LeafNode<VertexAttrib>* l1 = new LeafNode<VertexAttrib>("meshbsp_a",s,"leaf1");
  LeafNode<VertexAttrib>* l2 = new LeafNode<VertexAttrib>("meshbsp_b",s,"leaf2");
  LeafNode<VertexAttrib>* l3 = new LeafNode<VertexAttrib>("meshbsp_csg",s,"leaf3");
  LeafNode<VertexAttrib>* l4 = new LeafNode<VertexAttrib>("meshbsp_csg2",s,"leaf4");
  LeafNode<VertexAttrib>* l5 = new LeafNode<VertexAttrib>("meshbsp_csg3",s,"leaf5");

  
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
  m3->setAmbient(0.2,0.0,0.5);
  m3->setDiffuse(0.2,0.0,0.5);
  m3->setSpecular(0.2,0.0,0.5);
  m3->setShininess(10.0);

  util::Material* m4 = new util::Material();
  m4->setAmbient(0.7,0.1,0.8);
  m4->setDiffuse(0.7,0.1,0.8);
  m4->setSpecular(0.7,0.1,0.8);
  m4->setShininess(10.0);

  util::Material* m5 = new util::Material();
  m5->setAmbient(0.2,0.5,0.5);
  m5->setDiffuse(0.2,0.5,0.5);
  m5->setSpecular(0.2,0.5,0.5);
  m5->setShininess(10.0);

  l1->setMaterial(*m2);
  l2->setMaterial(*m3);
  l3->setMaterial(*m1);
  l4->setMaterial(*m3);
  l5->setMaterial(*m2);
  
  
  GroupNode<VertexAttrib>* g1 = new GroupNode<VertexAttrib>(s, "group1");
  
  s->addNode("leaf1", l1);
  s->addNode("group1", g1);
  s->addNode("leaf2", l2);
  s->addNode("leaf3", l3);
  s->addNode("leaf4", l4);
  s->addNode("leaf5", l5);
  
  g1->addChild(l1);
  g1->addChild(l2);
  g1->addChild(l3);
  g1->addChild(l4);
  g1->addChild(l5);

  util::Light l;
  l.setAmbient(1.0,1.0,1.0);
  l.setDiffuse(1.0,1.0,1.0);
  l.setSpecular(1.0,1.0,1.0);
  l.setPosition(-2.0,0.0,0.0);
  l.setSpotDirection(1.0,0.0,0.0);
  l.setSpotAngle(90.0);

  l1->addLight(l);
    
  s->makeScenegraph(g1);



  
  return s;
}


#endif
