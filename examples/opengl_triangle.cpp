#include <iostream>
#include <cassert>
#include <math.h> 
#include <algorithm>
#include <vector>
#define pi 3.142857

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

using namespace std;

class View;
class Scene;
class Mesh;

class Mesh {

private:
  GLuint vbo;
  GLuint vao;
  int size;
  int slide;
  string method;

public:
  Mesh(float points[], int sz, int sld, string method) {
    
    
    //    vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sz*sld * sizeof(float), points, GL_STATIC_DRAW);
        
    //    vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, sz, GL_FLOAT, GL_FALSE, 0, NULL);

    size = sz;
    slide = sld;

  }


  void draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, size);    
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
      "void main() {"
      "  gl_Position = vec4(vp, 1.0);"
      "}";
    
    const char* fragment_shader =
      "#version 400\n"
      "out vec4 frag_colour;"
      "void main() {"
      "  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
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
    
    window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
    if (!window) {
      fprintf(stderr, "ERROR: could not open window with GLFW3\n");
      glfwTerminate();
      return;
    }
    glfwMakeContextCurrent(window);

  }


  void setView(View *v) {
    view = v;
  }

  void draw() {

    while(!glfwWindowShouldClose(window)){
      view->draw();
      // update other events like input handling 
      glfwPollEvents();
      // put the stuff we've been drawing onto the display
      glfwSwapBuffers(window);

    }
    glfwTerminate();   
  }  
  
};



int main() {


  OpenGLWindow w;
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  //glewInit();


  if(GLEW_OK != glewInit()) {
    cout<< "glew not initialized";
    return -1;
  }
  
  float points[] = {
		    0.0f,  0.5f,  0.0f,
		    0.5f, -0.5f,  0.0f,
		    -0.5f, -0.5f,  0.0f
  };

  Mesh mesh(points, 3, 3, "GL_LINE_STRIP");


  
  
  //  vector<Mesh> meshes;
  //meshes.push_back(mesh);
  
  Scene scene;
  scene.addMesh(mesh);

  View v(&scene);

  w.setView(&v);

  w.draw();
  
  return 0;
}


