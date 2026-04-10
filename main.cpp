#include "fsgl.hpp"
#include <iostream>

const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

fsgl::fsVertexArray g_vao1, g_vao2, g_vao3;

void render() {
    fsgl::fs::fsClear(0.1f, 0.1f, 0.15f);
    
    fsgl::fs::fsBindVertexArray(g_vao1);
    fsgl::fs::fsDrawArrays(0x0004, 0, 3);
    
    fsgl::fs::fsBindVertexArray(g_vao2);
    fsgl::fs::fsDrawArrays(0x0004, 0, 3);
    
    fsgl::fs::fsBindVertexArray(g_vao3);
    fsgl::fs::fsDrawArrays(0x0004, 0, 3);
}

int main() {
    if (fsgl::fs::fsInit() != fsgl::FSOK) return -1;

    fsgl::fsWindow window = fsgl::fs::fsNewWindow("Colored Triangles", 800, 600);
    if (!window) {
        fsgl::fs::fsDrop();
        return -1;
    }

    fsgl::fs::fsSetContext(window);
    fsgl::fs::fsInitContext();

    fsgl::fsShader shader = fsgl::fs::fsCreateShader(vertexShaderSrc, fragmentShaderSrc);
    fsgl::fs::fsUseShader(shader);

    float triangle1[] = {
        -0.7f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.2f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.45f, 0.2f, 0.0f, 0.0f, 1.0f
    };

    float triangle2[] = {
        0.2f, -0.5f, 1.0f, 1.0f, 0.0f,
        0.7f, -0.5f, 1.0f, 0.0f, 1.0f,
        0.45f, 0.2f, 0.0f, 1.0f, 1.0f
    };

    float triangle3[] = {
        -0.3f, 0.3f, 1.0f, 0.5f, 0.0f,
        0.3f, 0.3f, 0.0f, 1.0f, 0.5f,
        0.0f, 0.7f, 0.5f, 0.0f, 1.0f
    };

    fsgl::fsVertexBuffer vbo1 = fsgl::fs::fsCreateVertexBuffer(triangle1, sizeof(triangle1));
    fsgl::fsVertexBuffer vbo2 = fsgl::fs::fsCreateVertexBuffer(triangle2, sizeof(triangle2));
    fsgl::fsVertexBuffer vbo3 = fsgl::fs::fsCreateVertexBuffer(triangle3, sizeof(triangle3));

    g_vao1 = fsgl::fs::fsCreateVertexArray();
    g_vao2 = fsgl::fs::fsCreateVertexArray();
    g_vao3 = fsgl::fs::fsCreateVertexArray();

    fsgl::fs::fsBindVertexArray(g_vao1);
    fsgl::fs::fsBindVertexBuffer(vbo1);
    fsgl::fs::fsSetVertexAttribute(0, 2, 5 * sizeof(float), 0);
    fsgl::fs::fsSetVertexAttribute(1, 3, 5 * sizeof(float), 2 * sizeof(float));

    fsgl::fs::fsBindVertexArray(g_vao2);
    fsgl::fs::fsBindVertexBuffer(vbo2);
    fsgl::fs::fsSetVertexAttribute(0, 2, 5 * sizeof(float), 0);
    fsgl::fs::fsSetVertexAttribute(1, 3, 5 * sizeof(float), 2 * sizeof(float));

    fsgl::fs::fsBindVertexArray(g_vao3);
    fsgl::fs::fsBindVertexBuffer(vbo3);
    fsgl::fs::fsSetVertexAttribute(0, 2, 5 * sizeof(float), 0);
    fsgl::fs::fsSetVertexAttribute(1, 3, 5 * sizeof(float), 2 * sizeof(float));

    fsgl::fs::fsPollEvents(window, render);

    fsgl::fs::fsDeleteVertexArray(g_vao1);
    fsgl::fs::fsDeleteVertexArray(g_vao2);
    fsgl::fs::fsDeleteVertexArray(g_vao3);
    fsgl::fs::fsDeleteVertexBuffer(vbo1);
    fsgl::fs::fsDeleteVertexBuffer(vbo2);
    fsgl::fs::fsDeleteVertexBuffer(vbo3);
    fsgl::fs::fsDeleteShader(shader);
    fsgl::fs::fsDrop();

    return 0;
}