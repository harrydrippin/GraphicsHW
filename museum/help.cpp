#include "help.h"

#include "SOIL.h"

#include <sstream>
#include <iostream>
#include <algorithm>
#include <locale>

using namespace std;

namespace help {

#pragma region Color4F

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

#pragma endregion

#pragma region Application

Application::Application(const std::string &title, int width, int height) :
_title(title),
_width(width),
_height(height) {
}

bool Application::initialize(int argc, char ** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(_width, _height);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow(_title.c_str());

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cout << "cannot initialize GLEW" << std::endl;
        return false;
    }

    glClearColor(1, 1, 1, 1);

    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Director::getInstance()->setProjectionMatrix(ortho(0, width, 0, height, -1, 1));

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(Application::displayFunction);
    glutKeyboardFunc(Application::keyboardFunction);
    glutKeyboardUpFunc(Application::keyboardUpFunction);
    glutSpecialFunc(Application::specialFunction);
    glutPassiveMotionFunc(Application::mouseMoveFunction);
    glutMouseFunc(Application::mouseDownFunction);
    glutIdleFunc(Application::idleFunction);

    return true;
}

int Application::run(Scene * scene) {
    SceneManager::get()->setScene(scene);

    glutMainLoop();

    return 0;
}

void Application::displayFunction() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SceneManager::get()->getScene()->onDraw();

    glutSwapBuffers();
}

void Application::idleFunction() {
    SceneManager::get()->getScene()->update();

    glutPostRedisplay();
}

void Application::keyboardFunction(unsigned char keycode, int x, int y) {
    SceneManager::get()->getScene()->onKeyboardPress(keycode, x, y);

    glutPostRedisplay();
}

void Application::keyboardUpFunction(unsigned char keycode, int x, int y) {
    SceneManager::get()->getScene()->onKeyboardRelease(keycode, x, y);

    glutPostRedisplay();
}

void Application::specialFunction(int keycode, int x, int y) {
    SceneManager::get()->getScene()->onSpecialKeyboardPress(keycode, x, y);

    glutPostRedisplay();
}

void Application::mouseDownFunction(int button, int state, int x, int y) {
    SceneManager::get()->getScene()->onMouseDown(button, state, x, y);

    glutPostRedisplay();
}

void Application::mouseMoveFunction(int x, int y) {
    SceneManager::get()->getScene()->onMouseMove(x, y);

    glutPostRedisplay();
}

#pragma endregion

#pragma region Object 

void Object::release() {
    for (auto &child : _children) child->release();

    delete this;
}

void Object::addChild(Object * child) {
    _children.push_back(child);
}

void Object::removeChild(Object *child) {
    auto iter = std::find(_children.begin(), _children.end(), child);
    if (iter != _children.end()) {
        (*iter)->release();
        _children.erase(iter);
    }
}

#pragma endregion

#pragma region Scene 

void Scene::init() {
    start();
}

void Scene::onDraw() {
    for (auto &child : _children) child->onDraw();
    draw();
}

void Scene::release() {
    end();

    Object::release();
}

#pragma endregion

#pragma region SceneManager 

SceneManager SceneManager::_instance;

SceneManager::~SceneManager() {
    _currentScene->release();
}

SceneManager * SceneManager::get() {
    return &_instance;
}

void SceneManager::setScene(Scene * scene) {
    if (_currentScene) _currentScene->release();

    _currentScene = scene;

    _currentScene->init();
}

Scene * SceneManager::getScene() {
    return _currentScene;
}

#pragma endregion

#pragma region Camera

Camera Camera::_instance = Camera();

Camera * Camera::get() {
    return &_instance;
}

Mat4 Camera::getView() {
    return lookAt(_position.x, _position.y, _position.z,
                  _position.x + _frontDirection.x, _position.y + _frontDirection.y, _position.z + _frontDirection.z,
                  _upDirection.x, _upDirection.y, _upDirection.z);
}

