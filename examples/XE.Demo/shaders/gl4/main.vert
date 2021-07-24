#version 410 core

uniform mat4 uProjViewModel;

layout(location = 0) in vec3 vsCoord;
layout(location = 1) in vec4 vsColor;

out vec4 fsColor;

void main() {
    gl_Position = vec4(vsCoord, 1.0) * uProjViewModel;
    fsColor = vsColor;
}
