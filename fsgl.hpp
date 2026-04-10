#pragma once

#include <string>
#include <cstdbool>
#include <cstdint>

struct GLFWwindow;

namespace fsgl {

    typedef GLFWwindow* fsWindow;
    typedef void (*fsLoopCallback)();
    typedef uint32_t fsShader;
    typedef uint32_t fsVertexBuffer;
    typedef uint32_t fsVertexArray;

    enum fsResult {
        FSOK = 0,
        FSERR = 1
    };

    class fs {
    public:
        // Core and Graphics

        static fsResult fsInit();
        static fsResult fsSetContext(fsWindow win);
        static fsResult fsInitContext();
        static fsWindow fsNewWindow(std::string title, int width, int height);
        static void fsPollEvents(fsWindow win, fsLoopCallback loop);
        static void fsClear();
        static void fsClear(float r, float g, float b);
        static void fsSetColor(float r, float g, float b);
        static void fsDrawLine(float x1, float y1, float x2, float y2);
        static void fsDrawTriangle(
            float x1, float y1,
            float x2, float y2,
            float x3, float y3,
            bool fill = false
        );
        static void fsSetLineWidth(float w);
        static void fsSetAlpha(float a);
        static void fsDrop();

        // Shader API

        static fsShader fsCreateShader(const char* vertexSrc, const char* fragmentSrc);
        static void fsUseShader(fsShader shader);
        static void fsEnableBlend();
        static void fsDisableBlend();
        static void fsSetBlendAdditive();
        static void fsSetBlendAlpha();

        // Modern Functions
        
        static fsVertexBuffer fsCreateVertexBuffer(const float* vertices, size_t size);
        static fsVertexArray fsCreateVertexArray();
        static void fsBindVertexArray(fsVertexArray vao);
        static void fsBindVertexBuffer(fsVertexBuffer vbo);
        static void fsSetVertexAttribute(uint32_t index, int size, int stride, int offset);
        static void fsDrawArrays(int mode, int first, int count);
        static void fsDeleteVertexArray(fsVertexArray vao);
        static void fsDeleteVertexBuffer(fsVertexBuffer vbo);
        static void fsDeleteShader(fsShader shader);
    };
}