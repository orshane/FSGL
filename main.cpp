#include "fsgl.hpp"
#include <cstdio>
#include <cstring>
#include <cmath>

using namespace fsgl;

// Vertex shader with lighting
const char* vertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uLightDir;

out vec3 vNormal;
out vec3 vLightDir;
out vec3 vFragPos;

void main() {
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vLightDir = uLightDir;
    vFragPos = vec3(uModel * vec4(aPos, 1.0));
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";

// Fragment shader with basic lighting and shadow
const char* fragmentShader = R"(
#version 330 core
in vec3 vNormal;
in vec3 vLightDir;
in vec3 vFragPos;

uniform vec3 uLightColor;
uniform vec3 uObjectColor;
uniform float uAmbientStrength;
uniform float uShadowStrength;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(vLightDir);
    
    // Ambient lighting
    float ambient = uAmbientStrength;
    
    // Diffuse lighting
    float diffuse = max(dot(norm, lightDir), 0.0);
    
    // Simple shadow calculation based on Y position
    float shadow = 1.0;
    if(vFragPos.y < -0.4) {
        shadow = uShadowStrength;
    }
    
    vec3 result = (ambient + diffuse) * uLightColor * uObjectColor * shadow;
    FragColor = vec4(result, 1.0);
}
)";

// Cube vertices (position + normal)
float cubeVertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    // Left face
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    // Right face
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f
};

unsigned int cubeIndices[] = {
    0,1,2, 2,3,0,    // Front
    4,5,6, 6,7,4,    // Back
    8,9,10, 10,11,8, // Left
    12,13,14, 14,15,12, // Right
    16,17,18, 18,19,16, // Top
    20,21,22, 22,23,20  // Bottom
};

// Shadow plane vertices (semi-transparent circle/ellipse)
float shadowVertices[] = {
    -0.8f, -0.49f, -0.6f,
     0.8f, -0.49f, -0.6f,
     0.8f, -0.49f,  0.6f,
    -0.8f, -0.49f,  0.6f,
    -0.6f, -0.49f, -0.8f,
     0.6f, -0.49f, -0.8f,
     0.6f, -0.49f,  0.8f,
    -0.6f, -0.49f,  0.8f
};

unsigned int shadowIndices[] = {
    0,1,2, 2,3,0,  // Outer quad
    4,5,6, 6,7,4   // Inner quad for better shadow shape
};

float angle = 0.0f;
fsShader shader;
fsVertexArray cubeVAO, shadowVAO;
fsVertexBuffer cubeVBO, shadowVBO;
fsIndexBuffer cubeEBO, shadowEBO;

void renderFrame() {
    // Clear screen
    fsClear(0.1f, 0.1f, 0.15f);
    fsClearDepth();
    
    // Update rotation angle
    angle += 0.015f;
    
    // Setup matrices
    float model[16];
    float view[16];
    float projection[16];
    
    // MODEL MATRIX: Cube centered at origin, spinning right to left
    fsMatrixIdentity(model);
    fsMatrixRotateY(model, angle);      // Spins right to left
    float temp[16];
    memcpy(temp, model, sizeof(float) * 16);
    fsMatrixRotateX(temp, angle * 0.2f); // Slight tilt
    memcpy(model, temp, sizeof(float) * 16);
    
    // VIEW MATRIX: Camera at corner view
    fsMatrixLookAt(view, 
                   2.8f, 2.2f, 3.8f,    // Camera position
                   0.0f, 0.0f, 0.0f,    // Looking at center
                   0.0f, 1.0f, 0.0f);   // Up vector
    
    // PROJECTION MATRIX
    fsMatrixPerspective(projection, 45.0f, 800.0f/600.0f, 0.1f, 100.0f);
    
    // Draw shadow on the ground plane
    float shadowModel[16];
    fsMatrixIdentity(shadowModel);
    fsSetUniformMatrix4(shader, "uModel", shadowModel);
    fsSetUniformMatrix4(shader, "uView", view);
    fsSetUniformMatrix4(shader, "uProjection", projection);
    fsSetUniformFloat3(shader, "uObjectColor", 0.05f, 0.05f, 0.1f);
    fsSetUniformFloat(shader, "uAmbientStrength", 0.8f);
    fsSetUniformFloat(shader, "uShadowStrength", 1.0f);
    
    fsBindVertexArray(shadowVAO);
    fsDrawElements(FS_TRIANGLES, 12);
    
    // Draw the filled cube with lighting
    fsSetUniformMatrix4(shader, "uModel", model);
    fsSetUniformFloat3(shader, "uObjectColor", 0.7f, 0.3f, 0.2f);  // Warm orange/red
    fsSetUniformFloat(shader, "uAmbientStrength", 0.3f);
    fsSetUniformFloat(shader, "uShadowStrength", 0.4f);  // Shadow strength on bottom
    
    fsBindVertexArray(cubeVAO);
    fsDrawElements(FS_TRIANGLES, 36);
    
    // Draw subtle grid on ground using fsDrawLine (2D, but appears 3D through projection)
    fsSetColor(0.3f, 0.3f, 0.4f);
    fsSetLineWidth(1.0f);
    
    // Draw a ground reference grid (these are 2D lines that will be transformed by the view/projection)
    // Note: In a full 3D scene, you'd want 3D lines, but this gives a reference
    for(float i = -1.5f; i <= 1.5f; i += 0.5f) {
        // These are drawn in screen space as a simple reference
        // For a proper 3D grid, you'd need 3D line drawing in your FSGL
    }
}

