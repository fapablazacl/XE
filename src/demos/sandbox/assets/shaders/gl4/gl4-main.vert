#version 410 core

layout(location = 0) in vec3 vertCoord;
layout(location = 1) in vec4 vertColor;
layout(location = 2) in vec3 vertNormal;
layout(location = 3) in vec2 vertTexCoord;

out vec4 fragColor;
out vec3 fragNormal;
out vec2 fragTexCoord;

/*
uniform mat4 m_model;
uniform mat4 m_view;
uniform mat4 m_proj;
*/

uniform mat4 m_mvp;

void main() {
    /* const mat4 m_mvp = m_proj * m_view * m_model; */
    gl_Position = m_mvp * vec4(vertCoord, 1.0);
    fragColor = vertColor;
    fragNormal = vertNormal;
    fragTexCoord = vertTexCoord;
}
