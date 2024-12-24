#version 410 core

in vec4 fragColor;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform sampler2D texture0;

out vec4 finalColor;

void main() {
    finalColor = fragColor * texture(texture0, fragTexCoord);
}