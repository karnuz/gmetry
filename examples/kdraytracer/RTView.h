#ifndef _RTVIEW_H
#define _RTVIEW_H


#include "Scenegraph.h"
#include "HitRecord.h"
#include "myscene.h"
#include "math.h"
#include <ctime>
#include "Light.h"
#include <fstream>

/*
  This is a wrapper class around Ligths.
  Though there is no use of this wrapper right now,
  we envisage its usefulness in future.
 */
class LightInfo {
public:
  util::Light light;
  LightInfo(util::Light light) {
    this->light = light;
  }
};

/*
This class implements a ray tracer.
 */

class RayTracer {
  
public:
  bool debug = false;                         // flag for printing some debugging information
  Scenegraph<VertexAttrib>* s;                // pointer to the scene on which ray tracing is done
  std::stack<glm::mat4> modelview =
    std::stack<glm::mat4>{{glm::mat4(1.0)}};  // view matrix for the scene
  const static int width = 100;               // width of image generated in pixels
  const static int height = 100;              // height of image generated in pixels
  float image[width][height][4];              // array to store color information for each pixel
  glm::vec4 clearColor = glm::vec4(0,0,0,1);  // background color of the scene
  float fov = 90;                             // field of view parameter
  float z = -(width)/(2*tan(fov*3.14/360));   // calculating the distance of screen on which
                                              // scene will be projected
  std::vector<LightInfo> lights;              // vector of lights in the scene
  
  RayTracer() {
  }

  /*
    the method sets the scenegraph on which ray tracing is to be done.
    @params sg : pointer to scenegraph
   */
  void setScenegraph(Scenegraph<VertexAttrib>* sg) {
    this->s = sg;
  }

