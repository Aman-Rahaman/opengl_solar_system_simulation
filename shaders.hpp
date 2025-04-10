
#ifndef SHADERS_CODE_HPP
#define SHADERS_CODE_HPP



const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;   // Vertex position
    layout(location = 1) in vec3 aNormal; // Vertex normal
    layout(location = 2) in vec2 aTexCoord; // Texture coordinates
    
    out vec3 FragPos;  // Position of fragment (for lighting)
    out vec3 Normal;   // Normal vector (to interpolate)
    out vec2 TexCoord; // Pass to fragment shader
    
    uniform mat4 transform;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main() {
        // Transform vertex position to world space
        FragPos = vec3(transform * vec4(aPos, 1.0));
    
        // Transform normal to world space
        Normal = normalize(mat3(transpose(inverse(transform))) * aNormal);
    
        TexCoord = aTexCoord; // Pass texture coordinates
    
        // Output final vertex position
        gl_Position = projection * view * transform * vec4(aPos, 1.0);
    }
    )";
    

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 FragPos;
    in vec3 Normal;
    in vec2 TexCoord; // Received from vertex shader
    
    out vec4 FragColor;
    
    // Uniforms for light and material properties
    uniform vec3 lightPos;   // Position of the light
    uniform vec3 viewPos;    // Camera position
    uniform vec3 lightColor; // Light color
    uniform sampler2D Texture; // Texture sampler
    
    void main()
    {
        // Normalize the normal (since interpolation can change its length)
        vec3 norm = normalize(Normal);
        
        // Compute light direction
        vec3 lightDir = normalize(lightPos - FragPos);
    
        // 1. Ambient Lighting
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * lightColor;
    
        // 2. Diffuse Lighting (Lambertian reflection)
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
    
        // Sample the texture color
        vec3 textureColor = texture(Texture, TexCoord).rgb;
    
        // Apply lighting to texture
        vec3 result = (ambient + diffuse) * textureColor;
        FragColor = vec4(result, 1.0);
    }
    )";
    


void checkShaderCompilation(GLuint shader, const char* type) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader " << type << " compilation failed: " << infoLog << std::endl;
    }
}



#endif