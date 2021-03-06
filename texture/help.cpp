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
    glutIdleFunc(Application::idleFunction);
    glutKeyboardFunc(Application::keyboardFunction);
    glutSpecialFunc(Application::specialFunction);
    glutPassiveMotionFunc(Application::mouseMoveFunction);

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

void Application::specialFunction(int keycode, int x, int y) {
    SceneManager::get()->getScene()->onSpecialKeyboardPress(keycode, x, y);

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

void Camera::moveVertical(float delta) {
    _position += _frontDirection * delta;
}

void Camera::moveHorizontal(float delta) {
    _position += _rightDirection * delta;
}

void Camera::rotate(float delta) {
    auto ret = help::rotate(delta, 0, -1, 0) * Vec4(_frontDirection, 1);
    _frontDirection.x = ret.x;
    _frontDirection.y = ret.y;
    _frontDirection.z = ret.z;
    
    _rightDirection = _frontDirection.cross(_upDirection);
    _upDirection = _rightDirection.cross(_frontDirection);
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
    _trMat = translate(pos.x, pos.y, pos.z);
}

void Node::setRotation(float angle, const Vec3 &rot) {
    _rotMat = rotate(angle, rot.x, rot.y, rot.z);
}

void Node::setScale(const Vec3 &s) {
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

Obj3D * Obj3D::create(const std::string &file) {
    auto ret = new Obj3D();
    if (ret->init(file)) return ret;
    
    return nullptr;
}

bool Obj3D::init(const std::string& filename) {
	std::ifstream file(filename.c_str());

	if (!file.is_open()) {
		std::cerr << "failed to open file: " << filename << std::endl;
		return false;
	}

	std::vector<Vec3> tmp_vertices;
	std::vector<Vec2> tmp_texcoords;
	std::vector<Vec3> tmp_normals;

	std::string line;
	std::locale loc;

	float x, y, z;
	std::string type_str;
	char slash;				// get only on character '\'

	std::stringstream ss;

	while (!file.eof()) {
		std::getline(file, line);

		ss.clear();
		ss.str(line);

		// comment or space		
		if (line[0] == '#' || std::isspace(line[0], loc)) {
			continue; // skip
		} else if (line.substr(0, 2) == "v ") { // vertex 
			Vec3 vertex;
			ss >> type_str >> vertex.x >> vertex.y >> vertex.z;
			tmp_vertices.push_back(vertex);
		} else if (line.substr(0, 3) == "vt ") { // texture coordinate
			Vec2 texcoord;
			ss >> type_str >> texcoord.x >> texcoord.y;
			tmp_texcoords.push_back(texcoord);
		} else if (line.substr(0, 3) == "vn ") { // vertex normal
			Vec3 norm;
			ss >> type_str >> norm.x >> norm.y >> norm.z;
			tmp_normals.push_back(norm);
		} else if (line.substr(0, 2) == "f ") { // faces
            int size = 3;//std::count(line.begin(), line.end(), ' ') - 1;

            ss >> type_str;
            int vert_idx, coord_idx, norm_idx;
            for (int i = 0; i < size; i++) {
                ss >> vert_idx >> slash /*>> coord_idx*/ >> slash >> norm_idx;
			    _vertices.push_back(tmp_vertices[vert_idx - 1]);
                _normals.push_back(tmp_normals[norm_idx - 1]);
            }
		}
	}
	
	std::cout << "finished to read: " << filename << std::endl;

    _modelMatrix.setIdentity();

	return true;
}

void Obj3D::onDraw() {
    transform();
    
	glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
}

void Obj3D::release() {
    _vertices.clear();
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

};