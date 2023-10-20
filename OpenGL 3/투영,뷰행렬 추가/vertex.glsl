#version 330 core

layout (location = 0) in vec3 in_Position; 
layout (location = 1) in vec3 in_Color;

out vec3 out_Color; 

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * view * transform * vec4(in_Position, 1.0f);
	out_Color = in_Color;
}