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

    scene = new Scene();
  }
  
  //  vector<Mesh> meshes;
  //meshes.push_back(mesh);
  



  void addPoints2D(vector<Point2D> pts, int size, string primitive ) {

    
    float points[3*size];
    for (int i = 0; i < size; i++) {
      points[3*i] = pts[i].x;
      points[3*i + 1] = pts[i].y;
      points[3*i + 2] = 0.0f;
    }
    
    
    

    /*    
    float points[] = {
		      0.0f,  0.5f,  0.0f,
		      0.5f, -0.5f,  0.0f,
		      -0.5f, -0.5f,  0.0f,
		      0.2f, -0.8f, 0.0f,
		      0.9f, 0.5f, 0.0f
    };
    size = 5;
    */
    

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

};
