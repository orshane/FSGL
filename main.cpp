#include "fsgl.hpp"
#include <cmath>

using namespace fsgl;

// =====================
// SIMPLE 3D STRUCT
// =====================
struct Vec3 {
    float x, y, z;
};

// =====================
// CUBE DATA
// =====================
Vec3 verts[8] = {
    {-0.5f,-0.5f,-0.5f}, { 0.5f,-0.5f,-0.5f},
    { 0.5f, 0.5f,-0.5f}, {-0.5f, 0.5f,-0.5f},
    {-0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f, 0.5f},
    { 0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}
};

int edges[12][2] = {
    {0,1},{1,2},{2,3},{3,0},
    {4,5},{5,6},{6,7},{7,4},
    {0,4},{1,5},{2,6},{3,7}
};

// =====================
// STATE
// =====================
float angle = 0.0f;

// =====================
// MATH
// =====================
Vec3 rotate(Vec3 v, float ay, float ax) {
    float cy = cos(ay), sy = sin(ay);
    float x = v.x * cy - v.z * sy;
    float z = v.x * sy + v.z * cy;

    float cx = cos(ax), sx = sin(ax);
    float y = v.y * cx - z * sx;
    z = v.y * sx + z * cx;

    return {x,y,z};
}

void project(Vec3 v, float &x, float &y) {
    float dist = 3.0f;
    float f = dist / (dist + v.z);

    x = v.x * f;
    y = v.y * f;
}

// =====================
// LOOP
// =====================
void loop() {

    // background
    fsgl::fsClear(0.02f, 0.02f, 0.05f);

    // enable glow blending
    fsgl::fsEnableBlend();
    fsgl::fsSetBlendAlpha();

    // cube color
    fsgl::fsSetColor(0.2f, 0.8f, 1.0f);

    Vec3 t[8];

    // rotate cube
    for(int i = 0; i < 8; i++) {
        t[i] = rotate(verts[i], angle, angle * 0.6f);
    }

    // =====================
    // DRAW GLOW WIREFRAME
    // =====================
    for(int i = 0; i < 12; i++) {

        Vec3 a = t[edges[i][0]];
        Vec3 b = t[edges[i][1]];

        float x1,y1,x2,y2;
        project(a, x1, y1);
        project(b, x2, y2);

        // outer glow
        fsgl::fsSetAlpha(0.15f);
        fsgl::fsSetLineWidth(6.0f);
        fsgl::fsDrawLine(x1,y1,x2,y2);

        // mid glow
        fsgl::fsSetAlpha(0.35f);
        fsgl::fsSetLineWidth(3.0f);
        fsgl::fsDrawLine(x1,y1,x2,y2);

        // core line
        fsgl::fsSetAlpha(1.0f);
        fsgl::fsSetLineWidth(1.5f);
        fsgl::fsDrawLine(x1,y1,x2,y2);
    }

    // =====================
    // optional: spinning triangles (debug feel)
    // =====================
    for(int i = 0; i < 12; i++) {

        Vec3 a = t[edges[i][0]];
        Vec3 b = t[edges[i][1]];
        Vec3 c = t[edges[(i+1)%12][0]];

        float x1,y1,x2,y2,x3,y3;
        project(a,x1,y1);
        project(b,x2,y2);
        project(c,x3,y3);

        fsgl::fsSetAlpha(0.05f);
        fsgl::fsDrawTriangle(x1,y1,x2,y2,x3,y3,true);
    }

    angle += 0.01f;
}

// =====================
// MAIN
// =====================
int main() {

    if(fsgl::fsInit() != FSOK) return -1;

    fsWindow win = fsgl::fsNewWindow("FSGL Test Cube", 800, 600);
    if(!win) return -1;

    fsgl::fsSetContext(win);

    if(fsgl::fsInitContext() != FSOK) return -1;

    fsgl::fsSetLineWidth(2.0f);

    fsgl::fsPollEvents(win, loop);

    fsgl::fsDrop();

    return 0;
}