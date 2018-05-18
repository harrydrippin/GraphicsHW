#include "help.h"

#include <iostream>

using namespace help;

class MainScene : public help::Scene {
public:
    static MainScene * create() {
        auto ret = new MainScene();
        return ret;
    }

    void start() {
        auto desk = Obj3D::create("./data/desk.obj");
        desk->setPosition(Vec3(0, 0, 0));
        addChild(desk);
    }

    void onKeyboardPress(unsigned char keycode, int x, int y) {
        if (keycode =='a' || keycode == 'A') {
            Camera::get()->rotateLeft(10);
        }

        if (keycode == 'd' || keycode == 'D') {
            Camera::get()->rotateRight(10);
        }
    }

    void onSpecialKeyboardPress(int keycode, int x, int y) {
        if (keycode == GLUT_KEY_UP) Camera::get()->moveForward(0.1);
        if (keycode == GLUT_KEY_DOWN) Camera::get()->moveBackward(0.1);
        if (keycode == GLUT_KEY_LEFT) Camera::get()->moveLeft(0.1);
        if (keycode == GLUT_KEY_RIGHT) Camera::get()->moveRight(0.1);
    }

    void update() {
    }

    void end() {
        
    }

};