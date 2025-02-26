#version 410 core

in vec3 vertCoord;
in float vertCoordZ;
in vec4 vertColor;

out vec4 fragColor;

uniform mat4 uMvp;

void main() {
    // GLSL matrix-vector multiplication performs the correct linear-algebra operation
    // matrix-vector will multiply each row of the matrix with the column-vector at the right
    // vector-matrix will multiply the row-vector at the left with each column of the matrix.

    // vector-matrix should be more efficient because we are using matrices column-major order
    // wich is more efficient because of cache locality
    gl_Position = vec4(vertCoord.xy, vertCoord.z + vertCoordZ, 1.0) * uMvp;

    fragColor = vertColor;
}