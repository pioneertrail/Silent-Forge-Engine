#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
// Instance data (model matrix)
layout (location = 3) in vec4 aModel1;
layout (location = 4) in vec4 aModel2;
layout (location = 5) in vec4 aModel3;
layout (location = 6) in vec4 aModel4;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    // Construct model matrix from instance data
    mat4 model = mat4(
        aModel1,
        aModel2,
        aModel3,
        aModel4
    );

    // Calculate position and normal
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
} 