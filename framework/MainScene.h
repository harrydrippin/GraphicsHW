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
        desk = Obj3D::create("./data/desk.obj");
        desk->setPosition(Vec3(-5, 0, 0));
        desk->setScale(Vec3(1.5, 1.5, 1.5));
        addChild(desk);

        fan = Obj3D::create("./data/fan.obj");
        fan->setPosition(Vec3(0, 5, 0));
        fan->setScale(Vec3(1.5, 1.5, 1.5));
        addChild(fan);

        sofa = Obj3D::create("./data/sofa.obj");
        sofa->setPosition(Vec3(0, 0, 5));
        sofa->setRotation(180, Vec3(0, 1, 0));
        sofa->setScale(Vec3(1.5, 1.5, 1.5));
        addChild(sofa);

        tv = Obj3D::create("./data/tv.obj");
        tv->setPosition(Vec3(0, 0, -5));
        tv->setScale(Vec3(2, 2, 2));
        addChild(tv);
    }

    void onKeyboardPress(unsigned char keycode, int x, int y) {
        if (keycode =='a' || keycode == 'A') {
            Camera::get()->rotateLeft(10);
        }

        if (keycode == 'd' || keycode == 'D') {
            Camera::get()->rotateRight(10);
        }

        // if (keycode == 'w' || keycode == 'W') {
        //     fovy += 10;
        //     Camera::get()->setFovy(fovy);
        // }

        // if (keycode =='s' || keycode == 'S') {
        //     fovy -= 10;
        //     Camera::get()->setFovy(fovy);
        // }
    }

    void onSpecialKeyboardPress(int keycode, int x, int y) {
        if (keycode == GLUT_KEY_UP) Camera::get()->moveForward(1);
        if (keycode == GLUT_KEY_DOWN) Camera::get()->moveBackward(1);
        if (keycode == GLUT_KEY_LEFT) Camera::get()->moveLeft(1);
        if (keycode == GLUT_KEY_RIGHT) Camera::get()->moveRight(1);
    }

    void update() {
        fan->setRotation(3, Vec3(0, 1, 0));
    }

    void end() {
        
    }

public:
    Obj3D * desk, * fan, * sofa, * tv;

    float fovy = 60;
};