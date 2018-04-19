#ifndef _g_h_
#define _g_h_

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cmath>

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

    Vector(const float s, const float t) { 
        _value[0] = s;
        _value[1] = t;
    }

    Vector(const float s, const float t, const float u) : Vector(s, t) {
        _value[2] = u;
     }

    Vector(const float s, const float t, const float u, const float v) : Vector(s, t, u) {
        _value[3] = v;
     }
    
    Vector(const Vector<N> &other) {
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

protected:
    float _value[M * N];

};

typedef Matrix<3, 3> Mat3;
typedef Matrix<4, 4> Mat4;

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

#pragma region Math



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

#pragma region Transform

Mat4 translate(float dx, float dy, float dz);

Mat4 rotate(float angle, float x, float y, float z);

Mat4 scale(float sx, float sy, float sz) ;
        
Mat4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);

Mat4 ortho(float left, float right, float bottom, float top, float near, float far);

Mat4 frustum(float left, float right, float bottom, float top, float near, float far);

Mat4 perspective(float fovy, float aspect, float zNear, float zFar);

#pragma endregion

void drawVertices(Vec3 *vertices, int vertexSize, Color3F *colors, int colorSize);

#endif