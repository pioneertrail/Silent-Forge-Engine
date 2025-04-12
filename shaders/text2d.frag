#version 330 core
in vec2 TexCoord;
in vec3 Color;
out vec4 FragColor;

uniform sampler2D text;

void main() {
    float alpha = texture(text, TexCoord).r;
    FragColor = vec4(Color, alpha);
} 