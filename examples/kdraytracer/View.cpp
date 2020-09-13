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
#include "myscene.h"

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


void View::initShaders(util::OpenGLFunctions& gl) {
  cout << "loading shader\n";
    //create the shader program
    program.createProgram(gl,
                          string("shaders/phong-multiple.vert"),
                          string("shaders/phong-multiple.frag"));
    cout << "shader loaded\n";

    //assuming it got created, get all the shader variables that it uses
    //so we can initialize them at some point
    shaderLocations = program.getAllShaderVariables(gl);

      
    map<string,string> shaderVarsToVertexAttribs;
    shaderVarsToVertexAttribs["vPosition"]="position";
    shaderVarsToVertexAttribs["vNormal"]="normal";

    sr = new ScenegraphRenderer<VertexAttrib>(gl, shaderLocations, shaderVarsToVertexAttribs, program);
    
    s->setRenderer(sr);
    //    proj = glm::ortho(-0.1f, 0.0f ,0.0f,0.1f);
    proj = glm::perspective(1.5f, 1.0f, 0.1f, 10000.0f);
    modelview = glm::lookAt(glm::vec3(9.0f, 7.3f, 3.0f),
  		   glm::vec3(0.0f, 0.0f, 0.0f),
  		   glm::vec3(0.0f, 1.0f, 1.0f));
}

void View::setScenegraph(Scenegraph<VertexAttrib>* sg)
{
  s = sg;
}




void View::draw(util::OpenGLFunctions& gl)
{
    //set the background color to be white
    gl.glClearColor(0.0f,0.0f,0.0f,1.0f);
    //clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //enable the shader program
    program.enable(gl);
    glEnable(GL_DEPTH_TEST);
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
    
    

    s->renderLights(mvstack);
    s->draw(mvstack);
    
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
  glm::mat4 transform = glm::mat4(1.0);
  transform = glm::rotate(transform, delta[0]/300, glm::vec3(0.0f, 1.0f, 0.0f));
  transform = glm::rotate(transform, delta[1]/300, glm::vec3(1.0f, 0.0f, 0.0f));
  trackballtransform = trackballtransform*transform;
  //  cout << delta[0] << " :: " << delta[1] << " :trackball\n";
}


void View::key_callback(char input, util::OpenGLFunctions& gl)
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
  else if (input == 'w') {
    if(wireframe_active) {
      gl.glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
    else {
      gl.glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    wireframe_active = !wireframe_active;
  }
  else if (input == 'f') {
    glm::mat4 eye;
    trackballtransform = eye;
  }
}


