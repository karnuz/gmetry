#version 140

in vec4 vPosition;
uniform vec4 vColor;
uniform mat4 projection;
uniform mat4 modelview;
out vec4 fPosition;
out vec4 outColor;

void main()
{
    fPosition = modelview * vPosition;
    gl_Position = projection * fPosition;
    outColor = vColor;
}
