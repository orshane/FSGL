#include "fsgl.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace fsgl {

    static float currentR = 1.0f, currentG = 1.0f, currentB = 1.0f, currentA = 1.0f;
    static fsShader currentShader = 0;

    static GLuint compileShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if(!success) {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::cerr << "Shader compile error: \n" << log << std::endl;
        }

        return shader;
    }

    static GLint getPositionAttribLocation() {
        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        
        if(currentProgram) {
            GLint loc = glGetAttribLocation(currentProgram, "aPos");
            if(loc == -1) loc = glGetAttribLocation(currentProgram, "vertexPosition");
            if(loc == -1) loc = glGetAttribLocation(currentProgram, "position");
            if(loc == -1) loc = glGetAttribLocation(currentProgram, "Position");
            if(loc == -1) loc = glGetAttribLocation(currentProgram, "inPosition");
            if(loc == -1) loc = 0;
            return loc;
        }
        
        return 0;
    }

    fsResult fs::fsInit() {
        return glfwInit() ? FSOK : FSERR;
    }

    fsResult fs::fsSetContext(fsWindow win) {
        if(!win) return FSERR;
        glfwMakeContextCurrent(win);
        return (glfwGetCurrentContext() == win) ? FSOK : FSERR;
    }

    fsResult fs::fsInitContext() {
        GLenum err = glewInit();
        return (err == GLEW_OK) ? FSOK : FSERR;
    }

    fsWindow fs::fsNewWindow(std::string title, int width, int height) {
        return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }

    void fs::fsPollEvents(fsWindow win, fsLoopCallback loop) {
        while(!glfwWindowShouldClose(win)) {
            loop();
            glfwSwapBuffers(win);
            glfwPollEvents();
        }
    }

    void fs::fsClear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void fs::fsClear(float r, float g, float b) {
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void fs::fsSetColor(float r, float g, float b) {
        currentR = r;
        currentG = g;
        currentB = b;
        if(!currentShader) {
            glColor3f(r, g, b);
        }
    }

    void fs::fsDrawLine(float x1, float y1, float x2, float y2) {
        GLint posLoc = getPositionAttribLocation();
        
        glBegin(GL_LINES);
        if(currentShader) {
            glVertexAttrib2f(posLoc, x1, y1);
            glVertexAttrib2f(posLoc, x2, y2);
        } else {
            glColor4f(currentR, currentG, currentB, currentA);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
        }
        glEnd();
    }

    void fs::fsDrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, bool fill) {
        GLint posLoc = getPositionAttribLocation();
        
        if(!fill) {
            glBegin(GL_LINES);
            if(currentShader) {
                glVertexAttrib2f(posLoc, x1, y1);
                glVertexAttrib2f(posLoc, x2, y2);
                glVertexAttrib2f(posLoc, x2, y2);
                glVertexAttrib2f(posLoc, x3, y3);
                glVertexAttrib2f(posLoc, x3, y3);
                glVertexAttrib2f(posLoc, x1, y1);
            } else {
                glColor4f(currentR, currentG, currentB, currentA);
                glVertex2f(x1, y1);
                glVertex2f(x2, y2);
                glVertex2f(x2, y2);
                glVertex2f(x3, y3);
                glVertex2f(x3, y3);
                glVertex2f(x1, y1);
            }
            glEnd();
        } else {
            glBegin(GL_TRIANGLES);
            if(currentShader) {
                glVertexAttrib2f(posLoc, x1, y1);
                glVertexAttrib2f(posLoc, x2, y2);
                glVertexAttrib2f(posLoc, x3, y3);
            } else {
                glColor4f(currentR, currentG, currentB, currentA);
                glVertex2f(x1, y1);
                glVertex2f(x2, y2);
                glVertex2f(x3, y3);
            }
            glEnd();
        }
    }

    void fs::fsSetLineWidth(float w) {
        glLineWidth(w);
    }

    void fs::fsSetAlpha(float a) {
        currentA = a;
        if(!currentShader) {
            glColor4f(currentR, currentG, currentB, currentA);
        }
    }

    void fs::fsDrop() {
        glfwTerminate();
        std::cout << "fsDrop: Successful drop." << std::endl;
    }

    fsShader fs::fsCreateShader(const char* vertexSrc, const char* fragmentSrc) {
        GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if(!success) {
            char log[512];
            glGetProgramInfoLog(program, 512, nullptr, log);
            std::cerr << "Shader link error: \n" << log << std::endl;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

    void fs::fsUseShader(fsShader shader) {
        currentShader = shader;
        glUseProgram(shader);
    }

    void fs::fsEnableBlend() {
        glEnable(GL_BLEND);
    }

    void fs::fsDisableBlend() {
        glDisable(GL_BLEND);
    }

    void fs::fsSetBlendAdditive() {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    void fs::fsSetBlendAlpha() {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    fsVertexBuffer fs::fsCreateVertexBuffer(const float* vertices, size_t size) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return vbo;
    }

    fsVertexArray fs::fsCreateVertexArray() {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        return vao;
    }

    void fs::fsBindVertexArray(fsVertexArray vao) {
        glBindVertexArray(vao);
    }

    void fs::fsBindVertexBuffer(fsVertexBuffer vbo) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    }

    void fs::fsSetVertexAttribute(uint32_t index, int size, int stride, int offset) {
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (void*)(intptr_t)offset);
        glEnableVertexAttribArray(index);
    }

    void fs::fsDrawArrays(int mode, int first, int count) {
        glDrawArrays(mode, first, count);
    }

    void fs::fsDeleteVertexArray(fsVertexArray vao) {
        glDeleteVertexArrays(1, &vao);
    }

    void fs::fsDeleteVertexBuffer(fsVertexBuffer vbo) {
        glDeleteBuffers(1, &vbo);
    }

    void fs::fsDeleteShader(fsShader shader) {
        glDeleteProgram(shader);
        if(currentShader == shader) {
            currentShader = 0;
        }
    }
}