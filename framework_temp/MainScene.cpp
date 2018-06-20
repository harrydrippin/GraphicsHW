#include "MainScene.h"

using namespace std;

MainScene * MainScene::create() {
    auto ret = new MainScene();
    
    return ret;
}

void MainScene::initialized() {
    shader = Shader::create("./shader/vertex.glsl", "./shader/fragment.glsl");
    shader->addUniform("u_mvp");
    shader->addAttrib("a_position");
    shader->addAttrib("a_color");

    float sx = -0.5, sy = -0.5, dx = 0.5, dy = 0.5;

    vertices.push_back(Vec3(sx, sy, 0));
    vertices.push_back(Vec3(dx, sy, 0));
    vertices.push_back(Vec3(sx, dy, 0));

    vertices.push_back(Vec3(dx, sy, 0));
    vertices.push_back(Vec3(dx, dy, 0));
    vertices.push_back(Vec3(sx, dy, 0));

    for (int i = 0; i < vertices.size(); i++) colors.push_back(Color4F::BLUE);

    mm.setIdentity();
    vm = lookAt(ex, ey, 0.1, 
                cx, cy, 0, 
                0, 1, 0);
}

void MainScene::keyInput(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') {
        ex -= 0.01;
        // cx -= 0.01;
        glutPostRedisplay();
    }
    if (key == 'd' || key == 'D') {
        ex += 0.01;
        // cx += 0.01;
        glutPostRedisplay();
    }
    if (key == 'w' || key == 'W') {
        ey += 0.01;
        // cy += 0.01;
        glutPostRedisplay();
    }
    if (key == 's' || key == 'S') {
        ey -= 0.01;
        // cy -= 0.01;
        glutPostRedisplay();
    }

    if (key == 'q' || key == 'Q') {
        fovy -= 5;
        glutPostRedisplay();
    }

    if (key == 'e' || key == 'E') {
        fovy += 5;
        glutPostRedisplay();
    }
}

void MainScene::update() {

}

void MainScene::draw() {
    glEnable(GL_DEPTH_TEST);

    shader->use();

    vm = lookAt(ex, ey, 0.5, 
                cx, cy, 0, 
                0, 1, 0);

    auto pm = perspective(fovy, 1, 0.01, 1000);

    auto mvp = pm * vm * mm;//Director::getInstance()->getProjectionMatrix() * vm * mm;

    glUniformMatrix4fv(shader->getUniformLocation("u_mvp"), 1, GL_FALSE, mvp);
    glVertexAttribPointer(shader->getAttribLocation("a_position"), 3, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glVertexAttribPointer(shader->getAttribLocation("a_color"), 4, GL_FLOAT, GL_FALSE, 0, colors.data());

    shader->enable();
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    shader->disable();

    shader->unUse();

    glDisable(GL_DEPTH_TEST);
}

void MainScene::released() {
    shader->release();
}