#ifndef _HELP_H_
#define _HELP_H_

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <fstream>

namespace help {

#pragma region Vector

template<unsigned int N>
class Vector {
public:
    Vector() {
        setToZero();
    }

    Vector(const float elem) {
        std::fill(_value, _value + N, elem);
    }

    Vector(const float s, const float t) : Vector(0) { 
        _value[0] = s;
        _value[1] = t;
    }

    Vector(const float s, const float t, const float u) : Vector(s, t) {
        _value[2] = u;
    }

    Vector(const float s, const float t, const float u, const float v) : Vector(s, t, u) {
        _value[3] = v;
    }

    Vector(const Vector<3> &other, float v) {
        for (int i = 0; i < 3; i++) (*this)(i) = other(i);
        _value[3] = v;
    }

    Vector(const Vector<4> &other) {
        for (int i = 0; i < N; i++) (*this)(i) = other(i);
    }
    
    Vector &operator=(const Vector<N> &other) {
        for (int i = 0; i < N; i++) (*this)(i) = other(i);
        return *this;
    }

    float &operator()(unsigned int i) {
        return _value[i];
    }

    const float &operator()(unsigned int i) const {
        return _value[i];
    }

    operator const float *() const {
        return _value;
    }

    operator float *() {
        return _value;
    }

    Vector &operator+=(const Vector<N> &other) {
        for (int i = 0; i < N; i++) (*this)(i) += other(i);
        return *this;
    }

    Vector &operator-=(const Vector<N> &other) {
        for (int i = 0; i < N; i++) (*this)(i) -= other(i);
        return *this;
    }

    Vector operator*(float s) {
        Vector ret = *this;
        for (int i = 0; i < N; i++) ret(i) *= s;
        return ret;
    }

    void setToZero() {
        std::fill(_value, _value + N, 0);
    }

    float dot(const Vector<N> &other) {
        float ret = 0;

        for (unsigned int i = 0; i < N; i++) ret += (*this)(i) * other(i);

        return ret;
    }

    Vector<3> cross(const Vector<3> &other) {
        Vector<3> ret;

        ret.x = this->y * other.z - this->z * other.y;
        ret.y = this->z * other.x - this->x * other.z;
        ret.z = this->x * other.y - this->y * other.x;

        return ret;
    }

    Vector<3> getNormalize() const {
        Vector<3> ret;

        float n = x * x + y * y + z * z;
        if (n == 1) return *this;

        n = 1 / std::sqrt(n);
        ret.x = x * n;
        ret.y = y * n;
        ret.z = z * n;

        return ret;
    }

    void normalize() {
        float n = x * x + y * y + z * z;
        if (n == 1) return;

        n = 1 / std::sqrt(n);
        x = x * n;
        y = y * n;
        z = z * n;
    }

public:
    union {
        struct {
            float x, y, z;
        };
        float _value[N];
    };

};

typedef Vector<2> Vec2;
typedef Vector<3> Vec3;
typedef Vector<4> Vec4;

#pragma endregion

#pragma region Matrix

template<unsigned int M, unsigned int N>
class Matrix {
public:
    Matrix() {
        setToZero();
    }

    Matrix(const float elem) {
        std::fill(_value, _value + M * N, elem);
    }

    float &operator()(unsigned int r, unsigned int c) {
        return _value[r + c * M];
    }

    const float &operator()(unsigned int r, unsigned int c) const {
        return _value[r + c * M];
    }

    operator const float *() const {
        return _value;
    }

    operator float *() {
        return _value;
    }

    void setToZero() {
        std::fill(_value, _value + M * N, 0);
    }

    Vector<M> getColumn(unsigned int i) const {
        Vector<M> ret;
        for (int r = 0; r < M; r++) {
            ret(r) = (*this)(r, i);
        }
        return ret;
    }

    Vector<N> getRow(unsigned int i) const {
        Vector<N> ret;
        for (int c = 0; c < N; c++) {
            ret(c) = (*this)(i, c);
        }
        return ret;
    }

    void setColumn(unsigned int i, const Vector<M> &col) {
        for (int r = 0; r < M; r++) {
            (*this)(r, i) = col(r);
        }
    }

    void setRow(unsigned int i, const Vector<N> &row) {
        for (int c = 0; c < N; c++) {
            (*this)(i, c) = row(c);
        }
    }

    Matrix<N, M> transpose() const {
        Matrix<N, M> ret;

        for (int r = 0; r < N; r++) {
            for (int c = 0; c < M; c++) {
                ret(r, c) = (*this)(c, r);
            }
        }
        
        return  ret;
    }