Mat4 Camera::getProjection() {
    return perspective(_fovy, 1.0f, 0.001f, 10000.0f);
}

Vec3 Camera::getPosition() {
    return _position;
}

void Camera::rotate(float x, float y) {
    _yaw += x;
    _pitch -= y;

    if (_pitch > 90) _pitch = 90 - 1;
    if (_pitch < -90) _pitch = -90 + 1;

    // deg to rad
    float yaw = _yaw * 0.0174533, pitch = _pitch * 0.0174533;

    _frontDirection.x = -sin(yaw) * cos(pitch);
    _frontDirection.y = -sin(pitch);
    _frontDirection.z = -cos(yaw) * cos(pitch);

    _rightDirection.x = -cos(yaw);
    _rightDirection.y = 0.0;
    _rightDirection.z = sin(yaw);

    _upDirection = _frontDirection.cross(_rightDirection);

    _frontDirection.normalize();
    _rightDirection.normalize();
    _upDirection.normalize();
}

void Camera::setFovy(float fovy) {
    _fovy = fovy;
}

float Camera::getFovy() const {
    return _fovy;
}

#pragma endregion

#pragma region Node

Node::Node() {
    _trMat.setIdentity();
    _rotMat.setIdentity();
    _sclMat.setIdentity();
}

void Node::setPosition(const Vec3 &pos) {
    _position = pos;
    _trMat = translate(pos.x, pos.y, pos.z);
}

void Node::setRotation(float angle, const Vec3 &rot) {
    _rotMat = rotate(angle, rot.x, rot.y, rot.z);
}

void Node::setScale(const Vec3 &s) {
    _scale = s;
    _sclMat = scale(s.x, s.y, s.z);
}

