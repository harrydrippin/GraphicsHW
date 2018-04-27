#include "MainScene.h"

#include <ctime>
#include <cstdlib>

using namespace std;

MainScene * MainScene::create() {
    auto ret = new MainScene();
    
    return ret;
}

void MainScene::initialized() {
    std::srand(std::time(NULL));

    Vec4 origin(320, 320, 0, 1);

    float offset = (640 / 4);
    float length = (640 / 2);

    computeHilbertCurve(6, (length - offset) * 2, offset, origin);

    rect = Primitives::create();

    for (int i = 0; i < vertices.size() - 1; i++) {
        rect->drawLine(vertices[i], vertices[i + 1], Color4F::RED);
    }

    // for (int i = 0; i < 10; i++)
        // drawRect(Vec4(rand() % 641, rand() % 641, 0, 1), (rand() % 20 + 2) * 20);
}

void MainScene::drawRect(const Vec4 &pos, float size) {
    for (int s = 20; s <= size; s += 20) {
        float r = (rand() % 101) / 100.0f;
        float g = (rand() % 101) / 100.0f;
        float b = (rand() % 101) / 100.0f;
        rect->drawSolidRectangle(Vec4(pos.x - s / 2, pos.y - s / 2, 0, 1),
                                Vec4(pos.x + s / 2, pos.y + s / 2), Color4F(r, g, b, 1));
    }
}

void MainScene::computeHilbertCurve(int iter, float length, float offset, const Vec4 &pos) {
    string hilb = "a";
    
    Vec4 p = pos;
    
    for (int i = 0; i < iter; i++) {
        string tmp = "";
        
        for (auto c : hilb) {
            if (c == 'a') tmp += "-bF+aFa+Fb-";
            else if (c == 'b') tmp += "+aF-bFb-Fa+";
            else tmp += c;
        }

        hilb = tmp;
        
        if (i == 0) continue;
        
        p = Vec4(p.x - offset, p.y - offset, 0, 1);
        offset /= 2;
        length /= 2;
    }
    
    p = Vec4(p.x - offset, p.y - offset, 0, 1);
    
    // 0: right, 1: down, 2: left, 3: up
    int dir = 0;
    vertices.push_back(p);
    
    for (auto c : hilb) {
        int s = 0;
        if (c == 'F') {
            switch(dir) {
                case 0: vertices.push_back(vertices.back() + Vec4(1 * length, 0, 0, 0)); break;
                case 1: vertices.push_back(vertices.back() + Vec4(0, -1 * length, 0, 0)); break;
                case 2: vertices.push_back(vertices.back() + Vec4(-1 * length, 0, 0, 0)); break;
                case 3: vertices.push_back(vertices.back() + Vec4(0, 1 * length, 0, 0)); break;
            }
        } else if (c == '+') {
            dir += 1;
            if (dir > 3) dir = 0;
        } else if (c == '-') {
            dir -= 1;
            if (dir < 0) dir = 3;
        }
    }
}

void MainScene::draw() {
    glEnable(GL_DEPTH_TEST);
    rect->draw();
    glDisable(GL_DEPTH_TEST);
}

void MainScene::released() {
    rect->release();
}