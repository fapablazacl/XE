#version 410 core

uniform mat4 uProjViewModel;
uniform mat4 uView;
uniform mat4 uModel;

uniform vec3 uLightDirection = normalize(vec3(0.25, -1.0, 0.125));

layout(location = 0) in vec3 vsCoord;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec4 vsColor;

out vec4 fsColor;

void main() {
    // compute fragment position
    gl_Position = vec4(vsCoord, 1.0) * uProjViewModel;

    // compute diffuse color component
    float d = dot(uLightDirection, (vec4(vsNormal, 0.0) * uModel).xyz);
    vec4 diffuse = vsColor * d;

    fsColor = diffuse;
}
