#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "OpenGLFunctions.h"
#include "View.h"




template <typename F, typename C>
class MyGLWindow {
  util::OpenGLFunctions *gl;
  C *obj;
  F fun;


  // replace F fun by five function pointers of specific signature and use them in the respective
  // callback method.
  
public:
  MyGLWindow(F fun, C *obj) {
    this->obj = obj;
    this->fun = fun;
  }

  MyGLWindow(util::OpenGLFunctions *g, F fun, C *obj) {
    this->gl = g;
    this->obj = obj;
    this->fun = fun;
  }

  void mouseButtonPressed(double x, double y) {
    //    ((*(this->obj)).(*(this->fun)))(x,y);
    (obj->*fun)(x,y);
    //fun(x,y);
  }

  void windowResized(double x, double y) {
    //    ((*(this->obj)).(*(this->fun)))(x,y);
    (obj->*fun)(*gl,x,y);
    //fun(x,y);
  }

  void mouse_button_callback(int button, int action, int mods, double xpos, double ypos){
    (obj->*fun)(button,action,mods,xpos,ypos);
  }

  void cursor_enter_callback(int entered) {
    (obj->*fun)(entered);
  }

  void cursor_position_callback(double xpos, double ypos) {
    (obj->*fun)(xpos, ypos);
  }

};


class OpenGLWindow
{
private:
  GLFWwindow *window;

  View view;

  util::OpenGLFunctions *gl;

  glm::vec2 mousePos;

  bool dragged;

  bool cursor_in_window;
  
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
    set_cursor_position_callback();
    set_mouse_button_callback();
    set_key_callback();
    
    //    set_cursor_enter_callback();
    //setWindowResizeCallback();
    //        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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



  

  
  void setWindowResizeCallback() {

    typedef void (View::*Fun) (util::OpenGLFunctions& gl, int width, int height);
    Fun func = &View::reshape;
    MyGLWindow<Fun,View> *gw = new MyGLWindow(this->gl,func, &view);

    glfwSetWindowUserPointer(window, gw);
    
    auto window_resize_callback = [](GLFWwindow* w, int width, int height)
    {
      static_cast<MyGLWindow<Fun,View> *>(glfwGetWindowUserPointer(w))->windowResized(width, height);
    };
    glfwSetFramebufferSizeCallback(window, window_resize_callback);
  }  

  
  void key_callback(int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS) {
      if(key == GLFW_KEY_A) {
	this->view.key_callback('a', *gl);
      }
      else if(key == GLFW_KEY_B) {
	this->view.key_callback('b', *gl);
      }
      else if(key == GLFW_KEY_C) {
	this->view.key_callback('c', *gl);
      }
      else if(key == GLFW_KEY_W) {
	this->view.key_callback('w', *gl);
      }
      else if(key == GLFW_KEY_F) {
	this->view.key_callback('f', *gl);
      }
      else {
      }
    }
  }

  
  void set_key_callback() {
    typedef void (OpenGLWindow::*Fun) (int key, int scancode, int action, int mods);
    Fun func = &OpenGLWindow::key_callback;
    glfwSetWindowUserPointer(window, this);

    auto key_callback_lambda = [](GLFWwindow* w, int key, int scancode, int action, int mods)
			       {
				 static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(w))
				   ->key_callback(key, scancode, action, mods);
			       };
    glfwSetKeyCallback(window, key_callback_lambda);
  }

  
  void mouse_button_callback(int button, int action, int mods, double xpos, double ypos)
  {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      mousePos = glm::vec2(xpos,ypos);
      dragged = true;
      cout << "all done in pressed\n";
    }
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
      glm::vec2 delta = glm::vec2(xpos - this->mousePos[0], ypos - this->mousePos[1]);
      this->view.trackball(delta);
      mousePos = glm::vec2(xpos, ypos);
      dragged = false;
      cout << "all done in release\n";
    }    
  }


  
  void set_mouse_button_callback() {

    typedef void (OpenGLWindow::*Fun) (int button, int action, int mods, double xpos, double ypos);
    Fun func = &OpenGLWindow::mouse_button_callback;
    //    MyGLWindow<Fun,OpenGLWindow> *gw = new MyGLWindow(func, this);
    glfwSetWindowUserPointer(window, this);


    auto mouse_button_callback_lambda = [](GLFWwindow* w, int button, int action, int mods)
					{
					  double xpos, ypos;
					  glfwGetCursorPos(w, &xpos, &ypos);
					  static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(w))
					    ->mouse_button_callback(button, action, mods, xpos, ypos);
					};
    glfwSetMouseButtonCallback(window, mouse_button_callback_lambda);
  }

  
  void cursor_enter_callback(int entered)
  {
    if (entered)
      {
        // The cursor entered the content area of the window
	this->cursor_in_window = true;
	cout << "cursor in window set true \n";
      }
    else
      {
        // The cursor left the content area of the window
	this->cursor_in_window = false;
	cout << "cursor in window set false \n";
    }
  }

  
  void set_cursor_enter_callback() {

    typedef void (OpenGLWindow::*Fun) (int entered);
    Fun func = &OpenGLWindow::cursor_enter_callback;
    MyGLWindow<Fun,OpenGLWindow> *gw = new MyGLWindow(func, this);
    glfwSetWindowUserPointer(window, gw);


    auto cursor_enter_callback_lambda = [](GLFWwindow* w, int entered)
    {
      // can be used to check from where cursor entered
      //      double xpos, ypos;
      //glfwGetCursorPos(w, &xpos, &ypos);
      //cout << xpos << ypos << "here\n";

      static_cast<MyGLWindow<Fun,OpenGLWindow> *>(glfwGetWindowUserPointer(w))->cursor_enter_callback(entered);
    };
    glfwSetCursorEnterCallback(window, cursor_enter_callback_lambda);
  }


  void cursor_position_callback(double xpos, double ypos)
  {

    //    cout << xpos << "," << ypos << " :cursor position without press\n";
    if(this->dragged) {
      glm::vec2 delta = glm::vec2(xpos - this->mousePos[0], ypos - this->mousePos[1]);
      this->view.trackball(delta);
      this->mousePos = glm::vec2(xpos, ypos);
    }
  }

  
  void set_cursor_position_callback() {

    typedef void (OpenGLWindow::*Fun) (double xpos, double ypos);
    Fun func = &OpenGLWindow::cursor_position_callback;
    MyGLWindow<Fun,OpenGLWindow> *gw = new MyGLWindow(func, this);
    glfwSetWindowUserPointer(window, this);

    auto cursor_position_callback_lambda = [](GLFWwindow* w, double xpos, double ypos)
    {
      //static_cast<MyGLWindow<Fun,OpenGLWindow> *>(glfwGetWindowUserPointer(w))->cursor_position_callback(xpos,ypos);

      static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(w))->cursor_position_callback(xpos,ypos);

    };
    
    glfwSetCursorPosCallback(window, cursor_position_callback_lambda);
  }
  
};






/*
    if(dragged) {
      //      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      glm::vec2 delta = glm::vec2(xpos - this->mousePos[0], ypos - this->mousePos[1]);
      this->view.trackball(delta);
      this->mousePos = glm::vec2(xpos, ypos);
      cout << "mousePos: " << mousePos[0] << ", " << mousePos[1] << "\n";
      cout << "delta: " << delta[0] << ", " << delta[1] << "\n";
      action = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
      //if(action == GLFW_RELEASE) {
      //	dragged = false;
      //}
    }
    */


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
