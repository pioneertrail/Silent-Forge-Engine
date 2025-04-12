#version 410 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec3 textColor;

void main() {
    vec4 sampled = texture(textTexture, TexCoord);
    FragColor = vec4(textColor, 1.0) * sampled;
}
