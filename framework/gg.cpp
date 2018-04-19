#include "gg.h"

#include <iostream>
#include <functional>

using namespace std;

#pragma region Application

bool Application::initialize(const std::string &title, int width, int height, int * argc, char ** argv) {
    glutInit(argc, argv);
    glutInitWindowSize(width, height);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow(title.c_str());

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cout << "cannot initialize GLEW" << std::endl;
        return false;
    }

    glClearColor(0.8, 0.8, 0.8, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // left, right, bottom, top, near, far
    // near: 카메라가 보고있는 방향으로 -1.0만큼 떨어짐 = 카메라는 -z방향을 보고 있으므로 near는 1.0으로 잡힌다.
    glOrtho(-1.0, 1.0, -1.0, 1.0, 0.1, 1000.0);

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(mainLoop);

    return true;
}

int Application::run(Scene * scene) {
    Director::getInstance()->setScene(scene);

    glutMainLoop();

    return 0;
}

void Application::mainLoop() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Director::getInstance()->loop();

    glutSwapBuffers();
}

#pragma endregion

#pragma region Director

Director Director::_instance;

Director::~Director() {
    if (_currentScene) {
        _currentScene->released();
        _currentScene->release();
    }
}

Director * Director::getInstance() {
    return &_instance;
}

void Director::setScene(Scene * scene) {
    if (_currentScene) {
        _currentScene->released();
        _currentScene->release();
    }

    _currentScene = scene;
    _currentScene->initialized();
}

void Director::loop() {
    _currentScene->update();
}

#pragma endregion

#pragma region Scene

void Scene::release() {
    delete this;
}

#pragma endregion

#pragma region Vec3

const Vec3 Vec3::ZERO = Vec3(0, 0, 0);

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {
}

Vec3::Vec3() : x(0), y(0), z(0) {
}

Vec3::Vec3(const Vec3 &other) {
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
}

#pragma endregion

#pragma region Color3F

const Color3F Color3F::BLACK      = Color3F(0, 0, 0);
const Color3F Color3F::WHITE      = Color3F(1, 1, 1);
const Color3F Color3F::RED        = Color3F(1, 0, 0);
const Color3F Color3F::GREEN      = Color3F(0, 1, 0);
const Color3F Color3F::BLUE       = Color3F(0, 0, 1);
const Color3F Color3F::YELLOW     = Color3F(1, 1, 0);
const Color3F Color3F::AQUA       = Color3F(0, 1, 1);
const Color3F Color3F::MAGENTA    = Color3F(1, 0, 1);
const Color3F Color3F::ORANGE     = Color3F(1, 0.647, 0);
const Color3F Color3F::GRAY       = Color3F(0.5, 0.5, 0.5);

Color3F::Color3F(float r, float g, float b) : r(r), g(g), b(b) {
}

Color3F::Color3F() : r(0), g(0), b(0) {
}

Color3F::Color3F(const Color3F &other) {
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
}

#pragma endregion

void drawVertices(Vec3 *vertices, int vertexSize, Color3F *colors, int colorSize) {
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, vertexSize);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}