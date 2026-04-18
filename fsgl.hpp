#pragma once

#include <string>
#include <cstdint>

struct GLFWwindow;

namespace fsgl {

// =====================
// TYPES
// =====================
using fsWindow = GLFWwindow*;
using fsShader = unsigned int;
using fsVertexBuffer = unsigned int;
using fsVertexArray = unsigned int;
using fsIndexBuffer = unsigned int;

// =====================
// DRAWING MODES
// =====================
enum fsDrawMode {
    FS_POINTS = 0x0000,
    FS_LINES = 0x0001,
    FS_LINE_LOOP = 0x0002,
    FS_LINE_STRIP = 0x0003,
    FS_TRIANGLES = 0x0004,
    FS_TRIANGLE_STRIP = 0x0005,
    FS_TRIANGLE_FAN = 0x0006
};

enum fsResult {
    FSOK,
    FSERR
};

// loop callback 
using fsLoopCallback = void (*)();

// =====================
// CORE SYSTEM
// =====================
fsResult fsInit();
fsResult fsSetContext(fsWindow win);
fsResult fsInitContext();
fsWindow fsNewWindow(std::string title, int width, int height);
void fsPollEvents(fsWindow win, fsLoopCallback loop);
void fsDrop();

// =====================
// RENDERING (IMMEDIATE MODE)
// =====================
void fsClear();
void fsClear(float r, float g, float b);
void fsClearDepth();
void fsEnableDepthTest();
void fsDisableDepthTest();

void fsSetColor(float r, float g, float b);
void fsSetAlpha(float a);
void fsSetLineWidth(float w);

void fsDrawLine(float x1, float y1, float x2, float y2);
void fsDrawTriangle(float x1, float y1,
                    float x2, float y2,
                    float x3, float y3,
                    bool fill = true);

// =====================
// BLENDING
// =====================
void fsEnableBlend();
void fsDisableBlend();
void fsSetBlendAlpha();
void fsSetBlendAdditive();

// =====================
// SHADERS
// =====================
fsShader fsCreateShader(const char* vertexSrc, const char* fragmentSrc);
void fsUseShader(fsShader shader);
void fsDeleteShader(fsShader shader);

void fsSetUniformInt(fsShader shader, const char* name, int value);
void fsSetUniformFloat(fsShader shader, const char* name, float value);
void fsSetUniformFloat2(fsShader shader, const char* name, float v0, float v1);
void fsSetUniformFloat3(fsShader shader, const char* name, float v0, float v1, float v2);
void fsSetUniformFloat4(fsShader shader, const char* name, float v0, float v1, float v2, float v3);
void fsSetUniformMatrix4(fsShader shader, const char* name, const float* matrix);

// =====================
// VERTEX & INDEX BUFFERS
// =====================
fsVertexBuffer fsCreateVertexBuffer(const float* vertices, size_t size);
fsIndexBuffer fsCreateIndexBuffer(const unsigned int* indices, size_t count);
fsVertexArray fsCreateVertexArray();

void fsBindVertexArray(fsVertexArray vao);
void fsBindVertexBuffer(fsVertexBuffer vbo);
void fsBindIndexBuffer(fsIndexBuffer ibo);

void fsSetVertexAttribute(uint32_t index, int size, int stride, int offset);
void fsDrawArrays(fsDrawMode mode, int first, int count);
void fsDrawElements(fsDrawMode mode, int count);

void fsDeleteVertexArray(fsVertexArray vao);
void fsDeleteVertexBuffer(fsVertexBuffer vbo);
void fsDeleteIndexBuffer(fsIndexBuffer ibo);

// =====================
// 3D MATRIX HELPERS
// =====================
void fsMatrixIdentity(float* mat4);
void fsMatrixTranslate(float* mat4, float x, float y, float z);
void fsMatrixScale(float* mat4, float x, float y, float z);
void fsMatrixRotateX(float* mat4, float radians);
void fsMatrixRotateY(float* mat4, float radians);
void fsMatrixRotateZ(float* mat4, float radians);
void fsMatrixPerspective(float* mat4, float fovDegrees, float aspect, float near, float far);
void fsMatrixLookAt(float* mat4, float eyeX, float eyeY, float eyeZ,
                    float targetX, float targetY, float targetZ,
                    float upX, float upY, float upZ);
void fsMatrixMultiply(float* result, const float* a, const float* b);

// =====================
// MATH UTILITIES
// =====================
float fsRadians(float degrees);
float fsDegrees(float radians);

}