    void setIdentity() {
        if (M != N) return;

        setToZero();
        for (int i = 0; i < M; i++) {
            _value[i + i * M] = 1;
        }
    }

    Matrix<4, 4> getInverse() {
        Matrix<4, 4> m = (*this);
        float m00 = m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3);
        float m01 = m(2, 1) * m(3, 3) - m(3, 1) * m(2, 3);
        float m02 = m(2, 1) * m(3, 2) - m(3, 1) * m(2, 2);
        float m03 = m(2, 0) * m(3, 3) - m(3, 0) * m(2, 3);
        float m04 = m(2, 0) * m(3, 2) - m(3, 0) * m(2, 2);
        float m05 = m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1);
        float m06 = m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3);
        float m07 = m(1, 1) * m(3, 3) - m(3, 1) * m(1, 3);
        float m08 = m(1, 1) * m(3, 2) - m(3, 1) * m(1, 2);
        float m09 = m(1, 0) * m(3, 3) - m(3, 0) * m(1, 3);
        float m10 = m(1, 0) * m(3, 2) - m(3, 0) * m(1, 2);
        float m11 = m(1, 1) * m(3, 3) - m(3, 1) * m(1, 3);
        float m12 = m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1);
        float m13 = m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3);
        float m14 = m(1, 1) * m(2, 3) - m(2, 1) * m(1, 3);
        float m15 = m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2);
        float m16 = m(1, 0) * m(2, 3) - m(2, 0) * m(1, 3);
        float m17 = m(1, 0) * m(2, 2) - m(2, 0) * m(1, 2);
        float m18 = m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1);

        Matrix<4, 4> inverseMat;
        inverseMat(0, 0) = + (m(1, 1) * m00 - m(1, 2) * m01 + m(1, 3) * m02);
        inverseMat(0, 1) = - (m(1, 0) * m00 - m(1, 2) * m03 + m(1, 3) * m04);
        inverseMat(0, 2) = + (m(1, 0) * m01 - m(1, 1) * m03 + m(1, 3) * m05);
        inverseMat(0, 3) = - (m(1, 0) * m02 - m(1, 1) * m04 + m(1, 2) * m05);
  
        inverseMat(1, 0) = - (m(0, 1) * m00 - m(0, 2) * m01 + m(0, 3) * m02);
        inverseMat(1, 1) = + (m(0, 0) * m00 - m(0, 2) * m03 + m(0, 3) * m04);
        inverseMat(1, 2) = - (m(0, 0) * m01 - m(0, 1) * m03 + m(0, 3) * m05);
        inverseMat(1, 3) = + (m(0, 0) * m02 - m(0, 1) * m04 + m(0, 2) * m05);
  
        inverseMat(2, 0) = + (m(0, 1) * m06 - m(0, 2) * m07 + m(0, 3) * m08);
        inverseMat(2, 1) = - (m(0, 0) * m06 - m(0, 2) * m09 + m(0, 3) * m10);
        inverseMat(2, 2) = + (m(0, 0) * m11 - m(0, 1) * m09 + m(0, 3) * m12);
        inverseMat(2, 3) = - (m(0, 0) * m08 - m(0, 1) * m10 + m(0, 2) * m12);
  
        inverseMat(3, 0) = - (m(0, 1) * m13 - m(0, 2) * m14 + m(0, 3) * m15);
        inverseMat(3, 1) = + (m(0, 0) * m13 - m(0, 2) * m16 + m(0, 3) * m17);
        inverseMat(3, 2) = - (m(0, 0) * m14 - m(0, 1) * m16 + m(0, 3) * m18);
        inverseMat(3, 3) = + (m(0, 0) * m15 - m(0, 1) * m17 + m(0, 2) * m18);

        float det = m(0, 0) * inverseMat(0, 0) + 
                    m(0, 1) * inverseMat(0, 1) + 
                    m(0, 2) * inverseMat(0, 2) + 
                    m(0, 3) * inverseMat(0, 3);

        for (unsigned int i = 0; i < 4; i++) {
            for (unsigned int j = 0; j < 4; j++) {
                inverseMat(i, j) = inverseMat(i, j) / det;
            }
        }

        inverseMat = inverseMat.transpose();

        return inverseMat;
    }

protected:
    float _value[M * N];

};

typedef Matrix<3, 3> Mat3;
typedef Matrix<4, 4> Mat4;

