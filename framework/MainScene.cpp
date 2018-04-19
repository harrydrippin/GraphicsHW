#include "MainScene.h"

MainScene * MainScene::create() {
    auto ret = new MainScene();
    
    return ret;
}

void MainScene::initialized() {
    v.push_back(Vec3(0.5f, 0.5f, -1.0f));
    v.push_back(Vec3(-0.5f, 0.5f, -1.0f));
    v.push_back(Vec3(-0.5f, -0.5f, -1.0f));

    c.push_back(Color3F::GREEN);
    c.push_back(Color3F::RED);
    c.push_back(Color3F::MAGENTA);
}

void MainScene::update() {
    drawVertices(v.data(), v.size(), c.data(), c.size());
}

void MainScene::released() {
    
}