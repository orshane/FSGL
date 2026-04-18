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

enum fsResult {
    FSOK,
    FSERR
};

// loop callback (IMPORTANT: no lambdas with capture here)
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

// =====================
// VERTEX (FUTURE SAFE)
// =====================
fsVertexBuffer fsCreateVertexBuffer(const float* vertices, size_t size);
fsVertexArray fsCreateVertexArray();

void fsBindVertexArray(fsVertexArray vao);
void fsBindVertexBuffer(fsVertexBuffer vbo);

void fsSetVertexAttribute(uint32_t index, int size, int stride, int offset);
void fsDrawArrays(int mode, int first, int count);

void fsDeleteVertexArray(fsVertexArray vao);
void fsDeleteVertexBuffer(fsVertexBuffer vbo);

}