#pragma once 
#include <string>
#include <cstdbool>

struct GLFWwindow;


namespace fsgl {

    typedef GLFWwindow* fsWindow;

    typedef void (*fsLoopCallback)();

    enum fsResult {
        FSOK = 0,
        FSERR = 1
    };

    class fs {
    public:
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
        static void fsDrop();
    };
}
