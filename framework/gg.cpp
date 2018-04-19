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

#pragma region Color

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

#pragma region Transform

Mat4 translate(float dx, float dy, float dz) {
    Mat4 ret;
    ret.setIdentity();

    ret(0, 3) = dx;
    ret(1, 3) = dy;
    ret(2, 3) = dz;

    return ret;
}

Mat4 rotate(float angle, float x, float y, float z) {
    Mat4 ret;

    Vec3 u(x, y, z);
    u.normalize();

    x = u.x;
    y = u.y;
    z = u.z;

    float radian = angle * M_PI / 180.0f;

    float cosine = std::cos(radian), sine = std::sin(radian);
    float ccx = (1 - cosine) * x, ccy = (1 - cosine) * y, ccz = (1 - cosine) * z;
    float sx = sine * x, sy = sine * y, sz = sine * z;

    ret(0, 0) = x * ccx + cosine;
    ret(0, 1) = y * ccx - sz;
    ret(0, 2) = z * ccx + sy;

    ret(1, 0) = x * ccy + sz;
    ret(1, 1) = y * ccy + cosine;
    ret(1, 2) = z * ccy - sx;

    ret(2, 0) = x * ccz - sy;
    ret(2, 1) = y * ccz + sx;
    ret(2, 2) = z * ccz + cosine;

    ret(3, 3) = 1;

    return ret;
}

Mat4 scale(float sx, float sy, float sz) {
    Mat4 ret;

    ret(0, 0) = sx;
    ret(1, 1) = sy;
    ret(2, 2) = sz;
    ret(3, 3) = 1;

    return ret;
}
        
Mat4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
    Mat4 A;

    Vec3 camXAxis, camYAxis(upX, upY, upZ), camZAxis(eyeX - centerX, eyeY - centerY, eyeZ - centerZ);
    camZAxis.normalize();
    camXAxis = camYAxis.cross(camZAxis).getNormalize();
    camYAxis = camZAxis.cross(camXAxis).getNormalize();

    for (int i = 0; i < 3; i++) {
        A(0, i) = camXAxis(i);
        A(1, i) = camYAxis(i);
        A(2, i) = camZAxis(i);
    }
    A(3, 3) = 1;

    return A * translate(-eyeX, -eyeY, -eyeZ);
}

Mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
    Mat4 ret;

    ret(0, 0) = 2.0f / (right - left);
    ret(1, 1) = 2.0f / (top - bottom);
    ret(2, 2) = -2.0f / (far - near);

    ret(0, 3) = -(right + left) / (right - left);
    ret(1, 3) = -(top + bottom) / (top - bottom);
    ret(2, 3) = -(far + near) / (far - near);

    ret(3, 3) = 1;

    return ret;
}

Mat4 frustum(float left, float right, float bottom, float top, float near, float far) {
    Mat4 ret;

    float near2 = near * 2.0f;
    
    ret(0, 0) = near2 / (right - left);
    ret(1, 1) = near2 / (top - bottom);
    ret(2, 2) = -(far + near) / (far - near);

    ret(0, 2) = (right + left) / (right - left);
    ret(1, 2) = (top + bottom) / (top - bottom);
    ret(2, 3) = -near2 * far / (far - near);
    ret(3, 2) = -1.0f;

    return ret;
}

Mat4 perspective(float fovy, float aspect, float zNear, float zFar) {
    float width, height;

    // rfovy = (fovy * M_PI / 180.0) / 2.0
    float rfovy = fovy * M_PI / 360.0;

    height = zNear * std::tanf(rfovy);
    width = height * aspect;

    return frustum(-width, width, -height, height, zNear, zFar);
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