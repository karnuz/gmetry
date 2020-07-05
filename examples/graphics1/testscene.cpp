#include "GLFWWindow.h"
//#include "OpenGLFunctions.h"
#include "ObjImporter.h"
#include <PolygonMesh.h>
#include <fstream>
#include "VertexAttrib.h"
#include <map>
#include "ShaderProgram_him.h"

//#include "ObjectInstance.h"



using namespace std;
using namespace util;
int main() {

  OpenGLWindow glfw;

  glfw.draw();

  /*
  ifstream in_file;
  in_file.open("/Users/himanshusharma/karnuz/AlgoDS/geometry/models/cone.obj");

  PolygonMesh<VertexAttrib> mesh;
  ObjImporter<VertexAttrib> obj;
  mesh = obj.importFile(in_file, true);

  mesh.setPrimitiveType(GL_TRIANGLES);
  mesh.setPrimitiveSize(3);

  map<string,string> shaderVarsToVertexAttribs;

  shaderVarsToVertexAttribs["vPosition"]="position";
  //  obj = new ObjectInstance("cone");

  ShaderProgram program;
  program.createProgram(glfw,
			string("shaders/default.vert"),
			string("shaders/default.frag"));

  //shaderLocations = program.getAllShaderVariables(gl);
  
  */

  cout << "all went well \n";
  return 0;
}
