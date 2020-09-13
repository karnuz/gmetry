#ifndef _POINTS_H_
#define _POINTS_H_

#include "gmetryexceptions.h"
using namespace std;


struct Triangle3D {

  Point3D a, b, c;

};

struct Triangle2D {

  Point2D a, b;

};



float determinant(float matrix[], int slide, int size)
{
  assert(size >= 1);

  if(size == 1) {
    return matrix[0];
  }
  elif (size == 2) {
    return matrix[0]*matrix[3] - matrix[1]*matrix[2];
  }
  elif (size == 3) {
    return matrix[0](matrix[4]*matrix[8] - matrix[5]*matrix[7]) + matrix[1](matrix[5]*matrix[6] - matrix[3]*matrix[8]) + matrix[2](matrix[3]*matrix[7] - matrix[4]*matrix[6]);
  }
  elif (size == 4) {
    matrix[0]*(matrix[5](matrix[10]*matrix[15] - matrix[11]*matrix[14]) + matrix[6](matrix[11]*matrix[13] - matrix[9]*matrix[15]) + matrix[7](matrix[9]*matrix[14] - matrix[10]*matrix[13]))
      + matrix[1]*(matrix[4](matrix[10]*matrix[15] - matrix[11]*matrix[14]) + matrix[6](matrix[11]*matrix[12] - matrix[8]*matrix[15]) + matrix[7](matrix[8]*matrix[14] - matrix[10]*matrix[12]))
      + matrix[2]*(matrix[4](matrix[9]*matrix[15] - matrix[11]*matrix[13]) + matrix[5](matrix[11]*matrix[12] - matrix[8]*matrix[15]) + matrix[7](matrix[8]*matrix[13] - matrix[9]*matrix[12]))
      + matrix[3]*(matrix[4](matrix[9]*matrix[14] - matrix[10]*matrix[13]) + matrix[5](matrix[10]*matrix[12] - matrix[8]*matrix[14]) + matrix[6](matrix[8]*matrix[13] - matrix[9]*matrix[12]));
  }
  elif(size > 5) {
    throw dimension_not_handled_exception;
  }

}



  


#endif
