#include "glfunctions.h"
#include "point2d.h"
#include "line.h"
//#include "face.h"


class GLScene
{
  OpenGLWindow w;
  Scene *scene;
  
  
public:
  GLScene() {

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    //glewInit();

    
    if(GLEW_OK != glewInit()) {
      cout<< "glew not initialized\n";
      return;
    }

    //    w = *(new OpenGLWindow(ortholeft, orthoright, orthodown, orthoup));

    scene = new Scene();
  }
  
  //  vector<Mesh> meshes;
  //meshes.push_back(mesh);
  


  template <typename T>
  void addPoints2D(vector<Point2D<T>> pts, int size, string primitive ) {

    
    float points[3*size];
    for (int i = 0; i < size; i++) {
      points[3*i] = pts[i].x;
      points[3*i + 1] = pts[i].y;
      points[3*i + 2] = 0.0f;
    }

    cout<< size << "\n";
    int slide = 3;
    Mesh mesh(points, size, 3, primitive);
    scene->addMesh(mesh);
  }

  template <typename T>
  void addPoints2D(Point2D<T> pts[], int size, string primitive ) {

    
    float points[3*size];
    for (int i = 0; i < size; i++) {
      points[3*i] = pts[i].x;
      points[3*i + 1] = pts[i].y;
      points[3*i + 2] = 0.0f;
    }

    cout<< size << "\n";
    int slide = 3;
    Mesh mesh(points, size, 3, primitive);
    scene->addMesh(mesh);
  }


  void draw() {

    View v(scene);
    
    w.setView(&v);

    w.draw();

    return;
  }



  template <typename F, typename C>
  void setClickPositionCallback(F func, C *obj) {

    w.setClickPositionCallback(func, obj);
    
  }



};
