#version 140

in vec4 vPosition;
in vec4 vNormal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalmatrix;

out vec3 fNormal;
out vec4 fPosition;

 
//uniform vec4 vColor;
//out vec4 outColor;

void main()
{
    vec3 lightVec, viewVec, reflectVec;
    vec3 normalView;
    vec3 ambient, diffuse, specular;

    fPosition modelview*vPosition;
    gl_Position = projection * fPosition;

    gl_Position = projection * vPosition;
    outColor = vColor;
}