  /*
    the main method which casts rays for each pixel and saves a text file encoding colors.
   */
  void generateImage() {
    
    double duration;
    std::clock_t start;
    start = std::clock();
    cout << "generating ray tracer image\n";
    
    std::vector<util::Light> alllights = this->s->collectLights(this->modelview);

    for(auto l:alllights) {
      this->lights.push_back(LightInfo(l));
    }
    
    glm::mat4 mv = glm::lookAt(glm::vec3(9.0f, 7.3f, 3.0f),
  		   glm::vec3(0.0f, 0.0f, 0.0f),
  		   glm::vec3(0.0f, 1.0f, 1.0f));
    

    modelview.push(mv);
    
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
	
	int p = x - width/2;
	int q = (height/2 - y);
	
	glm::vec4 result =  this->raytrace(p,q,this->modelview);
	float base = result[3];
	glm::vec4 color = glm::vec4(result[0] * 255/base, result[1] * 255/base, result[2] * 255/base, 255);

	image[y][x][0] = color[0];
	image[y][x][1] = color[1];
	image[y][x][2] = color[2];
	image[y][x][3] = color[3];
      }
    }
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    cout<< "Time taken Ray Tracer: "<< duration <<"\n\n";

    
    ofstream myfile ("colors.txt");
    if (myfile.is_open()) {
    myfile << width << " ";
    myfile << height << "\n";
    for(int i = 0; i < height; i ++){
	for(int j = 0; j < width; j ++){
	  for(int k = 0; k < 4; k ++){
	    myfile << image[i][j][k] << " " ;
	  }
	}
	myfile << "\n";
      }
    }
  }


  /*
    This method returns the color for an input pixel.
    @params p,q : input pixel coordinate
    @params modelview : view matrix for the scene
    @bounce : currently of no use, but will be used later on when adding reflection etc.
   */  
  glm::vec4 raytrace(int p, int q, std::stack<glm::mat4> modelview, int bounce = 0) {
    HitRecord ht = *(new HitRecord());
    if(bounce < 5) {
      glm::vec4 ray = glm::vec4(p,q,this->z,0);
      ht = this->raycast(glm::vec4(0,0,0,1), ray, modelview);
      return this->shader(ht, modelview, ray, bounce);
    } else {
      return this->clearColor;
    }
  }

  /*
    Returns a hit record containing information about the coordinate, normal and material
    of the spot the ray hit.
    @params origin : origin of ray
    @params direction : direction of ray
    @params modelview : view matrix of the scene.
   */
  HitRecord raycast(glm::vec4 origin, glm::vec4 direction, stack<glm::mat4> modelview) {
    direction = glm::normalize(direction);
    return this->s->raycast(origin, direction, modelview);
  }

  /*
    returns sign of a float. returns 1 if input is 0
   */
  int sign(float x) {  
    if(x < 0) return -1;
    return 1;
  }
  

  /*
    shader for the ray tracer.
    @params hit : HitRecord containing info about hit coordinate, normal and material
    @params modelview : view matrix of scene
    @params incidentray : direction of incident ray
    @params bounce : currently of no use, but will be used later on when adding reflection etc.
    @returns color of the hit spot
   */
  glm::vec4 shader(HitRecord hit, stack<glm::mat4> modelview, glm::vec4 incidentray, int bounce = 0) {

    
    if (hit.getT()>0) {

      util::Material material = hit.getmaterial();
      glm::vec4 result = glm::vec4(0,0,0,1);

      //      cout << "in shader\n";
      for (int i=0; i < this->lights.size(); i++) {

	//	cout << "there is a light\n";
	
	util::Light light = this->lights[i].light; // have to fix this
	glm::vec4 position= hit.gethitCoord();
	glm::vec3 lightVec3 = glm::vec3(0,0,0);
	glm::vec4 lightPos = light.getPosition();

	if(debug) {
	  cout << position[0] << " " << position[1] << " " << position[2] << " :position\n";
	}
	
	// 1. lightvector
	if (lightPos[3] != 0) {
	  lightVec3[0] = lightPos[0] - position[0];
	  lightVec3[1] = lightPos[1] - position[1];
	  lightVec3[2] = lightPos[2] - position[2];
	} else {
	  lightVec3[0] = -lightPos[0];
	  lightVec3[1] = -lightPos[1];
	  lightVec3[2] = -lightPos[2];
	}
	lightVec3 = glm::normalize(lightVec3);

	if(debug) {
	  cout << "light position here: " << lightPos[0] << " " << lightPos[1] << " " << lightPos[2] << "\n";
	  cout << "light Vector here: " << lightVec3[0] << " " << lightVec3[1] << " " << lightVec3[2] << "\n";
        }
	
	
	// 2. lightDirection
	glm::vec4 lightDir = glm::normalize(light.getSpotDirection());
	glm::vec3 lightDir3 = glm::vec3(lightDir[0],lightDir[1],lightDir[2]);
	
	// 3. Normal to the face
	glm::vec4 fNormal = hit.gethitNormal();
	glm::vec3 fNormal3 = glm::vec3(fNormal[0],fNormal[1],fNormal[2]);
	fNormal3 = glm::normalize(fNormal3);

	if(debug) {
	  cout << fNormal3[0] << " " << fNormal3[1] << " " << fNormal3[2] << " :normal\n";
	}
	
	// 4. viewing vector
	glm::vec3 viewVec3 = glm::vec3(-position[0],-position[1],-position[2]);
	viewVec3 = glm::normalize(viewVec3);
	
	// 5. Reflection Vector
	glm::vec3 reflectVec3;
	glm::vec3 nlightVec3 = glm::vec3(-lightVec3[0], -lightVec3[1], -lightVec3[2]);
	float sc = 2.0f*glm::dot(nlightVec3,fNormal3);
	reflectVec3 = fNormal3*sc;;
	reflectVec3 = nlightVec3 + reflectVec3;
	reflectVec3 = glm::normalize(reflectVec3);

	// 6. IncidentReflection Vector
	glm::vec3 reflectIncVec3;
	
	glm::vec3 incidentray3 = glm::vec3(incidentray[0],incidentray[1],incidentray[2]);
	incidentray3 = glm::normalize(incidentray3);
	sc = 2.0f*glm::dot(incidentray3,fNormal3);
	reflectIncVec3 = fNormal3 * sc;
	reflectIncVec3 = incidentray3 + reflectIncVec3;
	reflectIncVec3 = glm::normalize(reflectIncVec3);


	//get Absorbent color.
	glm::vec3 ads =  this->getAbsorbentColor(reflectVec3, viewVec3, lightVec3, lightDir3, light, fNormal3, material);
	result = result + glm::vec4(ads[0], ads[1], ads[2], 0);
	result[3] = 3.0;

	if(debug) {
	  cout << "result color: " << result[0] << " " << result[1] << " " << result[2] << "\n\n";
	}
        
      }
            
      return result;
    } else {
      return glm::vec4(0, 0, 0, 255);
    }
  }


  /*
    computes the abosorbent color of the pixel given the input, reflection, light vector etc.
    @params reflectVec3 : vector representing reflected light
    @params viewVec3 : vector representing view direction for the hit coordinate
    @params lightVec3 : vector representing direction light is pointing in
    @params lightDir3 : vector representing direction in which light approaches hit coordinate
    @params light : Light object
    @params fNormal3 : normal to the hit coordinate
    @params material : material of the hit coordinate
    @returns : abosorbent color of the hit coordinate for this scene
   */

  glm::vec3 getAbsorbentColor(glm::vec3 reflectVec3,
			      glm::vec3 viewVec3,
			      glm::vec3 lightVec3,
			      glm::vec3 lightDir3,
			      util::Light light,
			      glm::vec3 fNormal3,
			      util::Material material) {

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 nlightVec3;

    if(debug) {
      cout << "light Vector: " << lightVec3[0] << " " << lightVec3[1] << " " << lightVec3[2] << "\n";
      cout << fNormal3[0] << " " << fNormal3[1] << " " << fNormal3[2] << " :fnormal\n";
    }
    
    nlightVec3 = lightVec3*(-1.0f);
    float rDotV = glm::dot(reflectVec3,viewVec3);
    rDotV = (rDotV > 0.0) ? rDotV:0.0;
    float sDotV = glm::dot(nlightVec3, lightDir3);
    float nDotL = (-1.0f)*glm::dot(fNormal3, lightVec3);
    float cut = -1.0;

    glm::vec3 lamb = light.getAmbient();
    glm::vec4 mamb = material.getAmbient();
    ambient = glm::vec3(lamb[0] * mamb[0], lamb[1]*mamb[1], lamb[2]*mamb[2]);

    if(debug) {
      cout << "ambient: " << ambient[0] << " " << ambient[1] << " " << ambient[2] << "\n";
      cout << sDotV << " :sdotv\n";
    }
    
    if(sDotV>=cut) {
      glm::vec3 ldif = light.getDiffuse();
      glm::vec4 mdif = material.getDiffuse();
      diffuse = glm::vec3(ldif[0] * mdif[0], ldif[1]*mdif[1], ldif[2]*mdif[2]);
      float mndotl = (nDotL > 0.0)? nDotL:0.0;
      diffuse = diffuse * (mndotl);
      if(debug) {
	      cout << "diffuse: " << diffuse[0] << " " << diffuse[1] << " " << diffuse[2] << "\n";
	cout << nDotL <<" :ndotl\n";
      }
      if(nDotL > 0) {
	glm::vec3 lspec = light.getSpecular();
	glm::vec4 mspec = material.getSpecular();
	specular = glm::vec3(lspec[0] * mspec[0], lspec[1]*mspec[1], lspec[2]*mspec[2]);
	if(debug) {  
	  cout << "rDotV: " << rDotV << "\n";
	}
	specular = specular * pow(rDotV,material.getShininess());
	if(debug) {
	  cout << "specular: " << specular[0] << " " << specular[1] << " " << specular[2] << "\n";
	}
      } else {
	specular = glm::vec3(0,0,0);
      }
    }
    
    glm::vec3 ad(0,0,0);
    ad = ambient + diffuse;
    ad = ad + specular;
    return ad;
  }
};



#endif
