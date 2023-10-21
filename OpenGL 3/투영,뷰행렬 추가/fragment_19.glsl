#version 330 core

in vec3 out_Color; 
out vec4 FragColor;

uniform vec3 setcolor;
void main(void)
{
	FragColor = vec4 (setcolor, 1.0);
}