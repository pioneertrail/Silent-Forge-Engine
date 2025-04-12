#version 410 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec3 textColor;

void main() {
    vec4 sampled = texture(textTexture, TexCoord);
    // For white text on transparent background bitmap fonts
    FragColor = vec4(textColor, 1.0) * sampled;
    
    // If your font has colors and transparency already
    // FragColor = sampled * vec4(textColor, 1.0);
} 