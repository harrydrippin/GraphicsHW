#ifndef _g_h_
#define _g_h_

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>

#pragma region Application

class Scene;

class Application {
public:
    bool initialize(const std::string &title, int width, int height, int * argc, char ** argv);

    int run(Scene * scene);

private:
    static void mainLoop();

};

#pragma endregion

#pragma region Director

class Scene;

class Director {
public:
    static Director * getInstance();

    // current scene's loop
    void loop();

    void setScene(Scene * scene);

private:
    Director() {}
    ~Director();

private:
    static Director _instance;

    Scene * _currentScene;
    // std::vector<Scene *> _scenes;

};

#pragma endregion

#pragma region Scene

class Scene {
public:
    virtual void initialized() = 0;
    virtual void released() {};
    virtual void update() {};

    void release();

};

#pragma endregion

#pragma region Vec3 

class Vec3 {
public:
    float x, y, z;

public:
    Vec3(float x, float y, float z);
    Vec3();
    Vec3(const Vec3 &other);

public:
    static const Vec3 ZERO;

};

#pragma endregion

#pragma region Color3F

class Color3F {
public:
    float r, g, b;

public:
    Color3F(float r, float g, float b);
    Color3F();
    Color3F(const Color3F &other);

public:
    static const Color3F BLACK, WHITE, RED, GREEN, BLUE, YELLOW, AQUA, MAGENTA, ORANGE, GRAY;

};

#pragma endregion

void drawVertices(Vec3 *vertices, int vertexSize, Color3F *colors, int colorSize);

#endif