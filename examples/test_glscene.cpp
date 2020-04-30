#include "drawutils.h"
#include <vector>
#include "point2d.h"

int main() {

  Point2D p1 = {10,10}, p2 = {-0.1,-0.9}, p3 = {-0.9,-0.5}, p4 = {-0.9,-0.1}, p5 = {5,16}, p6 = {6,8}, p7 = {5,4.4}, p8 = {3,2}, p9 = {4,4}, p10 = {3,2}, p11 = {-1,5}, p12 = {0.5,0.5};

    Point2D points[] = {p3,p10,p2,p4,p7,p5,p9,p1,p6,p8};
    //Point2D pointsa[] = {p1,p3,p4};
    vector<Point2D> pointsvec(points, points + sizeof(points) / sizeof(Point2D) );
    //Point2D pointsb[] = {p1,p2,p12};
    //vector<Point2D> pointsvecb(pointsb, pointsb + sizeof(pointsb) / sizeof(Point2D) ); 

    GLScene s;
    s.addPoints2D(pointsvec, 10, "GL_LINE_LOOP");
    //    s.addPoints2D(pointsvecb, 2, "GL_LINE_LOOP");

    s.draw();
    
    return 0;
}
