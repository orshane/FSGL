#include "fsgl.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace fsgl {

// =====================
// INTERNAL STATE
// =====================
static float currentR = 1.0f;
static float currentG = 1.0f;
static float currentB = 1.0f;
static float currentA = 1.0f;

static fsShader currentShader = 0;

// =====================
// SHADER HELPERS
// =====================
static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error:\n" << log << std::endl;
    }

    return shader;
}

// =====================
// INIT
// =====================
fsResult fsInit() {
    return glfwInit() ? FSOK : FSERR;
}

fsResult fsSetContext(fsWindow win) {
    if(!win) return FSERR;
    glfwMakeContextCurrent(win);
    return (glfwGetCurrentContext() == win) ? FSOK : FSERR;
}

fsResult fsInitContext() {
    return (glewInit() == GLEW_OK) ? FSOK : FSERR;
}

fsWindow fsNewWindow(std::string title, int width, int height) {
    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

void fsPollEvents(fsWindow win, fsLoopCallback loop) {
    while(!glfwWindowShouldClose(win)) {
        loop();
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
}

void fsDrop() {
    glfwTerminate();
}

// =====================
// CLEAR
// =====================
void fsClear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void fsClear(float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// =====================
// COLOR STATE
// =====================
void fsSetColor(float r, float g, float b) {
    currentR = r;
    currentG = g;
    currentB = b;

    if(!currentShader)
        glColor3f(r, g, b);
}

void fsSetAlpha(float a) {
    currentA = a;

    if(!currentShader)
        glColor4f(currentR, currentG, currentB, currentA);
}

void fsSetLineWidth(float w) {
    glLineWidth(w);
}

// =====================
// DRAWING
// =====================
void fsDrawLine(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);

    if(currentShader) {
        glVertexAttrib2f(0, x1, y1);
        glVertexAttrib2f(0, x2, y2);
    } else {
        glColor4f(currentR, currentG, currentB, currentA);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    }

    glEnd();
}

void fsDrawTriangle(float x1,float y1,
                    float x2,float y2,
                    float x3,float y3,
                    bool fill) {

    if(fill) {
        glBegin(GL_TRIANGLES);
        glColor4f(currentR,currentG,currentB,currentA);

        glVertex2f(x1,y1);
        glVertex2f(x2,y2);
        glVertex2f(x3,y3);

        glEnd();
    } else {
        glBegin(GL_LINES);
        glColor4f(currentR,currentG,currentB,currentA);

        glVertex2f(x1,y1); glVertex2f(x2,y2);
        glVertex2f(x2,y2); glVertex2f(x3,y3);
        glVertex2f(x3,y3); glVertex2f(x1,y1);

        glEnd();
    }
}

// =====================
// BLENDING
// =====================
void fsEnableBlend() { glEnable(GL_BLEND); }
void fsDisableBlend() { glDisable(GL_BLEND); }

void fsSetBlendAlpha() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void fsSetBlendAdditive() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

// =====================
// SHADERS
// =====================
fsShader fsCreateShader(const char* vs, const char* fsSrc) {
    GLuint v = compileShader(GL_VERTEX_SHADER, vs);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, fsSrc);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);

    glDeleteShader(v);
    glDeleteShader(f);

    return prog;
}

void fsUseShader(fsShader shader) {
    currentShader = shader;
    glUseProgram(shader);
}

void fsDeleteShader(fsShader shader) {
    glDeleteProgram(shader);
    if(currentShader == shader)
        currentShader = 0;
}

// =====================
// VBO / VAO (BASIC)
// =====================
fsVertexBuffer fsCreateVertexBuffer(const float* v, size_t size) {
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, v, GL_STATIC_DRAW);
    return id;
}

fsVertexArray fsCreateVertexArray() {
    GLuint id;
    glGenVertexArrays(1, &id);
    return id;
}

void fsBindVertexArray(fsVertexArray vao) {
    glBindVertexArray(vao);
}

void fsBindVertexBuffer(fsVertexBuffer vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void fsSetVertexAttribute(uint32_t i, int size, int stride, int offset) {
    glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, stride, (void*)(intptr_t)offset);
    glEnableVertexAttribArray(i);
}

void fsDrawArrays(int mode, int first, int count) {
    glDrawArrays(mode, first, count);
}

void fsDeleteVertexArray(fsVertexArray vao) {
    glDeleteVertexArrays(1, &vao);
}

void fsDeleteVertexBuffer(fsVertexBuffer vbo) {
    glDeleteBuffers(1, &vbo);
}

}