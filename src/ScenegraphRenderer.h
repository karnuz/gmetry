#ifndef _SCENEGRAPHRENDERER_H
#define _SCENEGRAPHRENDERER_H


#include "IVertexData.h"
#include "ObjectInstance.h"
#include <map>
#include "Light.h"
#include "SGNode.h"
#include "Material.h"

template <typename VertexType> class SGNode;

template <typename VertexType>
class ScenegraphRenderer {

 public:
  util::OpenGLFunctions gl;

  util::ShaderLocationsVault shaderLocations;

  map<string,string> shaderVarsToVertexAttribs;

  //  map<string, TextureObject> textures;

  map<string, util::ObjectInstance*> meshRenderers;

  util::ShaderProgram program;

  ScenegraphRenderer(util::OpenGLFunctions ogl, util::ShaderLocationsVault shaderLocations, map<string, string> shaderVarsToAttribs, util::ShaderProgram shaderProg) {

    this->gl = ogl;
    this->shaderVarsToVertexAttribs = shaderVarsToAttribs;
    //    this->meshRenderers = new map<string, util::ObjectInstance*>;
    this->shaderLocations = shaderLocations;
    this->program = shaderProg;
    //this->textures = new Map<string, TextureObject>();
  }


  void addMesh(string meshName, util::PolygonMesh<VertexType>& mesh) {
    if(meshRenderers.find(meshName) != meshRenderers.end()) {
      return;
    }
    if(mesh.getVertexCount() <= 0){
      return;
    }

    cout << "initialized mesh\n";
    util::ObjectInstance *obj = new util::ObjectInstance(meshName);
    obj->initPolygonMesh<VertexAttrib>(
				       gl, //the gl wrapper
				       program, //the shader program
				       shaderLocations, //the shader locations
				       shaderVarsToVertexAttribs, //the shader variable -> attrib map
				       mesh); //the actual mesh object

    cout << "initialized mesh\n";
    this->meshRenderers[meshName] = obj;
  }

  

  void draw(SGNode<VertexType>* root, stack<glm::mat4> modelview) {
    root->draw(this, modelview);
  }



  void renderLights(vector<util::Light> lights) {
    for(int i = 0; i < lights.size(); i++) {
      util::Light l = lights[i];
      string stri = to_string(i);
      string ambientLocation = "light[" + stri + "].ambient";
      string diffuseLocation = "light[" + stri + "].diffuse";
      string specularLocation = "light[" + stri + "].specular";
      gl.glUniform3fv(this->shaderLocations.getLocation(ambientLocation), 1, glm::value_ptr(l.getAmbient()));
      gl.glUniform3fv(this->shaderLocations.getLocation(diffuseLocation), 1, glm::value_ptr(l.getDiffuse()));
      gl.glUniform3fv(this->shaderLocations.getLocation(specularLocation), 1, glm::value_ptr(l.getSpecular()));
      string lightPositionLocation = "light[" + stri + "].position";
      gl.glUniform4fv(this->shaderLocations.getLocation(lightPositionLocation), 1, glm::value_ptr(l.getPosition()));
    

      if (l.getSpotCutoff() > 0) {
	string lightspotcutoffangle = "light[" + stri + "].spotcutoff";
	float cutoff = glm::cos(glm::radians(l.getSpotCutoff()));
	gl.glUniform1f(this->shaderLocations.getLocation(lightspotcutoffangle), cutoff);
	
	string lightSpotDirection = "light[" + stri + "].spotdirection";
	gl.glUniform4fv(this->shaderLocations.getLocation(lightSpotDirection), 1, glm::value_ptr(l.getSpotDirection())); 
      }
    }
  }



  void dispose() {
    for(auto mesh: this->meshRenderers.values()) {
      mesh.cleanup();
    }
  }



  void drawMesh(string meshName, util::Material material, glm::mat4 transformation) {

    //    cout << "in draw mesh: " << meshName << "\n";
    glm::mat4 normalmatrix = transformation;
    normalmatrix = glm::transpose(normalmatrix);
    normalmatrix = glm::inverse(normalmatrix);

    gl.glUniformMatrix4fv(this->shaderLocations.getLocation("normalmatrix"), 1, false, glm::value_ptr(normalmatrix));
    gl.glUniform3fv(this->shaderLocations.getLocation("material.ambient"), 1, glm::value_ptr(material.getAmbient()));
    gl.glUniform3fv(this->shaderLocations.getLocation("material.diffuse"), 1, glm::value_ptr(material.getDiffuse()));
    gl.glUniform3fv(this->shaderLocations.getLocation("material.specular"), 1, glm::value_ptr(material.getSpecular()));
    gl.glUniform1f(this->shaderLocations.getLocation("material.shininess"), material.getShininess());
    gl.glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, false,  glm::value_ptr(transformation));


    /*
    cout << "normalmatrix: ";
    for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 4; j++) {
	cout << normalmatrix[i][j] << ",";
      }
      cout << "    ";
    }
    cout << "\n";
    */

    
    meshRenderers[meshName]->draw(gl);
  }
  /*

  void addMesh<VertexType>(stringname meshName, PolygonMesh<VertexType> mesh) {

    auto const it = this->meshRenderers.find(meshName);
    if (it != this->meshRenderers.end())
      return;
    
    //verify that the mesh has all the vertex attributes as specified in the map
    if (mesh.getVertexCount() <= 0)
      return;
    let vertexData: K = mesh.getVertexAttributes()[0];
    for (let [s, a] of this.shaderVarsToVertexAttribs) {
      if (!vertexData.hasData(a))
	throw new Error("Mesh does not have vertex attribute " + a);
    }
    let renderableMesh: RenderableMesh<K> = new RenderableMesh<K>(this.gl, name);
    
    renderableMesh.initMeshForRendering(this.shaderVarsToVertexAttribs, mesh);
    
    this.meshRenderers.set(meshName, renderableMesh);
  }
  */
  
  
  
};

#endif