#pragma endregion

#pragma region Color4F

class Color4F {
public:
    Color4F() {
        setToZero();
    }

    Color4F(const float r, const float g, const float b) {
        _value[0] = r;
        _value[1] = g;
        _value[2] = b;
        _value[3] = 1;
     }

    Color4F(const float r, const float g, const float b, const float a) : Color4F(r, g, b) {
        _value[3] = a;
     }
    
    Color4F(const Color4F &other) {
        for (int i = 0; i < 4; i++) (*this)(i) = other(i);
    }
    
    Color4F &operator=(const Color4F &other) {
        for (int i = 0; i < 4; i++) (*this)(i) = other(i);
        return *this;
    }

    float &operator()(unsigned int i) {
        return _value[i];
    }

    const float &operator()(unsigned int i) const {
        return _value[i];
    }

    operator const float *() const {
        return _value;
    }

    operator float *() {
        return _value;
    }

    Color4F &operator+=(const Color4F &other) {
        for (int i = 0; i < 4; i++) (*this)(i) += other(i);
        return *this;
    }

    Color4F &operator-=(const Color4F &other) {
        for (int i = 0; i < 4; i++) (*this)(i) -= other(i);
        return *this;
    }

    void setToZero() {
        std::fill(_value, _value + 4, 0);
    }

public:
    union {
        struct {
            float r, g, b, a;
        };
        float _value[4];
    };

    static const Color4F BLACK, WHITE, RED, GREEN, BLUE, YELLOW, AQUA, MAGENTA, ORANGE, GRAY;

};

#pragma endregion

#pragma region Operator

template<unsigned int N>
Vector<N> operator+(const Vector<N> &u, const Vector<N> &v) {
    Vector<N> ret;

    for (unsigned int i = 0; i < N; i++) {
        ret(i) = u(i) + v(i);
    }

    return ret;
}

template<unsigned int N>
Vector<N> operator-(const Vector<N> &u, const Vector<N> &v) {
    Vector<N> ret;

    for (unsigned int i = 0; i < N; i++) {
        ret(i) = u(i) - v(i);
    }

    return ret;
}

template<unsigned int N>
Vector<N> operator*(const float s, const Vector<N> &v) {
    Vector<N> ret;

    for (unsigned int i = 0; i < N; i++) {
        ret(i) = s * v(i);
    }

    return ret;
}

template<unsigned int M, unsigned int N>
Vector<M> operator*(const Matrix<M, N> &A, const Vector<N> &v) {
    Vector<M> ret, col;

    for (unsigned int i = 0; i < N; i++) {
        col = A.getColumn(i);
        ret += v(i) * col;
    }

    return ret;
}

template<unsigned int M, unsigned int N>
Vector<N> operator*(const Vector<M> &v, const Matrix<M, N> &A) {
    Vector<N> ret;
    Vector<M> col;

    for (unsigned int i = 0; i < N; i++) {
        col = A.getColumn(i);
        ret(i) = v.dot(col);
    }

    return ret;
}

template<unsigned int M, unsigned int N, unsigned int L>
Matrix<M, L> operator*(const Matrix<M, N> &A, const Matrix<N, L> &B) {
    Matrix<M, L> ret;
    Vector<N> row, col;

    for (unsigned int i = 0; i < M; i++) {
        row = A.getRow(i);

        for (unsigned int j = 0; j < L; j++) {
            col = B.getColumn(j);
            ret(i, j) = row.dot(col);
        }
    }

    return ret;
}

#pragma endregion

#pragma region Application

class Scene;

class Application {
public:
    Application(const std::string &title, int width, int height);

    bool initialize(int argc, char ** argv);
    int run(Scene * scene);
    
    static void displayFunction();
    static void idleFunction();
    static void keyboardFunction(unsigned char keycode, int x, int y);
    static void specialFunction(int keycode, int x, int y);
    static void mouseMoveFunction(int x, int y);

protected:
    std::string _title;
    int _width, _height;

};

#pragma endregion

#pragma region Object

class Object {
public:
    virtual void release();

    void addChild(Object * child);

    virtual void onDraw() {}

protected:
    std::vector<Object *> _children;

};

#pragma endregion

#pragma region Scene

class Scene : public Object {
public:
    void init();

    virtual void release();

    virtual void onDraw();

public:
    virtual void start() {}
    virtual void update() {}
    virtual void end() {}
    virtual void draw() {}

