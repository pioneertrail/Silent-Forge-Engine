#version 410 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture0;
uniform vec3 lightDir;     // Direction of the light (for directional light)
uniform vec3 viewPos;      // Camera position for specular calculation

void main() {
    // Base texture color
    vec4 texColor = texture(texture0, TexCoord);
    
    // Normalize vectors
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);  // Light coming from the light source
    
    // Ambient component - increased for better visibility
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * vec3(1.0);
    
    // Diffuse component
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    
    // Specular component - increased strength and shininess for more pronounced highlights
    float specularStrength = 0.8;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0); // Higher shininess
    vec3 specular = specularStrength * spec * vec3(1.0);
    
    // Combine results
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
} 