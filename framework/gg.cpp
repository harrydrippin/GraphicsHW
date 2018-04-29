#include "gg.h"

#include <iostream>
#include <functional>
#include <fstream>

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

    // glClearColor(0.8, 0.8, 0.8, 1);
    glClearColor(1, 1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, width, 0, height, -1, 1);
    // glOrtho(-1, 1, -1, 1, -1, 1);

    // glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(Application::display);
    glutIdleFunc(Application::idle);

    return true;
}

int Application::run(Scene * scene) {
    Director::getInstance()->setScene(scene);

    glutMainLoop();

    return 0;
}

void Application::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Director::getInstance()->display();

    glutSwapBuffers();
}

void Application::idle() {
    Director::getInstance()->loop();

    glutPostRedisplay();
}

#pragma endregion

#pragma region Director

Director Director::_instance;

Director::~Director() {
    if (_currentScene) {
        _currentScene->release();
    }
}

Director * Director::getInstance() {
    return &_instance;
}

void Director::setScene(Scene * scene) {
    if (_currentScene) {
        _currentScene->release();
    }

    _currentScene = scene;
    _currentScene->initialized();
}

void Director::display() {
    _currentScene->draw();
}

void Director::loop() {
    _currentScene->update();
}

#pragma endregion

#pragma region Scene

void Scene::release() {
    this->released();
    delete this;
}

#pragma endregion

#pragma region Color

// const Color3F Color3F::BLACK      = Color3F(0, 0, 0);
// const Color3F Color3F::WHITE      = Color3F(1, 1, 1);
// const Color3F Color3F::RED        = Color3F(1, 0, 0);
// const Color3F Color3F::GREEN      = Color3F(0, 1, 0);
// const Color3F Color3F::BLUE       = Color3F(0, 0, 1);
// const Color3F Color3F::YELLOW     = Color3F(1, 1, 0);
// const Color3F Color3F::AQUA       = Color3F(0, 1, 1);
// const Color3F Color3F::MAGENTA    = Color3F(1, 0, 1);
// const Color3F Color3F::ORANGE     = Color3F(1, 0.647, 0);
// const Color3F Color3F::GRAY       = Color3F(0.5, 0.5, 0.5);

// Color3F::Color3F(float r, float g, float b) : r(r), g(g), b(b) {
// }

// Color3F::Color3F() : r(0), g(0), b(0) {
// }

// Color3F::Color3F(const Color3F &other) {
//     this->r = other.r;
//     this->g = other.g;
//     this->b = other.b;
// }

const Color4F Color4F::BLACK      = Color4F(0, 0, 0, 1);
const Color4F Color4F::WHITE      = Color4F(1, 1, 1, 1);
const Color4F Color4F::RED        = Color4F(1, 0, 0, 1);
const Color4F Color4F::GREEN      = Color4F(0, 1, 0, 1);
const Color4F Color4F::BLUE       = Color4F(0, 0, 1, 1);
const Color4F Color4F::YELLOW     = Color4F(1, 1, 0, 1);
const Color4F Color4F::AQUA       = Color4F(0, 1, 1, 1);
const Color4F Color4F::MAGENTA    = Color4F(1, 0, 1, 1);
const Color4F Color4F::ORANGE     = Color4F(1, 0.647, 0, 1);
const Color4F Color4F::GRAY       = Color4F(0.5, 0.5, 0.5, 1);