void Node::transform() {
    _modelMatrix = _trMat * _rotMat * _sclMat;
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

    height = zNear * std::tan(rfovy);
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

Shader * Shader::createWithString(const std::string &vert, const std::string &frag) {
    auto ret = new Shader();

    ret->initWithString(vert, frag);
        
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

void Shader::initWithString(const std::string &vert, const std::string &frag) {
    GLuint vertexShader = createShaderFromString(vert, GL_VERTEX_SHADER); 
    GLuint fragmentShader = createShaderFromString(frag, GL_FRAGMENT_SHADER);

    _program = glCreateProgram();

    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);
}

GLuint Shader::getProgram() const {
    return _program;
}

GLuint Shader::createShaderFromFile(const std::string &file, GLuint shaderType) {
    std::ifstream shaderFile(file.c_str());
    std::string shaderString;
    shaderString.assign(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());

    return createShaderFromString(shaderString, shaderType);
}

GLuint Shader::createShaderFromString(const std::string &str, GLuint shaderType) {
    GLuint ret = glCreateShader(shaderType);

    const GLchar *shaderSrc = str.c_str();

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

void Shader::printError(const std::string& op) {
    int error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::string errorStr = "UNKNOWN";
		switch (error) {
		case 0:
			errorStr = "GL_NO_ERROR";
			break;
		case 1280:
			errorStr = "GL_INVALID_ENUM";
			break;
		case 1281:
			errorStr = "GL_INVALID_VALUE";
			break;
		case 1282:
			errorStr = "GL_INVALID_OPERATION";
			break;
		case 1283:
			errorStr = "GL_STACK_OVERFLOW";
			break;
		case 1284:
			errorStr = "GL_STACK_UNDERFLOW";
			break;
		case 1285:
			errorStr = "GL_OUT_OF_MEMORY";
			break;
		default:
			break;
		}
		std::cerr << op << " glError " << errorStr << std::endl;
	}
}

#pragma endregion

#pragma region Primitive2D

Primitive2D::Primitive2D(float width) : _lineWidth(width) {
}

Primitive2D * Primitive2D::create(float width) {
    auto ret = new Primitive2D(width);
    ret->init();

    return ret;
}

bool Primitive2D::init() {
    string vert = "\
        #version 120\n\
        \
        uniform mat4 u_pvm;\
        attribute vec3 a_position;\
        attribute vec4 a_color;\
        varying vec4 v_color;\
        \
        void main() {\
            v_color = a_color;\
            gl_Position = u_pvm * vec4(a_position, 1);\
        }\
    ";

    string frag = "\
        #version 120\n\
        \
        varying vec4 v_color;\
        \
        void main() {\
            gl_FragColor = v_color;\
        }\
    ";

    _shader = Shader::createWithString(vert, frag);

    _shader->addUniform("u_pvm");
    _shader->addAttrib("a_position");
    _shader->addAttrib("a_color");

    _modelMatrix.setIdentity();
}

void Primitive2D::drawPoint(const Vec3 &pos, float size, const Color4F &color) {
    _pointVertices.push_back(pos);

    _pointColors.push_back(color);
}

void Primitive2D::drawLine(const Vec3 &p1, const Vec3 &p2, const Color4F &color) {
    _lineVertices.push_back(p1);
    _lineVertices.push_back(p2);

    _lineColors.push_back(color);
    _lineColors.push_back(color);
}

void Primitive2D::drawRectangle(const Vec3 &origin, const Vec3 &dest, const Color4F &color) {
    drawLine(Vec3(origin.x, origin.y, 0), Vec3(dest.x, origin.y, 0), color);
    drawLine(Vec3(dest.x, origin.y, 0), Vec3(dest.x, dest.y, 0), color);
    drawLine(Vec3(dest.x, dest.y, 0), Vec3(origin.x, dest.y, 0), color);
    drawLine(Vec3(origin.x, dest.y, 0), Vec3(origin.x, origin.y, 0), color);
}

void Primitive2D::drawSolidRectangle(const Vec3 &origin, const Vec3 &dest, const Color4F &color) {
    _polygonVertices.push_back(Vec3(origin.x, origin.y, origin.z));
    _polygonVertices.push_back(Vec3(dest.x, origin.y, origin.z));
    _polygonVertices.push_back(Vec3(origin.x, dest.y, origin.z));

    _polygonVertices.push_back(Vec3(dest.x, origin.y, origin.z));
    _polygonVertices.push_back(Vec3(dest.x, dest.y, origin.z));
    _polygonVertices.push_back(Vec3(origin.x, dest.y, origin.z));

    for (int i = 0; i < 6; i++) _polygonColors.push_back(color);
}

void Primitive2D::setShader(Shader * shader) {
    _shader->release();
    _shader = shader;
}

void Primitive2D::onDraw() {
    //draw point
    _shader->use();

    transform();
    auto pm = ortho(0, 640, 0, 640, -1, 1) * _modelMatrix;

    glUniformMatrix4fv(_shader->getUniformLocation("u_pvm"), 1, GL_FALSE, pm);

    glVertexAttribPointer(_shader->getAttribLocation("a_position"), 3, GL_FLOAT, GL_FALSE, 0, _pointVertices.data());
    glVertexAttribPointer(_shader->getAttribLocation("a_color"), 4, GL_FLOAT, GL_FALSE, 0, _pointColors.data());

    _shader->enable();
    glPointSize(_lineWidth);
    glDrawArrays(GL_POINTS, 0, _pointVertices.size());
    _shader->disable();

    _shader->unUse();

    //draw line
    _shader->use();

    glUniformMatrix4fv(_shader->getUniformLocation("u_pvm"), 1, GL_FALSE, pm);

    glVertexAttribPointer(_shader->getAttribLocation("a_position"), 3, GL_FLOAT, GL_FALSE, 0, _lineVertices.data());
    glVertexAttribPointer(_shader->getAttribLocation("a_color"), 4, GL_FLOAT, GL_FALSE, 0, _lineColors.data());

    _shader->enable();
    glLineWidth(_lineWidth);
    glDrawArrays(GL_LINES, 0, _lineVertices.size());
    _shader->disable();

    _shader->unUse();

    //draw polygon
    _shader->use();

    glUniformMatrix4fv(_shader->getUniformLocation("u_pvm"), 1, GL_FALSE, pm);

    glVertexAttribPointer(_shader->getAttribLocation("a_position"), 3, GL_FLOAT, GL_FALSE, 0, _polygonVertices.data());
    glVertexAttribPointer(_shader->getAttribLocation("a_color"), 4, GL_FLOAT, GL_FALSE, 0, _polygonColors.data());

    _shader->enable();
    glDrawArrays(GL_TRIANGLES, 0, _polygonVertices.size());
    _shader->disable();

    _shader->unUse();
}

void Primitive2D::clear() {
    _pointVertices.clear();
    _pointColors.clear();

    _lineVertices.clear();
    _lineColors.clear();

    _polygonVertices.clear();
    _polygonColors.clear();
}

void Primitive2D::release() {
    this->clear();
    _shader->release();

    Object::release();
}

#pragma endregion

#pragma region Obj3D

Group::Group(const std::string &name) : _name(name), 
                                        _mtlName("Default") {
    _vertices.clear();
    _normals.clear();
    _texcoords.clear();
}

Material::Material() : _name("Default") {
    _ambient = Vec4(0.1, 0.1, 0.1, 1.0);
    _diffuse = Vec4(0.5, 0.5, 0.5, 1.0);
    _specular = Vec4(0.5, 0.5, 0.5, 1.0);
    _shininess = 1.0;
}

Material::Material(const std::string &name, 
                   Vec4 &ambient, 
                   Vec4 &diffuse, 
                   Vec4 &specular, 
                   float &shininess) : 
_name(name),
_ambient(ambient),
_diffuse(diffuse),
_specular(specular),
_shininess(shininess) {
}

Obj3D * Obj3D::create(const std::string &file, const std::string &vsh, const std::string &fsh) {
    auto ret = new Obj3D();
    if (ret->init(file, vsh, fsh)) return ret;
    
    return nullptr;
}

bool Obj3D::init(const std::string &filename, const std::string &vsh, const std::string &fsh) {
    if (!loadObjFile(filename)) return false;

    _shader = Shader::create(vsh, fsh);

    _shader->addAttrib("a_vertex");
    _shader->addAttrib("a_texcoord");
    _shader->addAttrib("a_normal");

    _shader->addUniform("u_pvm_matrix");
    _shader->addUniform("u_model_matrix");
    _shader->addUniform("u_view_matrix");
    _shader->addUniform("u_normal_matrix");

    _shader->addUniform("u_light_vector");
    _shader->addUniform("u_light_ambient");
    _shader->addUniform("u_light_diffuse");
    _shader->addUniform("u_light_specular");

    _shader->addUniform("u_material_ambient");
    _shader->addUniform("u_material_diffuse");
    _shader->addUniform("u_material_specular");
    _shader->addUniform("u_material_shininess");

    return true;
}

bool Obj3D::loadObjFile(const std::string &filename) {
    std::ifstream file(filename.c_str());

    if (!file.is_open()) {
        std::cerr << "failed to open file: " << filename << std::endl;
        return false;
    }

    std::string type;
    char slash;

    std::string line;
    std::locale loc;

    bool hasTexcoord = false;
    bool hasNormal = false;

    std::vector<Vec3> vertices, normals;
    std::vector<Vec2> texcoords;

    _groups.push_back(Group("Default"));
    _materials["Default"] = Material();

    PATH = filename.substr(0, filename.find_last_of('/'));

    std::stringstream ss;

    while (!file.eof()) {
        std::getline(file, line);

        ss.clear();
        ss.str(line);

        if (line[0] == '#' || std::isspace(line[0], loc)) continue;
        else if (line.substr(0, 7) == "mtllib ") {
            std::string mtlFileName;
            ss >> type >> mtlFileName;
            std::string fullpath = PATH + '/' + mtlFileName;
            loadMtlFile(fullpath);
        } else if (line.substr(0, 2) == "v ") {
            Vec3 vertex;
            ss >> type >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (line.substr(0, 3) == "vt ") {
            Vec2 texcoord;
            ss >> type >> texcoord.x >> texcoord.y;
            texcoords.push_back(texcoord);

            hasTexcoord = true;
        } else if (line.substr(0, 3) == "vn ") {
            Vec3 norm;
            ss >> type >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);

            hasNormal = true;
        } else if (line.substr(0, 2) == "g ") {
            std::string groupName;
            ss >> type >> groupName;

            _groups.push_back(Group(groupName));
        } else if (line.substr(0, 7) == "usemtl ") {
            std::string mtlName;
            ss >> type >> mtlName;

            _groups.back()._mtlName = mtlName;
        } else if (line.substr(0, 2) == "f ") {
            Group &group = _groups.back();

            std::vector<std::string> lines;

            Vec3 vertexIdx, texcoordIdx, normalIdx;

            if (!hasTexcoord && !hasNormal) { // f v
                ss >> type >> vertexIdx.x >> vertexIdx.y >> vertexIdx.z;

                group._vertices.push_back(vertices[vertexIdx.x - 1]);
                group._vertices.push_back(vertices[vertexIdx.y - 1]);
                group._vertices.push_back(vertices[vertexIdx.z - 1]);
            } else if (hasTexcoord && !hasNormal) { // f v/vt
                ss >> type >> 
                vertexIdx.x >> slash >> texcoordIdx.x >>
                vertexIdx.y >> slash >> texcoordIdx.y >>
                vertexIdx.z >> slash >> texcoordIdx.z;

                group._vertices.push_back(vertices[vertexIdx.x - 1]);
                group._vertices.push_back(vertices[vertexIdx.y - 1]);
                group._vertices.push_back(vertices[vertexIdx.z - 1]);

                group._texcoords.push_back(texcoords[texcoordIdx.x - 1]);
                group._texcoords.push_back(texcoords[texcoordIdx.y - 1]);
                group._texcoords.push_back(texcoords[texcoordIdx.z - 1]);
            } else if (!hasTexcoord && hasNormal) { // f v//vn
                ss >> type >>
                vertexIdx.x >> slash >> slash >> normalIdx.x >>
                vertexIdx.y >> slash >> slash >> normalIdx.y >>
                vertexIdx.z >> slash >> slash >> normalIdx.z;

                group._vertices.push_back(vertices[vertexIdx.x - 1]);
                group._vertices.push_back(vertices[vertexIdx.y - 1]);
                group._vertices.push_back(vertices[vertexIdx.z - 1]);

                group._normals.push_back(normals[normalIdx.x - 1]);
                group._normals.push_back(normals[normalIdx.y - 1]);
                group._normals.push_back(normals[normalIdx.z - 1]);
            } else { // f v/vt/vn
                ss >> type >>
                vertexIdx.x >> slash >> texcoordIdx.x >> slash >> normalIdx.x >>
                vertexIdx.y >> slash >> texcoordIdx.y >> slash >> normalIdx.y >>
                vertexIdx.z >> slash >> texcoordIdx.z >> slash >> normalIdx.z;

                group._vertices.push_back(vertices[vertexIdx.x - 1]);
                group._vertices.push_back(vertices[vertexIdx.y - 1]);
                group._vertices.push_back(vertices[vertexIdx.z - 1]);

                group._texcoords.push_back(texcoords[texcoordIdx.x - 1]);
                group._texcoords.push_back(texcoords[texcoordIdx.y - 1]);
                group._texcoords.push_back(texcoords[texcoordIdx.z - 1]);

                group._normals.push_back(normals[normalIdx.x - 1]);
                group._normals.push_back(normals[normalIdx.y - 1]);
                group._normals.push_back(normals[normalIdx.z - 1]);
            }
        }
    }

    std::cout << "finished to read: " << filename << std::endl;
    return true;
}

bool Obj3D::loadMtlFile(const std::string &filename) {
    std::ifstream file(filename.c_str());

    if (!file.is_open()) {
        std::cerr << "failed to open file: " << filename << std::endl;
        return false;
    }

    float r, g, b;
    std::string type;

    std::string line;
    std::locale loc;

    std::string name;
    Vec4 ambient, diffuse, specular;
    float shininess;

    bool isName = false;
    bool isKa = false;
    bool isKd = false;
    bool isKs = false;
    bool isNs = false;

    std::stringstream ss;

    while (!file.eof()) {
        std::getline(file, line);

        ss.clear();
        ss.str(line);

        if (line[0] == '#' || std::isspace(line[0], loc)) continue;
        else if (line.substr(0, 7) == "newmtl ") {
            ss >> type >> name;
            isName = true;
        } else if (line.substr(0, 3) == "Ka ") {
            ss >> type >> r >> g >> b;
            ambient = Vec4(r, g, b, 1.0);
            isKa = true;
        } else if (line.substr(0, 3) == "Kd ") {
            ss >> type >> r >> g >> b;
            diffuse = Vec4(r, g, b, 1.0);
            isKd = true;
        } else if (line.substr(0, 3) == "Ks ") {
            ss >> type >> r >> g >> b;
            specular = Vec4(r, g, b, 1.0);
            isKs = true;
        } else if (line.substr(0, 3) == "Ns ") {
            ss >> type >> shininess;
            isNs = true;
        }

        if (isName && isKa && isKd && isKs && isNs) {
            _materials[name] = Material(name, ambient, diffuse, specular, shininess);

            isName = false;
            isKa = false;
            isKd = false;
            isKs = false;
            isNs = false;
        }
    }

    std::cout << "finished to read: " << filename << std::endl;
    return true;
}

void Obj3D::print() {
    for (size_t i = 0; i < _groups.size(); ++i) {
        Group& group = _groups[i];

        Material& mtl = _materials[group._mtlName];
        Vec4& ambient = mtl._ambient;
        Vec4& diffuse = mtl._diffuse;
        Vec4& specular = mtl._specular;
        float& shininess = mtl._shininess;

        std::cout << "Group: " << group._name << std::endl;
        
        std::cout << "\tUsemtl: " << group._mtlName << std::endl;
        std::cout << "\t\tAmbient: " << ambient[0] << " " << ambient[1] << " " << ambient[2] << std::endl;
        std::cout << "\t\tDiffuse: " << diffuse[0] << " " << diffuse[1] << " " << diffuse[2] << std::endl;
        std::cout << "\t\tSpecular: " << specular[0] << " " << specular[1] << " " << specular[2] << std::endl;
        std::cout << "\t\tShininess: " << shininess << std::endl;

        std::cout << "\tVertices: " << group._vertices.size() << std::endl;
        for (size_t i = 0; i < group._vertices.size(); ++i) {
            // std::cout << "\t\t" << group._vertices[i][0] << " " << group._vertices[i][1] << " " << group._vertices[i][2] << std::endl;
        }
        std::cout << "\tTexcoord: " << group._texcoords.size() << std::endl;
        for (size_t i = 0; i < group._texcoords.size(); ++i) {
            // std::cout << "\t\t" << group._texcoords[i][0] << " " << group._texcoords[i][1] << std::endl;
        }
        std::cout << "\tNormal: " << group._normals.size() << std::endl;
        for (size_t i = 0; i < group._normals.size(); ++i) {
            // std::cout << "\t\t" << group._normals[i][0] << " " << group._normals[i][1] << " " << group._normals[i][2] << std::endl;
        }    
    }	
}

void Obj3D::onDraw() {
    transform();
    
    _shader->use();

    auto projection = Camera::get()->getProjection();
    auto viewInv    = Camera::get()->getView();
    auto pvm        = projection * viewInv * _modelMatrix;

    Mat3 norm;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            norm(i, j) = _modelMatrix(i, j); 
        }
    }
    Mat4 view = viewInv.getInverse();

    for (size_t i = 0; i < _groups.size(); i++) {
        Group &group = _groups[i];

        Material &mtl = _materials[group._mtlName];
        Vec4 &ambient = mtl._ambient;
        Vec4 &diffuse = mtl._diffuse;
        Vec4 &specular = mtl._specular;
        float &shininess = mtl._shininess;
        
        glUniformMatrix4fv(_shader->getUniformLocation("u_pvm_matrix"), 1, false, pvm);
        
        glUniformMatrix4fv(_shader->getUniformLocation("u_model_matrix"), 1, false, _modelMatrix);
        glUniformMatrix4fv(_shader->getUniformLocation("u_view_matrix"), 1, false, view);
        glUniformMatrix3fv(_shader->getUniformLocation("u_normal_matrix"), 1, false, norm);

        glUniform4fv(_shader->getUniformLocation("u_light_ambient"), 1, Vec4(1.0, 1.0, 1.0, 1.0));
        glUniform4fv(_shader->getUniformLocation("u_light_diffuse"), 1, Vec4(1.0, 1.0, 1.0, 1.0));
        glUniform4fv(_shader->getUniformLocation("u_light_specular"), 1, Vec4(1.0, 1.0, 1.0, 1.0));
        glUniform3fv(_shader->getUniformLocation("u_light_vector"), 1, Vec3(10.0, 10.0, 10.0));

        glUniform4fv(_shader->getUniformLocation("u_material_ambient"), 1, ambient);
        glUniform4fv(_shader->getUniformLocation("u_material_diffuse"), 1, diffuse);
        glUniform4fv(_shader->getUniformLocation("u_material_specular"), 1, specular);
        glUniform1f(_shader->getUniformLocation("u_material_shininess"), shininess);

        glVertexAttribPointer(_shader->getAttribLocation("a_vertex"), 3, GL_FLOAT, false, 0, group._vertices.data());
        glVertexAttribPointer(_shader->getAttribLocation("a_texcoord"), 3, GL_FLOAT, false, 0, group._texcoords.data());
        glVertexAttribPointer(_shader->getAttribLocation("a_normal"), 3, GL_FLOAT, false, 0, group._normals.data());

        _shader->enable();
        glDrawArrays(GL_TRIANGLES, 0, group._vertices.size());
        _shader->disable();
    }

    _shader->unUse();
}

