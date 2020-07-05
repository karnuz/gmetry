#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "OpenGLFunctions.h"
#include "View.h"

class OpenGLWindow
{
private:
  GLFWwindow *window;

  View view;

  util::OpenGLFunctions *gl;
  
public:
  OpenGLWindow() {
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
      fprintf(stderr, "ERROR: could not start GLFW3\n");
      return;
    }
    
    // uncomment these lines if on Apple OS X
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_ANY_PROFILE,GLFW_OPENGL_ANY_PROFILE);
    window = glfwCreateWindow(3960, 3960, "Hello", NULL, NULL);
    if (!window) {
      fprintf(stderr, "ERROR: could not open window with GLFW3\n");
      glfwTerminate();
      return;
    }

    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    

    glfwMakeContextCurrent(window);
    //    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    initializeGL();
  }

  void initializeGL() {
    gl = new util::OpenGLFunctions();
    view.init(*gl);
  }

  /*
  void setView(View *v) {
    view = v;
  }
  */

  void draw() {

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    //glViewport(0, 0, width, height);
    //glOrtho(-3000, 3000, -3000, 3000,-10,10);
    //glViewport(0, 0, width, height);

    cout << width << "\n";
    cout << height << "\n";

    while(!glfwWindowShouldClose(window)){
      view.draw(*gl);
      // update other events like input handling 
      glfwPollEvents();
      // put the stuff we've been drawing onto the display
      glfwSwapBuffers(window);

    }
    glfwTerminate();   
  }



  /*
  
  template <typename F, typename C>
  void setClickPositionCallback(F func, C *obj) {

    //(func)(8.0,5.0);   // wont work
    //((*obj).*func)(6.0,4.0);

    MyGLWindow<F,C> *gw = new MyGLWindow(func, obj);
    
    glfwSetWindowUserPointer(window, gw);
    
    auto mouse_button_callback = [](GLFWwindow* w, int button, int action, int mods)
    {
      if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
	  double xpos, ypos;
	  //getting cursor position                     
	  glfwGetCursorPos(w, &xpos, &ypos);
	  int width, height;
	  glfwGetFramebufferSize(w, &width, &height);
      	  static_cast<MyGLWindow<F,C>*>(glfwGetWindowUserPointer(w))->mouseButtonPressed(60*(-0.5 + (2*xpos/width)), 60*(0.5 - (2*ypos/height)));
	  // func(xpos,ypos);
	  //	  cout << "Cursor Position is " << xpos << " : " << ypos << " width: " << width << " height " << height << endl;
	}
    };

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
  } 
  */ 
};
