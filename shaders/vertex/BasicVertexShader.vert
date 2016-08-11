#version 410

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

uniform mat4 transform;

smooth out vec3 theColor;

void main()
{
   gl_Position = transform * vec4(inPosition, 1.0);
   theColor = inColor;
}