void Obj3D::release() {
    _groups.clear();
    _materials.clear();
    _shader->release();
    Object::release();
}

#pragma endregion

#pragma region texture

Texture *Texture::create() {
    auto ret = new Texture();

    return ret;
}

void Texture::loadTexture(const std::string &file) {
    int width, height, channels;
    unsigned char* image = SOIL_load_image(file.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);

    GLuint texid;

    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SOIL_free_image_data(image);

    _texid.push_back(texid);
    _width.push_back(width);
    _height.push_back(height);
}

void Texture::bindTextures() {
    for (int i = 0; i < _texid.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, _texid[i]);
    }
}

void Texture::release() {
    _texid.clear();
    _width.clear();
    _height.clear();

    delete this;
}

#pragma endregion

#pragma region AABB

AABB::~AABB() {
    _shader->release();
    _debugVertices.clear();
    _debugColors.clear();
}

AABB::AABB() : _start(Vec3()), _end(Vec3()) {
}

AABB::AABB(const Vec3 &start, const Vec3 &end) : _start(start), _end(end) {
    // front
    _debugVertices.push_back(Vec3(start.x, start.y, start.z));
    _debugVertices.push_back(Vec3(end.x, start.y, start.z));
    _debugVertices.push_back(Vec3(end.x, end.y, start.z));
    _debugVertices.push_back(Vec3(start.x, end.y, start.z));
    //back
    _debugVertices.push_back(Vec3(start.x, start.y, end.z));
    _debugVertices.push_back(Vec3(end.x, start.y, end.z));
    _debugVertices.push_back(Vec3(end.x, end.y, end.z));
    _debugVertices.push_back(Vec3(start.x, end.y, end.z));
    //top
    _debugVertices.push_back(Vec3(start.x, end.y, start.z));
    _debugVertices.push_back(Vec3(end.x, end.y, start.z));
    _debugVertices.push_back(Vec3(end.x, end.y, end.z));
    _debugVertices.push_back(Vec3(start.x, end.y, end.z));
    //bottom
    _debugVertices.push_back(Vec3(start.x, start.y, start.z));
    _debugVertices.push_back(Vec3(end.x, start.y, start.z));
    _debugVertices.push_back(Vec3(end.x, start.y, end.z));
    _debugVertices.push_back(Vec3(start.x, start.y, end.z));

    for (int i = 0; i < _debugVertices.size(); i++) _debugColors.push_back(Color4F::RED);

    float x = (end.x - start.x) * 0.5, y = (end.z - start.z) * 0.5;
    _frontAngle = atan(x / y) * 57.2958;

    string vert = "\
        uniform mat4 u_pvm;\
        attribute vec3 a_position;\
        attribute vec4 a_color;\
        varying vec4 v_color;\
        \
        void main() {\
            v_color = a_color;\
            gl_Position = u_pvm * vec4(a_position, 1.0);\
        }\
    ";

    string frag = "\
        varying vec4 v_color;\
        \
        void main() {\
            gl_FragColor = v_color;\
        }\
    ";

    _shader = Shader::createWithString(vert, frag);

    _shader->addUniform("u_pvm");
    _shader->addAttrib("a_position");
    _shader->addAttrib("a_color");

    _modelMatrix.setIdentity();
}

