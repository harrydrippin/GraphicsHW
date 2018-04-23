#include "MainScene.h"

MainScene * MainScene::create() {
    auto ret = new MainScene();
    
    return ret;
}

void MainScene::initialized() {
    shader = Shader::create("./shader/vertex.glsl", "./shader/fragment.glsl");

    shader->addUniform("u_pvm");
    shader->addAttrib("a_color");
    shader->addAttrib("a_position");

    modelMatrix(0, 0) = 1;
    modelMatrix(1, 1) = 1;
    modelMatrix(2, 2) = 1;
    modelMatrix(3, 3) = 1;

    v.push_back(Vec4(0.5f, 0.5f, -1.0f, 1.0f));
    v.push_back(Vec4(-0.5f, 0.5f, -1.0f, 1.0f));
    v.push_back(Vec4(-0.5f, -0.5f, -1.0f, 1.0f));

    c.push_back(Color4F::GREEN);
    c.push_back(Color4F::GREEN);
    c.push_back(Color4F::GREEN);
}

void MainScene::update() {
    modelMatrix = modelMatrix * translate(-0.001, -0.001, 0);
}

void MainScene::draw() {
    glUseProgram(shader->getProgram());

    glUniformMatrix4fv(shader->getUniformLocation("u_pvm"), 1, GL_FALSE, modelMatrix);
    glVertexAttribPointer(shader->getAttribLocation("a_position"), 4, GL_FLOAT, GL_FALSE, 0, v.data());
    glVertexAttribPointer(shader->getAttribLocation("a_color"), 4, GL_FLOAT, GL_FALSE, 0, c.data());

    shader->enable();

    glDrawArrays(GL_TRIANGLES, 0, v.size());

    shader->disable();

    glUseProgram(0);
}

void MainScene::released() {
    delete shader;
}