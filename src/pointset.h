#ifndef _POINTSET_H_
#define _POINTSET_H_

#include "point2D.h"

class Pointset {
  vector<Point2D> s;

 public:

  Pointset(vector<Point2D> v) {
    s = v;
  }

    
  void sort_set(void) {
    sort(s);
  }

  // return 0 if all goes well, else return something ese #TODO
  int add_point(Point2D p) {
    s.push_back(p)
  }

  int compare( const  *vp1, const void *vp2) {
    
  }


  vector<Edge> grahm_impl() {
    sort_set();
    

    
  }


  vector<Edge> incremental_impl() {
    
  }

  
}