Color4F::Color4F(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {
}

Color4F::Color4F() : r(0), g(0), b(0), a(0) {
}

Color4F::Color4F(const Color4F &other) {
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
    this->a = other.a;
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

#pragma region Shader

Shader * Shader::create(const std::string &vert, const std::string &frag) {
    auto ret = new Shader();

    ret->initWithFile(vert, frag);
        
    return ret;
}

void Shader::initWithFile(const std::string &vert, const std::string &frag) {
    GLuint vertexShader = createShaderFromFile(vert, GL_VERTEX_SHADER); 
    GLuint fragmentShader = createShaderFromFile(frag, GL_FRAGMENT_SHADER);

    _program = glCreateProgram();

    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);
}

GLuint Shader::getProgram() const {
    return _program;
}

GLuint Shader::createShaderFromFile(const std::string &file, GLuint shaderType) {
    GLuint ret = glCreateShader(shaderType);

    std::ifstream shaderFile(file.c_str());
    std::string shaderString;
    shaderString.assign(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());

    const GLchar *shaderSrc = shaderString.c_str();

    glShaderSource(ret, 1, (const GLchar**)&shaderSrc, NULL);
    glCompileShader(ret);

    return ret;
}

void Shader::addUniform(const std::string &uniform) {
    _uniformLocations[uniform] = glGetUniformLocation(_program, uniform.c_str());
}

void Shader::addAttrib(const std::string &attrib) {
    _attribLocations[attrib] = glGetAttribLocation(_program, attrib.c_str());
}

GLuint Shader::getUniformLocation(const std::string &uniform) const {
    return _uniformLocations.at(uniform);
}

GLuint Shader::getAttribLocation(const std::string &attrib) const {
    return _attribLocations.at(attrib);
}

void Shader::enable() {
    for (auto i : _attribLocations) glEnableVertexAttribArray(i.second);
}

void Shader::disable() {
    for (auto i : _attribLocations) glDisableVertexAttribArray(i.second);
}

void Shader::use() {
    glUseProgram(_program);
}

void Shader::unUse() {
    glUseProgram(0);
}

void Shader::release() {
    _uniformLocations.clear();
    _attribLocations.clear();

    delete this;
}

#pragma endregion

#pragma region Primitives

Primitives::Primitives(float width) : _width(width) {
}

Primitives * Primitives::create(float width) {
    auto ret = new Primitives(width);
    ret->init();

    return ret;
}

bool Primitives::init() {

}

void Primitives::drawPoint(const Vec4 &pos, float size, const Color4F &color) {

}

void Primitives::drawLine(const Vec4 &p1, const Vec4 &p2, const Color4F &color) {
    _lineVertices.push_back(p1);
    _lineVertices.push_back(p2);

    _lineColors.push_back(color);
    _lineColors.push_back(color);
}

void Primitives::drawRectangle(const Vec4 &origin, const Vec4 &dest, const Color4F &color) {
    drawLine(Vec4(origin.x, origin.y, 0, 1), Vec4(dest.x, origin.y, 0, 1), color);
    drawLine(Vec4(dest.x, origin.y, 0, 1), Vec4(dest.x, dest.y, 0, 1), color);
    drawLine(Vec4(dest.x, dest.y, 0, 1), Vec4(origin.x, dest.y, 0, 1), color);
    drawLine(Vec4(origin.x, dest.y, 0, 1), Vec4(origin.x, origin.y, 0, 1), color);
}

void Primitives::drawSolidRectangle(const Vec4 &origin, const Vec4 &dest, const Color4F &color) {
    _polygonVertices.push_back(Vec4(origin.x, origin.y, origin.z, 1));
    _polygonVertices.push_back(Vec4(dest.x, origin.y, origin.z, 1));
    _polygonVertices.push_back(Vec4(origin.x, dest.y, origin.z, 1));

    _polygonVertices.push_back(Vec4(dest.x, origin.y, origin.z, 1));
    _polygonVertices.push_back(Vec4(dest.x, dest.y, origin.z, 1));
    _polygonVertices.push_back(Vec4(origin.x, dest.y, origin.z, 1));

    for (int i = 0; i < 6; i++) _polygonColors.push_back(color);
}

void Primitives::draw() {
    // draw line
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColorPointer(4, GL_FLOAT, 0, _lineColors.data());
    glVertexPointer(4, GL_FLOAT, 0, _lineVertices.data());

    glLineWidth(_width);
    glDrawArrays(GL_LINES, 0, _lineVertices.size());
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // draw polygon
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColorPointer(4, GL_FLOAT, 0, _polygonColors.data());
    glVertexPointer(4, GL_FLOAT, 0, _polygonVertices.data());

    glDrawArrays(GL_TRIANGLES, 0, _polygonVertices.size());
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void Primitives::clear() {
    _lineVertices.clear();
    _lineColors.clear();

    _polygonVertices.clear();
    _polygonColors.clear();
}

void Primitives::release() {
    this->clear();

    delete this;
}

#pragma endregion

void drawVertices(Vec4 *vertices, int vertexSize, Color4F *colors, int colorSize) {
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, colors);
    glVertexPointer(4, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, vertexSize);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}