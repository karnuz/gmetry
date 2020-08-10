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


using namespace std;

View::View()
{
    WINDOW_WIDTH = WINDOW_HEIGHT = 0;
    obj = NULL;
}

View::~View()
{
    if (obj!=NULL)
        delete obj;
}


void View::init(util::OpenGLFunctions& gl)
{

    //create the shader program
    program.createProgram(gl,
                          string("shaders/default.vert"),
                          string("shaders/default.frag"));

    //assuming it got created, get all the shader variables that it uses
    //so we can initialize them at some point
    shaderLocations = program.getAllShaderVariables(gl);
    s = new Scenegraph<VertexAttrib>();

    map<string,string> shaderVarsToVertexAttribs;
    shaderVarsToVertexAttribs["vPosition"]="position";

    
    //now we create a polygon mesh object. Think of this as literally a mesh
    
    ifstream in_file;
    util::ObjImporter<VertexAttrib> objImp;

    
    util::PolygonMesh<VertexAttrib> mesh_a;
    in_file.open("/Users/himanshusharma/karnuz/AlgoDS/geometry/models/torus.obj");
    mesh_a = objImp.importFile(in_file, true);
    mesh_a.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh_a.setPrimitiveSize(3); //3 vertices per polygon
    in_file.close();
    in_file.clear();

    /*
    util::PolygonMesh<VertexAttrib> mesh_a2 = mesh_a.getTransformedMesh(glm::translate(glm::mat4(1.0f),glm::vec3(0.5,0,0))); 
    mesh_a2.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh_a2.setPrimitiveSize(3); //3 vertices per polygon
    */
    

    
    
    util::PolygonMesh<VertexAttrib> mesh_b;    
    in_file.open("/Users/himanshusharma/karnuz/AlgoDS/geometry/models/box.obj");
    mesh_b = objImp.importFile(in_file, false);
    mesh_b.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh_b.setPrimitiveSize(3); //3 vertices per polygon
    in_file.close();
    in_file.clear();
    
    
    cout << "mesh a1 started\n\n\n";
    
    BSPTree bsp_a(mesh_a);
    util::PolygonMesh<VertexAttrib> meshbsp_a;
    meshbsp_a = bsp_a.getMesh();
    meshbsp_a.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_a.setPrimitiveSize(3); //3 vertices per polygon
    s->addPolygonMesh("aeroplane", &meshbsp_a);


    
    cout << "bsp_a done\n";


    
    //BSPTree bsp_a2(mesh_a2);
    BSPTree<VertexAttrib> bsp_a2 = *bsp_a.transformTree(glm::translate(glm::mat4(1.0f),glm::vec3(0.5,0,0)));
    util::PolygonMesh<VertexAttrib> meshbsp_a2;
    meshbsp_a2 = bsp_a2.getMesh();
    meshbsp_a2.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_a2.setPrimitiveSize(3); //3 vertices per polygon
    s->addPolygonMesh("aeroplanetrans", &meshbsp_a2);
    

    cout << "bsp_a2 done\n";
    
    BSPTree bsp_b(mesh_b);
    util::PolygonMesh<VertexAttrib> meshbsp_b;
    meshbsp_b = bsp_b.getMesh();
    meshbsp_b.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_b.setPrimitiveSize(3); //3 vertices per polygon
    s->addPolygonMesh("cylinder", &meshbsp_b);

    cout << "bsp_b done\n";

    
    BSPTree<VertexAttrib> bsp_c = *(bsp_a2.clipTo(&bsp_a));
    util::PolygonMesh<VertexAttrib> meshbsp_c;
    meshbsp_c = bsp_c.getMesh();
    meshbsp_c.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_c.setPrimitiveSize(3); //3 vertices per polygon
    s->addPolygonMesh("csg", &meshbsp_c);
    
    cout << "csg done\n";

    
    LeafNode<VertexAttrib>* l1 = new LeafNode<VertexAttrib>("aeroplane",s,"leaf1");
    LeafNode<VertexAttrib>* l2 = new LeafNode<VertexAttrib>("aeroplanetrans",s,"leaf2");
    LeafNode<VertexAttrib>* l3 = new LeafNode<VertexAttrib>("csg",s,"leaf3");
    //    LeafNode<VertexAttrib>* l4 = new LeafNode<VertexAttrib>("cone",s,"leaf4");
    
    GroupNode<VertexAttrib>* g1 = new GroupNode<VertexAttrib>(s, "group1");
    GroupNode<VertexAttrib>* g2 = new GroupNode<VertexAttrib>(s, "group2");

    TransformNode<VertexAttrib>* t1 = new TransformNode<VertexAttrib>(s, "transform1");
    TransformNode<VertexAttrib>* t2 = new TransformNode<VertexAttrib>(s, "transform2");
    TransformNode<VertexAttrib>* t3 = new TransformNode<VertexAttrib>(s, "transform3");
    TransformNode<VertexAttrib>* t4 = new TransformNode<VertexAttrib>(s, "transform4");

    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));
    t2->setTransform(m);
    
    s->addNode("leaf1", l1);
    s->addNode("leaf2", l2);
    s->addNode("leaf3", l3);
    s->addNode("group1", g1);
    s->addNode("group2", g2);
    s->addNode("transform1", t1);
    s->addNode("transform2", t2);
    s->addNode("transform3", t3);
    
    g1->addChild(l1);
    g1->addChild(l2);
    g1->addChild(l3);
    //    g1->addChild(l3);
    //t1->addChild(l2);
    //g1->addChild(g2);
    //g2->addChild(t2);
    //t2->addChild(l3);
    //g2->addChild(l4);

    util::Light l;
    l.setAmbient(1,1,0);
    l.setDiffuse(1,1,0);
    l.setSpecular(1,1,0);
    l.setPosition(-2,0,0);
    l.setSpotDirection(1,0,0);
    l.setSpotAngle(1);
    
    //g1->addLight(l);

    /*
    s->addNode("leaf1", l1);
    s->addNode("leaf2", l2);
    s->addNode("leaf3", l3);
    //    s->addNode("leaf4", l4);
    s->addNode("group1", g1);
    s->addNode("group2", g2);
    s->addNode("transform1", t1);
    //    s->addNode("transform2", t2);
    s->addNode("transform3", t3);
    s->addNode("transform4", t4);

    */
    
    s->makeScenegraph(g1);
    
    sr = new ScenegraphRenderer<VertexAttrib>(gl, shaderLocations, shaderVarsToVertexAttribs, program);
    
    s->setRenderer(sr);

    cout << "reached hereeeeeeeeeeeasdfsdf\n";
    
    //we will color this square red
    color = glm::vec4(1.0f,0.5f,1.0f,1.0f);

    proj = glm::perspective(1.5f, 1.0f, 0.1f, 10000.0f);
    modelview = glm::lookAt(glm::vec3(1.5f, 1.0f, 1.5f),
    //modelview = glm::lookAt(glm::vec3(0.75f, 0.5f, 0.75f), 
  		   glm::vec3(0.0f, 0.0f, 0.0f), 
  		   glm::vec3(0.0f, 0.1f, 0.0f));

}



