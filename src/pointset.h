#ifndef _POINTSET_H_
#define _POINTSET_H_

#include "points.h"

class Pointset {
  vector<Point> s;

 public:

  Pointset(vector<Point> v) {
    s = v;
  }

    
  void sort_set(void) {
    sort(s);
  }

  // return 0 if all goes well, else return something ese #TODO
  int add_point(Point p) {
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