    virtual void onKeyboardPress(unsigned char keycode, int x, int y) {}
    virtual void onSpecialKeyboardPress(int keycode, int x, int y) {}
    virtual void onMouseMove(int x, int y) {}
    
};

#pragma endregion

#pragma region Node

class Node : public Object {
public:
    Mat4 _modelMatrix;
    // Vec3 _position, _rotation, _scale;
    Mat4 _trMat, _rotMat, _sclMat;

public:
    Node();

    void setPosition(const Vec3 &pos);
    void setRotation(float angle, const Vec3 &rot);
    void setScale(const Vec3 &s);

    void transform();

};

#pragma endregion

#pragma region SceneManager

class SceneManager {
public:
    static SceneManager * get();

    void setScene(Scene * scene);
    Scene * getScene();

private:
    SceneManager() {}
    ~SceneManager();

    static SceneManager _instance;

    Scene * _currentScene;

};

#pragma endregion

#pragma region Camera

class Camera {
public:
    static Camera * get();

    Mat4 getView(); 
    Mat4 getProjection();

    void moveVertical(float delta);
    void moveHorizontal(float delta);

    void rotate(float delta);

    void setFovy(float fovy);
    float getFovy() const;

private:
    Camera() {}
    ~Camera() {}

    static Camera _instance;

    Vec3 _position          = Vec3(0, 2, 0), 
         _frontDirection    = Vec3(0, 0, -1), 
         _rightDirection    = Vec3(1, 0, 0),
         _upDirection       = Vec3(0, 1, 0);

    float _fovy = 60;

};

#pragma endregion

#pragma region Shader

class Shader {
public:
    static Shader * create(const std::string &vert, const std::string &frag);
    static Shader * createWithString(const std::string &vert, const std::string &frag);

    void addUniform(const std::string &uniform);
    void addAttrib(const std::string &attrib);

    GLuint getUniformLocation(const std::string &uniform) const;
    GLuint getAttribLocation(const std::string &attrib) const;

    void enable();
    void disable();

    void use();
    void unUse();

    void printError(const std::string& op);

    GLuint getProgram() const;

    void release();

protected:
    virtual void initWithFile(const std::string &vert, const std::string &frag);
    virtual void initWithString(const std::string &vert, const std::string &frag);

    GLuint createShaderFromFile(const std::string &file, GLuint shaderType);
    GLuint createShaderFromString(const std::string &str, GLuint shaderType);

protected:
    GLuint _program;

    std::map<std::string, GLuint> _uniformLocations, _attribLocations;
    // std::map<std::string, 

};

#pragma endregion

#pragma region Primitive2D

class Primitive2D : public Node {
public:
    Primitive2D(float width);

    static Primitive2D * create(float width = 1.0f);

    void drawPoint(const Vec3 &pos, float size, const Color4F &color);
    void drawLine(const Vec3 &p1, const Vec3 &p2, const Color4F &color);
    void drawRectangle(const Vec3 &origin, const Vec3 &dest, const Color4F &color);

    void drawSolidRectangle(const Vec3 &origin, const Vec3 &dest, const Color4F &color);

    void setShader(Shader * shader);

    virtual void onDraw();

    void clear();

    virtual void release();

protected:
    bool init();

protected:
    float _lineWidth;

    std::vector<Vec3> _pointVertices, _lineVertices, _polygonVertices;
    std::vector<Color4F> _pointColors, _lineColors, _polygonColors;

    Shader * _shader;

};

#pragma endregion

#pragma region Obj3D

class Obj3D : public Node {
public:
    static Obj3D * create(const std::string &file);

    virtual void release();

    void onDraw();

public:
    bool init(const std::string &file);

    std::vector<Vec3> _vertices;
    std::vector<Vec3> _normals;
};

#pragma endregion

#pragma region Transform

Mat4 translate(float dx, float dy, float dz);
Mat4 rotate(float angle, float x, float y, float z);
Mat4 scale(float sx, float sy, float sz) ;   
Mat4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);
Mat4 ortho(float left, float right, float bottom, float top, float near, float far);
Mat4 frustum(float left, float right, float bottom, float top, float near, float far);
Mat4 perspective(float fovy, float aspect, float zNear, float zFar);

#pragma endregion

#pragma region Texture

class Texture {
public:
    static Texture * create();

    void loadTexture(const std::string &file);

    void bindTextures();

    virtual void release();

public:
    std::vector<GLuint> _texid;
    std::vector<int> _width, _height;

};

#pragma endregion

};

#endif