void View::init_a(util::OpenGLFunctions& gl)
{

    //create the shader program
    program.createProgram(gl,
                          string("shaders/default.vert"),
                          string("shaders/default.frag"));

    //assuming it got created, get all the shader variables that it uses
    //so we can initialize them at some point
    shaderLocations = program.getAllShaderVariables(gl);

    map<string,string> shaderVarsToVertexAttribs;
    shaderVarsToVertexAttribs["vPosition"]="position";

    
    vector<glm::vec4> positions;
    /*
    positions.push_back(glm::vec4(-1.0f,-1.0f,0.0f,1.0f));
    positions.push_back(glm::vec4( 1.0f,-1.0f,0.0f,1.0f));
    positions.push_back(glm::vec4( 1.0f, 1.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(-1.0f, 1.0f,0.0f,1.0f));
    
    positions.push_back(glm::vec4(-1.0f,0.0f,-1.0f,1.0f));
    positions.push_back(glm::vec4(-1.0f,0.0f,1.0f,1.0f));
    positions.push_back(glm::vec4(1.0f,0.0f,1.0f,1.0f));
    positions.push_back(glm::vec4(1.0f,0.0f,-1.0f,1.0f));
    */

    positions.push_back(glm::vec4(0.0f,0.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(0.0f,2.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(0.0f,0.0f,2.0f,1.0f));
    positions.push_back(glm::vec4(0.0f,2.0f,2.0f,1.0f));

    positions.push_back(glm::vec4(1.0f,0.0f,1.0f,1.0f));
    positions.push_back(glm::vec4(1.0f,2.0f,1.0f,1.0f));
    positions.push_back(glm::vec4(1.0f,0.0f,2.0f,1.0f));
    positions.push_back(glm::vec4(1.0f,2.0f,2.0f,1.0f));

    positions.push_back(glm::vec4(2.0f,0.0f,1.0f,1.0f));
    positions.push_back(glm::vec4(2.0f,2.0f,1.0f,1.0f));
    positions.push_back(glm::vec4(2.0f,0.0f,2.0f,1.0f));
    positions.push_back(glm::vec4(2.0f,2.0f,2.0f,1.0f));
    
    positions.push_back(glm::vec4(4.0f,0.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(4.0f,2.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(4.0f,0.0f,2.0f,1.0f));
    positions.push_back(glm::vec4(4.0f,2.0f,2.0f,1.0f));


    //set up vertex attributes (in this case we have only position)
    vector<VertexAttrib> vertexData;
    for (unsigned int i=0;i<positions.size();i++) {
        VertexAttrib v;
        vector<float> data;

        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        data.push_back(positions[i].w);
        v.setData("position",data);

        vertexData.push_back(v);
    }



    /*
    We now generate a series of indices.
    These indices will be for the above list
    of vertices. For example we want to use
    the above list to draw triangles.
    The first triangle will be created from
    vertex numbers 0, 1 and 2 in the list above
    (indices begin at 0 like arrays). The second
    triangle will be created from vertex numbers
    0, 2 and 3. Therefore we will create a list
    of indices {0,1,2,0,2,3}.

    What is the advantage of having a second
    list? Vertices are often shared between
    triangles, and having a separate list of
    indices avoids duplication of vertex data.
    In other words, we would have the following options:
    (a) no indices: 2 triangles = 6 vertices = 24 floats
        (96 bytes @4 bytes/float)
    (b) with indices: 4 vertices = 16 floats + 6 indices (integers)
        (64+24=88 bytes @ 4 bytes/float and 4 bytes/int assumed)
     */
    vector<unsigned int> indices;

    /*

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(0); //vertex 0 is shared
    indices.push_back(2); //vertex 2 is shared
    indices.push_back(3);


    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);

    indices.push_back(4);
    indices.push_back(6);
    indices.push_back(7);

    */

    // here
    
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(1);

    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(1);

    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(2);
    
    indices.push_back(2);
    indices.push_back(7);
    indices.push_back(3);

    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);
    
    indices.push_back(6);
    indices.push_back(5);
    indices.push_back(7);
    
    indices.push_back(4);
    indices.push_back(8);
    indices.push_back(5);

    indices.push_back(5);
    indices.push_back(8);
    indices.push_back(9);

    indices.push_back(10);
    indices.push_back(9);
    indices.push_back(8);

    indices.push_back(9);
    indices.push_back(10);
    indices.push_back(11);

    indices.push_back(10);
    indices.push_back(14);
    indices.push_back(15);

    indices.push_back(10);
    indices.push_back(15);
    indices.push_back(11);

    indices.push_back(12);
    indices.push_back(13);
    indices.push_back(14);

    indices.push_back(14);
    indices.push_back(13);
    indices.push_back(15);


    // bottom
    
    indices.push_back(13);
    indices.push_back(12);
    indices.push_back(0);

    indices.push_back(13);
    indices.push_back(0);
    indices.push_back(1);


    //this
    indices.push_back(0);
    indices.push_back(6);
    indices.push_back(2);

    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(6);

    indices.push_back(0);
    indices.push_back(12);
    indices.push_back(4);

    indices.push_back(4);
    indices.push_back(12);
    indices.push_back(8);

    indices.push_back(12);
    indices.push_back(14);
    indices.push_back(10);

    indices.push_back(12);
    indices.push_back(10);
    indices.push_back(8);


    //back face
    indices.push_back(3);
    indices.push_back(5);
    indices.push_back(1);

    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(5);

    indices.push_back(5);
    indices.push_back(13);
    indices.push_back(1);

    indices.push_back(9);
    indices.push_back(13);
    indices.push_back(5);

    indices.push_back(15);
    indices.push_back(13);
    indices.push_back(9);

    indices.push_back(11);
    indices.push_back(15);
    indices.push_back(9);
    

    
    //now we create a polygon mesh object. Think of this as literally a mesh
    //or network of polygons. There are vertices and they form polygons
    util::PolygonMesh<VertexAttrib> mesh;

    //give it the vertex data
    mesh.setVertexData(vertexData);
    //give it the index data that forms the polygons
    mesh.setPrimitives(indices);
    mesh.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh.setPrimitiveSize(3); //3 vertices per polygon

    BSPTree bsp(mesh);
    util::PolygonMesh<VertexAttrib> meshbsp = bsp.getMesh();
    meshbsp.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp.setPrimitiveSize(3); //3 vertices per polygon
    
    ifstream in_file;
    
    util::PolygonMesh<VertexAttrib> mesh_a;
    in_file.open("/Users/himanshusharma/karnuz/AlgoDS/geometry/models/cone.obj");
    util::ObjImporter<VertexAttrib> objImp;
    mesh_a = objImp.importFile(in_file, true);
    mesh_a.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh_a.setPrimitiveSize(3); //3 vertices per polygon
    in_file.close();
    in_file.clear();

    
    util::PolygonMesh<VertexAttrib> mesh_b;    
    in_file.open("/Users/himanshusharma/karnuz/AlgoDS/geometry/models/cylinder.obj");
    mesh_b = objImp.importFile(in_file, true);
    mesh_b.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh_b.setPrimitiveSize(3); //3 vertices per polygon
    in_file.close();
    in_file.clear();
    
    
    BSPTree bsp_a(mesh_a);
    cout << "reach here\n";
    util::PolygonMesh<VertexAttrib> meshbsp_a;
    meshbsp_a = bsp_a.getMesh();
    cout << "reach not here\n";
    meshbsp_a.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_a.setPrimitiveSize(3); //3 vertices per polygon


    
    BSPTree bsp_b(mesh_b);
    util::PolygonMesh<VertexAttrib> meshbsp_b;
    meshbsp_b = bsp_b.getMesh();
    meshbsp_b.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_b.setPrimitiveSize(3); //3 vertices per polygon


    
    BSPTree<VertexAttrib> bsp_c = *(bsp_a.clipTo(&bsp_b));
    util::PolygonMesh<VertexAttrib> meshbsp_c;
    meshbsp_c = bsp_c.getMesh();
    meshbsp_c.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_c.setPrimitiveSize(3); //3 vertices per polygon
    

    obj = new util::ObjectInstance("mesh1");
    obj->initPolygonMesh<VertexAttrib>(
                gl, //the gl wrapper
                program, //the shader program
                shaderLocations, //the shader locations
                shaderVarsToVertexAttribs, //the shader variable -> attrib map
                meshbsp); //the actual mesh object

    
    obj2 = new util::ObjectInstance("mesh2");
    obj2->initPolygonMesh<VertexAttrib>(
                gl, //the gl wrapper
                program, //the shader program
                shaderLocations, //the shader locations
                shaderVarsToVertexAttribs, //the shader variable -> attrib map
                meshbsp_b); //the actual mesh object
    
    
    //we will color this square red
    color = glm::vec4(1.0f,0.5f,1.0f,1.0f);

    proj = glm::perspective(4.5f, 3.0f, 4.5f, 10000.0f);
    modelview = glm::lookAt(glm::vec3(1.5f, 1.0f, 1.5f),
    //modelview = glm::lookAt(glm::vec3(0.75f, 0.5f, 0.75f), 
  		   glm::vec3(0.0f, 0.0f, 0.0f), 
  		   glm::vec3(0.0f, 0.1f, 0.0f));

}



void View::init_b(util::OpenGLFunctions& gl)
{
    program.createProgram(gl,
                          string("shaders/default.vert"),
                          string("shaders/default.frag"));

    shaderLocations = program.getAllShaderVariables(gl);
    
    vector<glm::vec4> positions;
    
    //    positions.push_back(glm::vec4(0.0f,0.0f,0.0f,1.0f));
    //positions.push_back(glm::vec4(5.0f,0.0f,0.0f,1.0f));
    //positions.push_back(glm::vec4(0.0f, 2.0f,0.0f,1.0f));

    positions.push_back(glm::vec4(0.0f,0.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(2.0f,0.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(0.0f, 2.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(0.0f,0.0f,2.0f,1.0f));
    positions.push_back(glm::vec4(2.0f,2.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(0.0f, 2.0f,2.0f,1.0f));
    positions.push_back(glm::vec4(2.0f,0.0f,2.0f,1.0f));
    positions.push_back(glm::vec4(2.0f,2.0f,2.0f,1.0f));

    
    //    positions.push_back(glm::vec4(1.0f,1.0f,-1.0f,1.0f));
    //positions.push_back(glm::vec4(0.0f,0.0f,1.0f,1.0f));
    //positions.push_back(glm::vec4(0.0f,4.0f,1.0f,1.0f));
    

    //set up vertex attributes (in this case we have only position)
    vector<VertexAttrib> vertexData;
    for (unsigned int i=0;i<positions.size();i++) {
        VertexAttrib v;
        vector<float> data;

        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        data.push_back(positions[i].w);
        v.setData("position",data);

        vertexData.push_back(v);
    }


    vector<unsigned int> indices;
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(1);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(4);
    
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(6);
    indices.push_back(0);
    indices.push_back(6);
    indices.push_back(3);    
    
    indices.push_back(0);
    indices.push_back(5);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(5);
    
    indices.push_back(1);
    indices.push_back(4);
    indices.push_back(7);
    indices.push_back(1);
    indices.push_back(7);
    indices.push_back(6);
    
    indices.push_back(3);
    indices.push_back(6);
    indices.push_back(5);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(7);
    
    indices.push_back(7);
    indices.push_back(4);
    indices.push_back(2);
    indices.push_back(7);
    indices.push_back(2);
    indices.push_back(5);

    
    //now we create a polygon mesh object. Think of this as literally a mesh
    //or network of polygons. There are vertices and they form polygons
    util::PolygonMesh<VertexAttrib> mesh;

    //give it the vertex data
    mesh.setVertexData(vertexData);
    //give it the index data that forms the polygons
    mesh.setPrimitives(indices);
    mesh.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh.setPrimitiveSize(3); //3 vertices per polygon

    BSPTree bsp(mesh);
    util::PolygonMesh<VertexAttrib> meshbsp = bsp.getMesh();
    meshbsp.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp.setPrimitiveSize(3); //3 vertices per polygon



    vector<glm::vec4> positions_b;
    
    //    positions.push_back(glm::vec4(0.0f,0.0f,0.0f,1.0f));
    //positions.push_back(glm::vec4(5.0f,0.0f,0.0f,1.0f));
    //positions.push_back(glm::vec4(0.0f, 2.0f,0.0f,1.0f));

    positions_b.push_back(glm::vec4(1.0f,1.0f,1.0f,1.0f));
    positions_b.push_back(glm::vec4(3.0f,1.0f,1.0f,1.0f));
    positions_b.push_back(glm::vec4(1.0f, 3.0f,1.0f,1.0f));
    positions_b.push_back(glm::vec4(1.0f,1.0f,3.0f,1.0f));
    positions_b.push_back(glm::vec4(3.0f,3.0f,1.0f,1.0f));
    positions_b.push_back(glm::vec4(1.0f, 3.0f,3.0f,1.0f));
    positions_b.push_back(glm::vec4(3.0f,1.0f,3.0f,1.0f));
    positions_b.push_back(glm::vec4(3.0f,3.0f,3.0f,1.0f));

    
    //    positions.push_back(glm::vec4(1.0f,1.0f,-1.0f,1.0f));
    //positions.push_back(glm::vec4(0.0f,0.0f,1.0f,1.0f));
    //positions.push_back(glm::vec4(0.0f,4.0f,1.0f,1.0f));
    

    //set up vertex attributes (in this case we have only position)
    vector<VertexAttrib> vertexData_b;
    for (unsigned int i=0;i<positions_b.size();i++) {
        VertexAttrib v;
        vector<float> data;

        data.push_back(positions_b[i].x);
        data.push_back(positions_b[i].y);
        data.push_back(positions_b[i].z);
        data.push_back(positions_b[i].w);
        v.setData("position",data);

        vertexData_b.push_back(v);
    }


    vector<unsigned int> indices_b;
    indices_b.push_back(0);
    indices_b.push_back(4);
    indices_b.push_back(1);
    indices_b.push_back(0);
    indices_b.push_back(2);
    indices_b.push_back(4);

    
    indices_b.push_back(0);
    indices_b.push_back(1);
    indices_b.push_back(6);
    indices_b.push_back(0);
    indices_b.push_back(6);
    indices_b.push_back(3);

    
    indices_b.push_back(0);
    indices_b.push_back(5);
    indices_b.push_back(2);
    indices_b.push_back(0);
    indices_b.push_back(3);
    indices_b.push_back(5);

    
    indices_b.push_back(1);
    indices_b.push_back(4);
    indices_b.push_back(7);
    indices_b.push_back(1);
    indices_b.push_back(7);
    indices_b.push_back(6);

    indices_b.push_back(3);
    indices_b.push_back(6);
    indices_b.push_back(5);
    indices_b.push_back(5);
    indices_b.push_back(6);
    indices_b.push_back(7);

    indices_b.push_back(7);
    indices_b.push_back(4);
    indices_b.push_back(2);
    indices_b.push_back(7);
    indices_b.push_back(2);
    indices_b.push_back(5);

    
    
    //now we create a polygon mesh object. Think of this as literally a mesh
    //or network of polygons. There are vertices and they form polygons
    util::PolygonMesh<VertexAttrib> mesh_b;

    //give it the vertex data
    mesh_b.setVertexData(vertexData_b);
    //give it the index data that forms the polygons
    mesh_b.setPrimitives(indices_b);
    mesh_b.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh_b.setPrimitiveSize(3); //3 vertices per polygon

    BSPTree bsp_b(mesh_b);
    util::PolygonMesh<VertexAttrib> meshbsp_b = bsp_b.getMesh();
    meshbsp_b.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_b.setPrimitiveSize(3); //3 vertices per polygon
    
    BSPTree<VertexAttrib> bsp_c = *(bsp_b.clipTo(&bsp));
    util::PolygonMesh<VertexAttrib> meshbsp_c;
    meshbsp_c = bsp_c.getMesh();
    meshbsp_c.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_c.setPrimitiveSize(3); //3 vertices per polygon
    
    map<string,string> shaderVarsToVertexAttribs;

    //currently there are only two per-vertex attribute: position and color
    shaderVarsToVertexAttribs["vPosition"]="position";

    obj = new util::ObjectInstance("triangles");
    obj->initPolygonMesh<VertexAttrib>(
                gl, //the gl wrapper
                program, //the shader program
                shaderLocations, //the shader locations
                shaderVarsToVertexAttribs, //the shader variable -> attrib map
                meshbsp_c); //the actual mesh object

    obj2 = new util::ObjectInstance("triangles2");
    obj2->initPolygonMesh<VertexAttrib>(
                gl, //the gl wrapper
                program, //the shader program
                shaderLocations, //the shader locations
                shaderVarsToVertexAttribs, //the shader variable -> attrib map
                meshbsp); //the actual mesh object

    //we will color this square red
    color = glm::vec4(1.0f,0.5f,1.0f,1.0f);

    proj = glm::perspective(1.5f, 1.0f, 0.1f, 10000.0f);
    modelview = glm::lookAt(glm::vec3(4.5f, 1.5f, 4.5f),
    //modelview = glm::lookAt(glm::vec3(0.75f, 0.5f, 0.75f), 
  		   glm::vec3(0.0f, 0.0f, 0.0f), 
  		   glm::vec3(0.0f, 0.1f, 0.0f));

}


void View::draw(util::OpenGLFunctions& gl)
{
    //set the background color to be white
    gl.glClearColor(0.0f,0.0f,0.0f,1.0f);
    //clear the background
    gl.glClear(GL_COLOR_BUFFER_BIT);
    //enable the shader program
    program.enable(gl);

    //pass the projection matrix to the shader
    gl.glUniformMatrix4fv( //projection matrix is a uniform variable in shader
                           //4f indicates 4x4 matrix,
                           //v indicates it will be given as an array
                shaderLocations.getLocation("projection"), //location in shader
                1, //only one matrix
                false, //don't normalize the matrix (i.e. takes numbers as-is)
                glm::value_ptr(proj)); //convenience function to convert
                                       //glm::mat4 to float array


    glm::mat4 modelview_trackball = modelview*trackballtransform;
    stack<glm::mat4> mvstack;
    mvstack.push(modelview);
    mvstack.push(modelview_trackball);
    

    
    gl.glUniform4fv(shaderLocations.getLocation("vColor"), //location in shader
		    1, //only one value should be read from the array below
		    glm::value_ptr(color)); //convenience function to convert
    
    gl.glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    s->draw(mvstack);
    
    gl.glFlush();
    //disable the program
    program.disable(gl);
}


void View::draw_a(util::OpenGLFunctions& gl)
{
    //set the background color to be white
    gl.glClearColor(0.0f,0.0f,0.0f,1.0f);
    //clear the background
    gl.glClear(GL_COLOR_BUFFER_BIT);
    //enable the shader program
    program.enable(gl);

    //pass the projection matrix to the shader
    gl.glUniformMatrix4fv( //projection matrix is a uniform variable in shader
                           //4f indicates 4x4 matrix,
                           //v indicates it will be given as an array
                shaderLocations.getLocation("projection"), //location in shader
                1, //only one matrix
                false, //don't normalize the matrix (i.e. takes numbers as-is)
                glm::value_ptr(proj)); //convenience function to convert
                                       //glm::mat4 to float array


    /*
    for(int p = 0; p< 4; p++) {
      for(int q = 0; q< 4; q++) {
	cout << modelview[p][q] << " : ";
      }
    }
    cout << " modelview\n";
    */
    glm::mat4 modelview_trackball = modelview*trackballtransform;

    gl.glUniformMatrix4fv( //modelview matrix is a uniform variable in shader
                           //4f indicates 4x4 matrix,
                           //v indicates it will be given as an array
			  shaderLocations.getLocation("modelview"), //location in shader
			  1, //only one matrix
			  false, //don't normalize the matrix (i.e. takes numbers as-is)
			  glm::value_ptr(modelview_trackball)); //convenience function to convert
    //glm::mat4 to float array

    
    gl.glUniform4fv( //the color is a uniform variable in the shader
                     //4f indicates this will be specified as 3 float values
                     //v means the three values will be specified in an array
                shaderLocations.getLocation("vColor"), //location in shader
                1, //only one value should be read from the array below
                glm::value_ptr(color)); //convenience function to convert
                                        //glm::vec3 to float array

    gl.glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    //draw the object
    obj->draw(gl);
    obj2->draw(gl);
    //opengl is a pipeline-based framework. Things are not drawn as soon as
    //they are supplied. glFlush flushes the pipeline and draws everything
    gl.glFlush();
    //disable the program
    program.disable(gl);
}

void View::reshape(util::OpenGLFunctions& gl,int width,int height)
{
    //record the new width and height
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;

    cout << width << ":" << height << "  -wh\n";

    /*
     * The viewport is the portion of the screen window where the drawing
     * would be placed. We want it to take up the entire area of the window
     * so we set the viewport to be the entire window.
     * Look at documentation of glViewport
     */

    gl.glViewport(0, 0, width, height);

    /*
     * This sets up the part of our virtual world that will be visible in
     * the screen window. Since this program is drawing 2D, the virtual world
     * is 2D. Thus this window can be specified in terms of a rectangle
     * Look at the documentation of glOrtho2D, which glm::ortho implements
     */

    proj = glm::ortho(-150.0f,150.0f,-150.0f,150.0f);

}

void View::dispose(util::OpenGLFunctions& gl)
{
    //clean up the OpenGL resources used by the object
    obj->cleanup(gl);
    //release the shader resources
    program.releaseShaders(gl);
}


void View::trackball(glm::vec2 delta)
{
  glm::mat4 transform;
  transform = glm::rotate(transform, delta[0]/300, glm::vec3(0.0f, 1.0f, 0.0f));
  transform = glm::rotate(transform, delta[1]/300, glm::vec3(1.0f, 0.0f, 0.0f));
  trackballtransform = trackballtransform*transform;
  //  cout << delta[0] << " :: " << delta[1] << " :trackball\n";
}


void View::key_callback(char input)
{
  if(input == 'a') {
    s->toggleNode("leaf1");
  }
  else if (input == 'b') {
    s->toggleNode("leaf2");
  }
  else if (input == 'c') {
    s->toggleNode("leaf3");
  }
  
}


/*
void View::init(util::OpenGLFunctions& gl)
{
    program.createProgram(gl,
                          string("shaders/default.vert"),
                          string("shaders/default.frag"));

    shaderLocations = program.getAllShaderVariables(gl);
    
    vector<glm::vec4> positions;
    
    //    positions.push_back(glm::vec4(0.0f,0.0f,0.0f,1.0f));
    //positions.push_back(glm::vec4(5.0f,0.0f,0.0f,1.0f));
    //positions.push_back(glm::vec4(0.0f, 2.0f,0.0f,1.0f));

    positions.push_back(glm::vec4(0.0f,0.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(5.0f,0.0f,0.0f,1.0f));
    positions.push_back(glm::vec4(0.0f, 5.0f,0.0f,1.0f));

    //    positions.push_back(glm::vec4(1.0f,1.0f,2.0f,1.0f));
    //positions.push_back(glm::vec4(3.0f,1.0f,1.0f,1.0f));
    //positions.push_back(glm::vec4(1.0f, 3.0f,-2.0f,1.0f));


    //set up vertex attributes (in this case we have only position)
    vector<VertexAttrib> vertexData;
    for (unsigned int i=0;i<positions.size();i++) {
        VertexAttrib v;
        vector<float> data;

        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        data.push_back(positions[i].w);
        v.setData("position",data);

        vertexData.push_back(v);
    }


    vector<unsigned int> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    //indices.push_back(3);
    //indices.push_back(4);
    //indices.push_back(5);


    //now we create a polygon mesh object. Think of this as literally a mesh
    //or network of polygons. There are vertices and they form polygons
    util::PolygonMesh<VertexAttrib> mesh;
    mesh.setVertexData(vertexData);
    mesh.setPrimitives(indices);
    mesh.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh.setPrimitiveSize(3); //3 vertices per polygon

    BSPTree bsp(mesh);
    util::PolygonMesh<VertexAttrib> meshbsp = bsp.getMesh();
    meshbsp.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp.setPrimitiveSize(3); //3 vertices per polygon



    vector<glm::vec4> positions_b;
    positions_b.push_back(glm::vec4(1.0f,1.0f,-1.0f,1.0f));
    positions_b.push_back(glm::vec4(1.0f, 3.0f,-2.0f,1.0f));
    positions_b.push_back(glm::vec4(3.0f,1.0f,1.0f,1.0f));

    
    

    //set up vertex attributes (in this case we have only position)
    vector<VertexAttrib> vertexData_b;
    for (unsigned int i=0;i<positions_b.size();i++) {
        VertexAttrib v;
        vector<float> data;

        data.push_back(positions_b[i].x);
        data.push_back(positions_b[i].y);
        data.push_back(positions_b[i].z);
        data.push_back(positions_b[i].w);
        v.setData("position",data);

        vertexData_b.push_back(v);
    }


    vector<unsigned int> indices_b;
    indices_b.push_back(0);
    indices_b.push_back(1);
    indices_b.push_back(2);

    //now we create a polygon mesh object. Think of this as literally a mesh
    //or network of polygons. There are vertices and they form polygons
    util::PolygonMesh<VertexAttrib> mesh_b;

    //give it the vertex data
    mesh_b.setVertexData(vertexData_b);
    //give it the index data that forms the polygons
    mesh_b.setPrimitives(indices_b);
    mesh_b.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    mesh_b.setPrimitiveSize(3); //3 vertices per polygon

    BSPTree bsp_b(mesh_b);
    util::PolygonMesh<VertexAttrib> meshbsp_b = bsp_b.getMesh();
    meshbsp_b.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_b.setPrimitiveSize(3); //3 vertices per polygon


    
    BSPTree<VertexAttrib> bsp_c = *(bsp_b.clipTo(&bsp));
    util::PolygonMesh<VertexAttrib> meshbsp_c;
    meshbsp_c = bsp_c.getMesh();
    meshbsp_c.setPrimitiveType(GL_TRIANGLES); //when rendering specify this to OpenGL
    meshbsp_c.setPrimitiveSize(3); //3 vertices per polygon
    
    map<string,string> shaderVarsToVertexAttribs;

    //currently there are only two per-vertex attribute: position and color
    shaderVarsToVertexAttribs["vPosition"]="position";

    cout << "yahan aaya\n";
    obj = new util::ObjectInstance("triangles");
    obj->initPolygonMesh<VertexAttrib>(
                gl, //the gl wrapper
                program, //the shader program
                shaderLocations, //the shader locations
                shaderVarsToVertexAttribs, //the shader variable -> attrib map
                meshbsp_c); //the actual mesh object

    obj2 = new util::ObjectInstance("triangles2");
    obj2->initPolygonMesh<VertexAttrib>(
                gl, //the gl wrapper
                program, //the shader program
                shaderLocations, //the shader locations
                shaderVarsToVertexAttribs, //the shader variable -> attrib map
                meshbsp); //the actual mesh object

    //we will color this square red
    color = glm::vec4(1.0f,0.5f,1.0f,1.0f);

    proj = glm::perspective(1.5f, 1.0f, 0.1f, 10000.0f);
    modelview = glm::lookAt(glm::vec3(4.5f, 1.5f, 4.5f),
    //modelview = glm::lookAt(glm::vec3(0.75f, 0.5f, 0.75f), 
  		   glm::vec3(0.0f, 0.0f, 0.0f), 
  		   glm::vec3(0.0f, 0.1f, 0.0f));

}
*/
