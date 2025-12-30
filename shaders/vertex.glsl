#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 uMVP;

out vec4 vColor;

void main()
{
    gl_Position = uMVP * aPos;
    vColor = aColor;
	gl_PointSize = 2.0;
}
