#version 330 core

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    vec4 spotdirection;
    float spotcutoff;
};


in vec3 fNormal;
in vec4 fPosition;

const int MAXLIGHTS = 10;

uniform MaterialProperties material;
uniform LightProperties light[MAXLIGHTS];
uniform int numLights = 1;

/* texture */

out vec4 fColor;

void main()
{
    vec3 lightVec,viewVec,reflectVec;
    vec3 normalView;
    vec3 ambient,diffuse,specular;
    float nDotL,rDotV,sDotV,sDotN,cut;


   fColor = vec4(0,0,0,1);
   //vec4 ac = vec4(material.ambient[0], material.ambient[1], material.ambient[2], 1.0);
   //fColor = ac;

    for (int i=0;i<numLights;i++)
    {
        if (light[i].position.w!=0)
            lightVec = normalize(light[i].position.xyz - fPosition.xyz);
        else
            lightVec = normalize(-light[i].position.xyz);

        vec3 tNormal = fNormal;
        normalView = normalize(tNormal.xyz);
        nDotL = dot(normalView,lightVec);
	nDotL = -nDotL;
	
        viewVec = -fPosition.xyz;
        viewVec = normalize(viewVec);

        reflectVec = reflect(-lightVec,normalView);
        reflectVec = normalize(reflectVec);

        rDotV = max(dot(reflectVec,viewVec),0.0);
	sDotV = dot(-lightVec,normalize(light[i].spotdirection.xyz));
        sDotN = dot(lightVec,fNormal);
        cut = light[i].spotcutoff;
//	cut = -1.0;
	
	if(sDotV>cut) {
	    ambient = material.ambient * light[i].ambient;
            diffuse = material.diffuse * light[i].diffuse * max(nDotL,0);
            if (nDotL > 0.0) {
                specular = material.specular * light[i].specular * pow(rDotV,material.shininess);
	    }
            else {
                specular = vec3(0,0,0);
            }
         fColor = fColor + vec4((0.33f)*(ambient+diffuse+specular),1.0);
         }
      }
//    fColor = fColor;
    //gl_FragColor = fColor;
    //fColor = vec4(fTexCoord.s,fTexCoord.t,0,1);
}
B