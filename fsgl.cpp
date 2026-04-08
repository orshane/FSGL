#include "fsgl.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace fsgl {
    static float currentR = 1.0, currentG = 1.0, currentB = 1.0;


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
        glColor3f(currentR, currentG, currentB);
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

    void fs::fsDrop() {
        glfwTerminate();
        std::cout << "fsDrop: Successful drop.";
    }
}
