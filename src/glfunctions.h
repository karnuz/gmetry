#include <iostream>
#include <cassert>
#include <math.h> 
#include <algorithm>
#include <vector>
//#define pi 3.142857

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace std;

class View;
class Scene;
class Mesh;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(-width/2, -height/2, width/2, height/2);
    glOrtho(-20,20,-20,20,-10,10);
}

/*
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
    {
       double xpos, ypos;
       //getting cursor position
       glfwGetCursorPos(window, &xpos, &ypos);
       cout << "Cursor Position at (" << xpos << " : " << ypos << endl;
    }
}
*/

// a callable pointer here.

template <typename F, typename C>
class MyGLWindow {
  C *obj;
  F fun;

public:
  MyGLWindow(F fun, C *obj) {
    this->obj = obj;
    this->fun = fun;
  }

  void mouseButtonPressed(double x, double y) {
    //    ((*(this->obj)).(*(this->fun)))(x,y);
    (obj->*fun)(x,y);
    //fun(x,y);
  }
  
};


class Mesh {

private:
  GLuint vbo;
  GLuint vao;
  int size;
  int slide;
  GLuint method;

public:
  Mesh(float points[], int sz, int sld, string mthd) {
    
    //    vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sz*sld * sizeof(float), points, GL_STATIC_DRAW);
        
    //    vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, sld, GL_FLOAT, GL_FALSE, 0, NULL);

    size = sz;
    slide = sld;
    setPrimitive(mthd);

  }


  void draw() {
    glBindVertexArray(vao);
    glDrawArrays(method, 0, size);
  }


private:
  void setPrimitive(string mthd) {
    
    if(mthd=="GL_LINE_STRIP") {
      method = GL_LINE_STRIP;
    }
    else if(mthd=="GL_POINTS") {
      method = GL_POINTS;
      glEnable(GL_PROGRAM_POINT_SIZE);
      
    }
    else if(mthd=="GL_LINE_LOOP") {
      method = GL_LINE_LOOP;
    }
    else if(mthd=="GL_TRIANGLES") {
      method = GL_TRIANGLES;
    }
    else if(mthd=="GL_TRIANGLE_STRIP") {
      method = GL_TRIANGLE_STRIP;
    }
    else if(mthd=="GL_TRIANGLE_FAN") {
      method = GL_TRIANGLE_FAN;
    }
    else if(mthd=="GL_LINES_ADJACENCY") {
      method = GL_LINES_ADJACENCY;
    }
    else if(mthd=="GL_LINE_STRIP_ADJACENCY") {
      method = GL_LINE_STRIP_ADJACENCY;
    }
    else if(mthd=="GL_TRIANGLES_ADJACENCY") {
      method = GL_TRIANGLES_ADJACENCY;
    }
    else if(mthd=="GL_TRIANGLE_STRIP_ADJACENCY") {
      method = GL_TRIANGLE_STRIP_ADJACENCY;
    }
    
  }

  
};




class Scene {
public:
  
  GLuint shader_programme;

  vector<Mesh> meshes;
  

  Scene() {
    //    meshes = meshs;
    //this.root = root;
    initShaders();
  }
  
  void initShaders() {
    const char* vertex_shader =
      "#version 400\n"
      "in vec3 vp;"
      "uniform mat4 u_proj;"
      //"uniform mat4 u_view;"
      "void main() {"
      "  gl_Position = u_proj * vec4(vp, 1.0);"
      "  gl_PointSize = 3.0;"
      "}";
    
    const char* fragment_shader =
      "#version 400\n"
      "out vec4 frag_colour;"
      "void main() {"
      "  frag_colour = vec4(0.9, 0.0, 0.0, 1.0);"
      "}";
    

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    
    
    shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);


  }


  void addMesh(Mesh mesh) {
    meshes.push_back(mesh);
  }

    
  void draw() {
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glUseProgram(shader_programme);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, -10.0f, 10.0f);
    //    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    GLint projLoc = glGetUniformLocation(shader_programme, "u_proj");
    //GLint viewLoc = glGetUniformLocation(shader_programme, "u_view");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    //glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    

    vector<Mesh>::iterator it = meshes.begin();
    
    while(it != meshes.end()) {
      Mesh m = *it;
      m.draw();
      ++it;
    }
  }

};


class View
{

public:
  Scene *scene;

  View(Scene *s) {
    scene = s;
  }
  
  
  void draw() {
    scene->draw();
  }

 
};


class OpenGLWindow
{
private:
  GLFWwindow *window;

  View *view;
  
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
    window = glfwCreateWindow(1280, 960, "Hello", NULL, NULL);
    if (!window) {
      fprintf(stderr, "ERROR: could not open window with GLFW3\n");
      glfwTerminate();
      return;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(-width/2, -height/2, width, height);
    glOrtho(-20, 20, -20, 20,-10,10);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);	
  }


  void setView(View *v) {
    view = v;
  }

  void draw() {

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    cout << width << "\n";
    cout << height << "\n";
    glOrtho(-10,10,-10,10,-10,10);

    while(!glfwWindowShouldClose(window)){
      view->draw();
      // update other events like input handling 
      glfwPollEvents();
      // put the stuff we've been drawing onto the display
      glfwSwapBuffers(window);

    }
    glfwTerminate();   
  }



  
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
      	  static_cast<MyGLWindow<F,C>*>(glfwGetWindowUserPointer(w))->mouseButtonPressed(2*xpos/width, 2*ypos/height);
	  // func(xpos,ypos);
	  //cout << "Cursor Position is " << xpos << " : " << ypos << endl;
	}
    };

    cout << "here\n";
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
  }
  

  
  
};


/*

class MyGLWindow {
  TestIncremental *obj;

  MyGLWindow(TestIncremental *obj) {
    this->obj = obj;
  }

  void mouseButtonPressed(double x, double y) {
    obj->incrementHull(x,y);
  }

}
*/
