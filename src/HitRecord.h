#ifndef _HITRECORD_H
#define _HITRECORD_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Material.h"



class HitRecord {

protected :
  glm::vec3 origin;
  glm::vec3 raydir;
  float t;
  glm::vec4 hitcoord;
  glm::vec4 hitnormal;
  string texName;
  glm::vec4 texcoord;
  util::Material material;

public:
  HitRecord() {
  }

  util::Material getmaterial() {
    return this->material;
  }
  void setmaterial(util::Material value) {
    this->material = value;
  }    

  void setT(float t) {
    this->t = t;
  }

  void sethitNormal(glm::vec4 v) {
    this->hitnormal = v;
  }
  
  void sethitCoord(glm::vec4 v) {
    this->hitcoord = v;
  }
  
  void settexName(string s) {
    this->texName = s;
  }
  
  void settexCoord(glm::vec4 v) {
    this->texcoord = v;
  }
  
  float getT() {
    return this->t;
  }
  
  glm::vec4 gethitNormal() {
    return this->hitnormal;
  }
  
  glm::vec4 gethitCoord() {
    return this->hitcoord;
  }
  
  string gettexName() {
    return this->texName;
  }
  
  glm::vec4 gettexCoord() {
    return this->texcoord;
  }
  


};
#endif