int main() {
    // Initialize
    if (fsInit() != FSOK) {
        printf("Failed to init GLFW\n");
        return -1;
    }
    
    fsWindow window = fsNewWindow("Filled Spinning Cube with Lighting & Shadow", 800, 600);
    if (!window) {
        printf("Failed to create window\n");
        fsDrop();
        return -1;
    }
    
    fsSetContext(window);
    fsInitContext();
    
    // Enable depth testing and blending
    fsEnableDepthTest();
    fsEnableBlend();
    fsSetBlendAlpha();
    
    // Create shader
    shader = fsCreateShader(vertexShader, fragmentShader);
    fsUseShader(shader);
    
    // Create cube buffers
    cubeVAO = fsCreateVertexArray();
    cubeVBO = fsCreateVertexBuffer(cubeVertices, sizeof(cubeVertices));
    cubeEBO = fsCreateIndexBuffer(cubeIndices, 36);
    
    fsBindVertexArray(cubeVAO);
    fsBindVertexBuffer(cubeVBO);
    fsBindIndexBuffer(cubeEBO);
    fsSetVertexAttribute(0, 3, 6 * sizeof(float), 0);        // Position
    fsSetVertexAttribute(1, 3, 6 * sizeof(float), 3 * sizeof(float)); // Normal
    
    // Create shadow buffers
    shadowVAO = fsCreateVertexArray();
    shadowVBO = fsCreateVertexBuffer(shadowVertices, sizeof(shadowVertices));
    shadowEBO = fsCreateIndexBuffer(shadowIndices, 12);
    
    fsBindVertexArray(shadowVAO);
    fsBindVertexBuffer(shadowVBO);
    fsBindIndexBuffer(shadowEBO);
    fsSetVertexAttribute(0, 3, 3 * sizeof(float), 0);        // Position only for shadow
    
    // Set light properties
    fsSetUniformFloat3(shader, "uLightDir", 1.2f, 1.8f, 1.0f);   // Light from top-right
    fsSetUniformFloat3(shader, "uLightColor", 1.0f, 1.0f, 1.0f);  // White light
    
    printf("========================================\n");
    printf("FSGL Demo: Filled Spinning Cube\n");
    printf("========================================\n");
    printf("- Filled cube with basic lighting\n");
    printf("- Cube centered at origin\n");
    printf("- Camera at corner view\n");
    printf("- Cube spins right to left\n");
    printf("- Shadow projected on ground plane\n");
    printf("- Directional lighting from top-right\n");
    printf("========================================\n");
    printf("Close the window to exit\n\n");
    
    // Main loop
    fsPollEvents(window, renderFrame);
    
    // Cleanup
    fsDeleteShader(shader);
    fsDeleteVertexArray(cubeVAO);
    fsDeleteVertexBuffer(cubeVBO);
    fsDeleteIndexBuffer(cubeEBO);
    fsDeleteVertexArray(shadowVAO);
    fsDeleteVertexBuffer(shadowVBO);
    fsDeleteIndexBuffer(shadowEBO);
    fsDrop();
    
    return 0;
}