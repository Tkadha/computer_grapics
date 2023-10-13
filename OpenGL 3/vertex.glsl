#version 330 core

layout (location = 0) in vec3 in_Position; 
layout (location = 1) in vec3 in_Color;

out vec3 out_Color; 

uniform mat4 transform;

void main(void)
{
	gl_Position = transform * vec4(in_Position.x,in_Position.y,in_Position.z, 1.0f);
	out_Color = in_Color;
}