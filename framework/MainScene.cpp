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

    computeHilbertCurve(5, (length - offset) * 2, offset, origin);

    rect = Primitives::create();

    // draw hilbert curve
    // for (int i = 0; i < vertices.size() - 1; i++) {
    //     rect->drawLine(vertices[i], vertices[i + 1], Color4F::RED);
    // }

    // // very small rect size(40, 40) - count(80, 120)
    // for (int i = 0; i < random(80, 120); i++)
    //     drawRect(Vec4(vertices[random(0, vertices.size() - 1)].x, vertices[random(0, vertices.size() - 1)].y, 0, 1), random(40, 40), random(5, 15));

    // // small rect size(40, 120) - count(20, 45)
    // // for (int i = 0; i < random(20, 40); i++)
    //     // drawRect(Vec4(vertices[random(0, vertices.size() - 1)].x, vertices[random(0, vertices.size() - 1)].y, 0, 1), random(40, 120), random(15, 30));

    // // normal rect size(120, 320) - count(10, 20)
    // for (int i = 0; i < random(10, 20); i++)
    //     drawRect(Vec4(vertices[random(0, vertices.size() - 1)].x, vertices[random(0, vertices.size() - 1)].y, 0, 1), random(120, 320), random(15, 30));

    // // big rect size(320, 400) - count(5, 10)
    // for (int i = 0; i < random(5, 10); i++)
    //     drawRect(Vec4(vertices[random(0, vertices.size() - 1)].x, vertices[random(0, vertices.size() - 1)].y, 0, 1), random(320, 400), random(15, 30));

    makeRects(20, random(5, 10), random(80, 120));
    makeRects(40, random(10, 15), random(40, 60));
    makeRects(random(80, 120), random(30, 40), random(15, 25));
    makeRects(random(240, 400), random(30, 40), random(5, 10));

    GLint offsetMenu = glutCreateMenu(0);
    glutAddMenuEntry("7", 1);
    glutAddMenuEntry("5", 2);
    glutAddMenuEntry("3", 3);

    GLint minMaxNumberMenu = glutCreateMenu(0);
    glutAddMenuEntry("5/10", 4);
    glutAddMenuEntry("10/15", 5);
    glutAddMenuEntry("80/120", 6);

    GLint groupRatioMenu = glutCreateMenu(0);
    glutAddMenuEntry("1", 4);
    glutAddMenuEntry("2", 5);
    glutAddMenuEntry("3", 6);

    glutCreateMenu(0);
    glutAddSubMenu("offset", offsetMenu);
    glutAddSubMenu("square min/max number", minMaxNumberMenu);
    glutAddSubMenu("square group ratio", groupRatioMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void MainScene::makeRects(float size, int period, int count) {
    std::vector<Vec4> pos;

    for (;pos.size() < count ;) {
        auto v = Vec4(vertices[random(0, vertices.size() - 1)].x, vertices[random(0, vertices.size() - 1)].y, 0, 1);

        bool escape = false;
        for (int i = 0; i < pos.size(); i++) {
            if (v.x == pos[i].x && v.y == pos[i].y) {
                escape = true; break;
            }
        }
        if (escape) continue;
        pos.push_back(v);
    }
    for (auto v : pos)
        drawRect(v, size, period);
}

void MainScene::drawRect(const Vec4 &pos, float size, int n) {
    n = size / n;
    for (int s = n; s <= size; s += n) {
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

int MainScene::random(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void MainScene::draw() {
    glEnable(GL_DEPTH_TEST);
    rect->draw();
    glDisable(GL_DEPTH_TEST);
}

void MainScene::released() {
    rect->release();
}