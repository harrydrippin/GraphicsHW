#include "MainScene.h"

MainScene * MainScene::create() {
    auto ret = new MainScene();
    
    return ret;
}

void MainScene::initialized() {
    Vec2 origin(240, 240);

    rect = Primitives::create(2);

    rect->drawSolidRectangle(Vec4(origin.x, origin.y, 0, 1), Vec4(origin.x + 100, origin.y + 100, 0, 1), Color4F(0.1, 0.7, 0.8, 1));
    rect->drawSolidRectangle(Vec4(20, 20, 0, 1), Vec4(120, 120, 0, 1), Color4F(0.4, 0.3, 0.9, 1));

    // shader = Shader::create("./shader/vertex.glsl", "./shader/fragment.glsl");

    // shader->addUniform("u_pvm");
    // shader->addAttrib("a_color");
    // shader->addAttrib("a_position");

    // modelMatrix(0, 0) = 1;
    // modelMatrix(1, 1) = 1;
    // modelMatrix(2, 2) = 1;
    // modelMatrix(3, 3) = 1;

    // float size = 240;
    // Vec2 pos = origin;

    // v.push_back(Vec4(pos.x - size / 2, pos.y - size / 2, 0, 1.0f));
    // v.push_back(Vec4(pos.x + size / 2, pos.y - size / 2, 0, 1.0f));
    // v.push_back(Vec4(pos.x - size / 2, pos.y + size / 2, 0, 1.0f));
    // v.push_back(Vec4(pos.x + size / 2, pos.y - size / 2, 0, 1.0f));
    // v.push_back(Vec4(pos.x + size / 2, pos.y + size / 2, 0, 1.0f));
    // v.push_back(Vec4(pos.x - size / 2, pos.y + size / 2, 0, 1.0f));

    // for (int i = 0; i < v.size(); i++) c.push_back(Color4F(0.1, 0.7, 0.8, 1.0));

    // size = 200;
    // pos = origin;

    // v2.push_back(Vec4(pos.x - size / 2, pos.y - size / 2, 0, 1.0f));
    // v2.push_back(Vec4(pos.x + size / 2, pos.y - size / 2, 0, 1.0f));
    // v2.push_back(Vec4(pos.x - size / 2, pos.y + size / 2, 0, 1.0f));
    // v2.push_back(Vec4(pos.x + size / 2, pos.y - size / 2, 0, 1.0f));
    // v2.push_back(Vec4(pos.x + size / 2, pos.y + size / 2, 0, 1.0f));
    // v2.push_back(Vec4(pos.x - size / 2, pos.y + size / 2, 0, 1.0f));

    // for (int i = 0; i < v2.size(); i++) c2.push_back(Color4F(0.2, 0.6, 0.9, 1.0));
}

void MainScene::update() {
    // modelMatrix = modelMatrix * translate(-0.001, -0.001, 0);
}

void MainScene::draw() {
    rect->draw();
    // glEnable(GL_DEPTH_TEST);
    // drawVertices(v2.data(), v2.size(), c2.data(), c2.size());
    // drawVertices(v.data(), v.size(), c.data(), c.size());
    // glDisable(GL_DEPTH_TEST);

    // shader->use();

    // glUniformMatrix4fv(shader->getUniformLocation("u_pvm"), 1, GL_FALSE, modelMatrix);

    // glVertexAttribPointer(shader->getAttribLocation("a_position"), 4, GL_FLOAT, GL_FALSE, 0, v.data());
    // glVertexAttribPointer(shader->getAttribLocation("a_color"), 4, GL_FLOAT, GL_FALSE, 0, c.data());
    
    // shader->enable();

    // glDrawArrays(GL_TRIANGLE_STRIP, 0, v.size());

    // shader->disable();

    // shader->unUse();

}

void MainScene::released() {
    rect->release();
}