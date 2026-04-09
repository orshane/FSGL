#include "fsgl.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace fsgl {

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

    static float currentR = 1.0, currentG = 1.0, currentB = 1.0, currentA = 1.0f;


    fsResult fs::fsInit() {
        return glfwInit() ? FSOK : FSERR;
    }

    fsResult fs::fsSetContext(fsWindow win) {
        if(!win)
            return FSERR;

        glfwMakeContextCurrent(win);

        if(glfwGetCurrentContext() != win)
            return FSERR;

        return FSOK;
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
        glColor3f(r, g, b);
    }

    void fs::fsDrawLine(float x1, float y1, float x2, float y2) {
        glBegin(GL_LINES);
        glColor4f(currentR, currentG, currentB, currentA);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    void fs::fsDrawTriangle(
            float x1, float y1,
            float x2, float y2,
            float x3, float y3,
            bool fill
    ) {

        if(!fill) {
            fs::fsDrawLine(x1, y1, x2, y2);
            fs::fsDrawLine(x2, y2, x3, y3);
            fs::fsDrawLine(x3, y3, x1, y1);
        } else {
            glBegin(GL_TRIANGLES);
            glColor3f(currentR, currentG, currentB);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
            glVertex2f(x3, y3);
            glEnd();
        }
    }

    void fs::fsSetLineWidth(float w) {
        glLineWidth(w);
    }

    void fs::fsSetAlpha(float a) {
        currentA = a;
    }

    void fs::fsDrop() {
        glfwTerminate();
        std::cout << "fsDrop: Successful drop.";
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
}
