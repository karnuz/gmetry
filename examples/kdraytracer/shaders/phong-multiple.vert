#version 330 core



in vec4 vPosition;
in vec4 vNormal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalmatrix;
out vec3 fNormal;
out vec4 fPosition;

void main()
{
    vec3 lightVec,viewVec,reflectVec;
    vec3 normalView;
    vec3 ambient,diffuse,specular;
    float nDotL,rDotV;

    fPosition = modelview * vec4(vPosition.xyzw);
    
    gl_Position = projection * fPosition;


    vec4 tNormal = normalmatrix * vNormal;
    fNormal = normalize(tNormal.xyz);


}