void AABB::setPosition(const Vec3 &pos) {
    _position = pos;
    _modelMatrix = translate(pos.x, pos.y, pos.z);// * _modelMatrix;
}

bool AABB::isIntersect(AABB *other) {
    Vec3 min = _position + _start, max = _position + _end;
    Vec3 otherMin = other->_position + other->_start, otherMax = other->_position + other->_end;

    if (min.x <= otherMax.x && max.x >= otherMin.x && 
        min.y <= otherMax.y && max.y >= otherMin.y && 
        min.z <= otherMax.z && max.z >= otherMin.z) return true;

    return false;
}

bool AABB::isIntersect(const Vec3 &point) {
    Vec3 min = _position + _start, max = _position + _end;

    if (point.x >= min.x && point.x <= max.x && 
        point.y >= min.y && point.y <= max.y && 
        point.z >= min.z && point.z <= max.z) return true;

    return false;
}

void AABB::debugDraw() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    _shader->use();

    auto projection = Camera::get()->getProjection();
    auto viewInv    = Camera::get()->getView();
    auto pvm        = projection * viewInv * _modelMatrix;

    _shader->getUniformLocation("u_pvm");
    glUniformMatrix4fv(_shader->getUniformLocation("u_pvm"), 1, GL_FALSE, pvm);

    glVertexAttribPointer(_shader->getAttribLocation("a_position"), 3, GL_FLOAT, GL_FALSE, 0, _debugVertices.data());
    glVertexAttribPointer(_shader->getAttribLocation("a_color"), 4, GL_FLOAT, GL_FALSE, 0, _debugColors.data());

    _shader->enable();
    glDrawArrays(GL_QUADS, 0, _debugVertices.size());
    _shader->disable();

    _shader->unUse();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

#pragma endregion

};