#version 330 core

in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D texture0;

void main() {
    vec4 texColor = texture(texture0, texCoord);
    fragColor = texColor;
} 