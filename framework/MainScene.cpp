#include "MainScene.h"

using namespace std;

MainScene * MainScene::create() {
    auto ret = new MainScene();
    
    return ret;
}

void MainScene::initialized() {
    preColors.push_back(Color4F(0.282, 0.552, 0.788, 1));
    preColors.push_back(Color4F(0.494, 0.768, 0.474, 1));
    preColors.push_back(Color4F(0.262, 0.717, 0.478, 1));
    preColors.push_back(Color4F(0.678, 0.823, 0.470, 1));
    preColors.push_back(Color4F(1.000, 0.964, 0.619, 1));
    preColors.push_back(Color4F(0.027, 0.294, 0.494, 1));
    preColors.push_back(Color4F(0.168, 0.733, 0.701, 1));
    preColors.push_back(Color4F(0.933, 0.937, 0.600, 1));
    preColors.push_back(Color4F(0.514, 0.784, 0.518, 1));
    preColors.push_back(Color4F(0.259, 0.745, 0.643, 1));
    preColors.push_back(Color4F(0.173, 0.427, 0.506, 1));
    preColors.push_back(Color4F(0.247, 0.596, 0.765, 1));
    preColors.push_back(Color4F(0.212, 0.733, 0.647, 1));

    Vec4 origin(240, 240, 0, 1);

    float offset = (320 / 4);
    float length = (320 / 2);

    vertices = getHilbertCurve(5, (length - offset) * 2, offset, origin);

    smallSquareCount = 100;
    mediumSquareCount = 70;
    bigSquareCount = 6;

    rect = Primitive2D::create();

    drawScreen();

    GLint smallSquareCountMenu = glutCreateMenu(MainScene::menuCallback);
    glutAddMenuEntry("40", 1);
    glutAddMenuEntry("100", 2);
    glutAddMenuEntry("180", 3);

    GLint mediumSquareCountMenu = glutCreateMenu(MainScene::menuCallback);
    glutAddMenuEntry("50", 4);
    glutAddMenuEntry("70", 5);
    glutAddMenuEntry("90", 6);

    GLint bigSquareCountMenu = glutCreateMenu(MainScene::menuCallback);
    glutAddMenuEntry("4", 7);
    glutAddMenuEntry("6", 8);
    glutAddMenuEntry("8", 9);

    glutCreateMenu(MainScene::menuCallback);
    glutAddSubMenu("Small square count", smallSquareCountMenu);
    glutAddSubMenu("Medium square count", mediumSquareCountMenu);
    glutAddSubMenu("Big square count", bigSquareCountMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void MainScene::drawScreen() {
    rect->clear();

    makeRects(vertices, 20, 5, smallSquareCount);
    makeRects(vertices, random(40, 80), 15, mediumSquareCount);
    makeRects(vertices, random(160, 200), 20, bigSquareCount);
}

void MainScene::makeRects(const std::vector<Vec4> &vertices, float size, int period, int count) {
    std::vector<Vec4> pos;

    for (;pos.size() < count ;) {
        int randomIndex = random(0, vertices.size() - 1);
        auto v = Vec4(vertices[randomIndex].x, vertices[randomIndex].y, 0, 1);

        bool escape = false;
        for (int i = 0; i < pos.size(); i++) {
            if (v.x == pos[i].x && v.y == pos[i].y) {
                escape = true; break;
            }
        }
        if (escape) continue;
        pos.push_back(v);
    }
    for (auto v : pos) drawRect(v, size, period);
}

void MainScene::drawRect(const Vec4 &pos, float size, int n) {
    n = size / n;
    n = n > 0 ? n : 1;
    for (int s = n; s <= size; s += n) {
        Color4F c = preColors[random(0, preColors.size() - 1)];
        rect->drawSolidRectangle(Vec4(pos.x - s / 2, pos.y - s / 2, 0, 1),
                                Vec4(pos.x + s / 2, pos.y + s / 2, 0, 1), c);
    }
}

std::vector<Vec4> MainScene::getHilbertCurve(int iter, float length, float offset, const Vec4 &pos) {
    std::vector<Vec4> ret;

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
    ret.push_back(p);
    
    for (auto c : hilb) {
        int s = 0;
        if (c == 'F') {
            switch(dir) {
                case 0: ret.push_back(ret.back() + Vec4(1 * length, 0, 0, 0)); break;
                case 1: ret.push_back(ret.back() + Vec4(0, -1 * length, 0, 0)); break;
                case 2: ret.push_back(ret.back() + Vec4(-1 * length, 0, 0, 0)); break;
                case 3: ret.push_back(ret.back() + Vec4(0, 1 * length, 0, 0)); break;
            }
        } else if (c == '+') {
            dir += 1;
            if (dir > 3) dir = 0;
        } else if (c == '-') {
            dir -= 1;
            if (dir < 0) dir = 3;
        }
    }

    return ret;
}

void MainScene::draw() {
    glEnable(GL_DEPTH_TEST);
    rect->draw();
    glDisable(GL_DEPTH_TEST);
}

void MainScene::released() {
    rect->release();
}

void MainScene::menuCallback(int value) {
    auto scene = ((MainScene*)Director::getInstance()->getCurrentScene());
    switch(value) {
    case 1:
        scene->smallSquareCount = 40;
        break;
    case 2:
        scene->smallSquareCount = 100;
        break;
    case 3:
        scene->smallSquareCount = 180;
        break;
    case 4:
        scene->mediumSquareCount = 50;
        break;
    case 5:
        scene->mediumSquareCount = 70;
        break;
    case 6:
        scene->mediumSquareCount = 90;
        break;
    case 7:
        scene->bigSquareCount = 4;
        break;
    case 8:
        scene->bigSquareCount = 6;
        break;
    case 9:
        scene->bigSquareCount = 8;
        break;
    }
    scene->drawScreen();
}