#include "fsgl.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>
#include <cmath>

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

static GLint getUniformLocation(fsShader shader, const char* name) {
    GLint loc = glGetUniformLocation(shader, name);
    if(loc == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
    return loc;
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
// CLEAR & DEPTH
// =====================
void fsClear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void fsClear(float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void fsClearDepth() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void fsEnableDepthTest() {
    glEnable(GL_DEPTH_TEST);
}

void fsDisableDepthTest() {
    glDisable(GL_DEPTH_TEST);
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

void fsSetUniformInt(fsShader shader, const char* name, int value) {
    glUniform1i(getUniformLocation(shader, name), value);
}

void fsSetUniformFloat(fsShader shader, const char* name, float value) {
    glUniform1f(getUniformLocation(shader, name), value);
}

void fsSetUniformFloat2(fsShader shader, const char* name, float v0, float v1) {
    glUniform2f(getUniformLocation(shader, name), v0, v1);
}

void fsSetUniformFloat3(fsShader shader, const char* name, float v0, float v1, float v2) {
    glUniform3f(getUniformLocation(shader, name), v0, v1, v2);
}

void fsSetUniformFloat4(fsShader shader, const char* name, float v0, float v1, float v2, float v3) {
    glUniform4f(getUniformLocation(shader, name), v0, v1, v2, v3);
}

void fsSetUniformMatrix4(fsShader shader, const char* name, const float* matrix) {
    glUniformMatrix4fv(getUniformLocation(shader, name), 1, GL_FALSE, matrix);
}

// =====================
// VBO / VAO / EBO
// =====================
fsVertexBuffer fsCreateVertexBuffer(const float* v, size_t size) {
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, v, GL_STATIC_DRAW);
    return id;
}

fsIndexBuffer fsCreateIndexBuffer(const unsigned int* indices, size_t count) {
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
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

void fsBindIndexBuffer(fsIndexBuffer ibo) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void fsSetVertexAttribute(uint32_t i, int size, int stride, int offset) {
    glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, stride, (void*)(intptr_t)offset);
    glEnableVertexAttribArray(i);
}

void fsDrawArrays(fsDrawMode mode, int first, int count) {
    GLenum glMode;
    switch(mode) {
        case FS_POINTS: glMode = GL_POINTS; break;
        case FS_LINES: glMode = GL_LINES; break;
        case FS_LINE_LOOP: glMode = GL_LINE_LOOP; break;
        case FS_LINE_STRIP: glMode = GL_LINE_STRIP; break;
        case FS_TRIANGLES: glMode = GL_TRIANGLES; break;
        case FS_TRIANGLE_STRIP: glMode = GL_TRIANGLE_STRIP; break;
        case FS_TRIANGLE_FAN: glMode = GL_TRIANGLE_FAN; break;
        default: glMode = GL_TRIANGLES; break;
    }
    glDrawArrays(glMode, first, count);
}

void fsDrawElements(fsDrawMode mode, int count) {
    GLenum glMode;
    switch(mode) {
        case FS_POINTS: glMode = GL_POINTS; break;
        case FS_LINES: glMode = GL_LINES; break;
        case FS_LINE_LOOP: glMode = GL_LINE_LOOP; break;
        case FS_LINE_STRIP: glMode = GL_LINE_STRIP; break;
        case FS_TRIANGLES: glMode = GL_TRIANGLES; break;
        case FS_TRIANGLE_STRIP: glMode = GL_TRIANGLE_STRIP; break;
        case FS_TRIANGLE_FAN: glMode = GL_TRIANGLE_FAN; break;
        default: glMode = GL_TRIANGLES; break;
    }
    glDrawElements(glMode, count, GL_UNSIGNED_INT, nullptr);
}

void fsDeleteVertexArray(fsVertexArray vao) {
    glDeleteVertexArrays(1, &vao);
}

void fsDeleteVertexBuffer(fsVertexBuffer vbo) {
    glDeleteBuffers(1, &vbo);
}

void fsDeleteIndexBuffer(fsIndexBuffer ibo) {
    glDeleteBuffers(1, &ibo);
}

// =====================
// MATRIX HELPERS
// =====================
void fsMatrixIdentity(float* m) {
    static const float identity[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    memcpy(m, identity, sizeof(float) * 16);
}

void fsMatrixTranslate(float* m, float x, float y, float z) {
    fsMatrixIdentity(m);
    m[12] = x;
    m[13] = y;
    m[14] = z;
}

void fsMatrixScale(float* m, float x, float y, float z) {
    fsMatrixIdentity(m);
    m[0] = x;
    m[5] = y;
    m[10] = z;
}

void fsMatrixRotateX(float* m, float rad) {
    fsMatrixIdentity(m);
    float c = cosf(rad);
    float s = sinf(rad);
    m[5] = c;
    m[6] = s;
    m[9] = -s;
    m[10] = c;
}

void fsMatrixRotateY(float* m, float rad) {
    fsMatrixIdentity(m);
    float c = cosf(rad);
    float s = sinf(rad);
    m[0] = c;
    m[2] = -s;
    m[8] = s;
    m[10] = c;
}

void fsMatrixRotateZ(float* m, float rad) {
    fsMatrixIdentity(m);
    float c = cosf(rad);
    float s = sinf(rad);
    m[0] = c;
    m[1] = s;
    m[4] = -s;
    m[5] = c;
}

void fsMatrixPerspective(float* m, float fovDegrees, float aspect, float near, float far) {
    float fovRad = fsRadians(fovDegrees);
    float tanHalfFov = tanf(fovRad / 2.0f);
    
    fsMatrixIdentity(m);
    m[0] = 1.0f / (aspect * tanHalfFov);
    m[5] = 1.0f / tanHalfFov;
    m[10] = -(far + near) / (far - near);
    m[11] = -1.0f;
    m[14] = -(2.0f * far * near) / (far - near);
    m[15] = 0.0f;
}

void fsMatrixLookAt(float* m, float eyeX, float eyeY, float eyeZ,
                    float targetX, float targetY, float targetZ,
                    float upX, float upY, float upZ) {
    float forwardX = targetX - eyeX;
    float forwardY = targetY - eyeY;
    float forwardZ = targetZ - eyeZ;
    
    // Normalize forward
    float len = sqrtf(forwardX*forwardX + forwardY*forwardY + forwardZ*forwardZ);
    forwardX /= len; forwardY /= len; forwardZ /= len;
    
    // Right = forward x up
    float rightX = forwardY * upZ - forwardZ * upY;
    float rightY = forwardZ * upX - forwardX * upZ;
    float rightZ = forwardX * upY - forwardY * upX;
    
    // Recompute up = right x forward
    float upX2 = rightY * forwardZ - rightZ * forwardY;
    float upY2 = rightZ * forwardX - rightX * forwardZ;
    float upZ2 = rightX * forwardY - rightY * forwardX;
    
    m[0] = rightX;  m[4] = upX2;   m[8]  = -forwardX; m[12] = 0;
    m[1] = rightY;  m[5] = upY2;   m[9]  = -forwardY; m[13] = 0;
    m[2] = rightZ;  m[6] = upZ2;   m[10] = -forwardZ; m[14] = 0;
    m[3] = 0;       m[7] = 0;      m[11] = 0;         m[15] = 1;
    
    // Translate
    float t[16];
    fsMatrixIdentity(t);
    t[12] = -eyeX;
    t[13] = -eyeY;
    t[14] = -eyeZ;
    
    float result[16];
    fsMatrixMultiply(result, m, t);
    memcpy(m, result, sizeof(float) * 16);
}

void fsMatrixMultiply(float* result, const float* a, const float* b) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result[i*4 + j] = 
                a[i*4 + 0] * b[0*4 + j] +
                a[i*4 + 1] * b[1*4 + j] +
                a[i*4 + 2] * b[2*4 + j] +
                a[i*4 + 3] * b[3*4 + j];
        }
    }
}

// =====================
// MATH UTILITIES
// =====================
float fsRadians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

float fsDegrees(float radians) {
    return radians * 180.0f / 3.14159265358979323846f;
}

}