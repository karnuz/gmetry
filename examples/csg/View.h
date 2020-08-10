#ifndef VIEW_H
#define VIEW_H

#include <OpenGLFunctions.h>
//#include <exception>
using namespace std;
#include <glm/glm.hpp>
#include "ShaderProgram_him.h"
#include "ShaderLocationsVault.h"
#include <ObjectInstance.h>
#include "VertexAttrib.h"
#include "Scenegraph.h"

/*
 * This class encapsulates all our program-specific details. This makes our
 * design better if we wish to port it to another C++-based windowing
 * library
 */

class View
{
public:
    View();
    ~View();
    /*
     * This is called when the application is being initialized. We should
     * do all our initializations here. This is also the first function where
     * OpenGL commands will work (i.e. don't do any OpenGL related stuff in the
     * constructor!)
     */
    void init(util::OpenGLFunctions& e);
    void init_a(util::OpenGLFunctions& e);
    void init_b(util::OpenGLFunctions& e);
    /*
     * This function is called whenever the window is to be redrawn
     */
    void draw(util::OpenGLFunctions& e);
    void draw_a(util::OpenGLFunctions& e);

    /*
     * This function is called whenever the window is reshaped
     */
    void reshape(util::OpenGLFunctions& gl,int width,int height);

    /*
     * This function is called whenever the window is being destroyed
     */
    void dispose(util::OpenGLFunctions& gl);

  void trackball(glm::vec2 delta);
  void key_callback(char input, util::OpenGLFunctions& gl);
  
private:
  //record the current window width and height
  int WINDOW_WIDTH,WINDOW_HEIGHT;
  //the projection matrix
  glm::mat4 proj;
  //the modelview matrix
  glm::mat4 modelview;
  //the object which we are rendering
  util::ObjectInstance *obj;
  util::ObjectInstance *obj2;
  //the list of shader variables and their locations within the shader program
  util::ShaderLocationsVault shaderLocations;
  //the color of our object
  glm::vec3 color;
  //the GLSL shader
  util::ShaderProgram program;
  //trackball transform
  glm::mat4 trackballtransform = glm::mat4(1.0);
  //Scenegraph
  Scenegraph<VertexAttrib>* s;
  //ScenegraphRenderer
  ScenegraphRenderer<VertexAttrib>* sr;

  bool wireframe_active = false;
};

#endif // VIEW_H
