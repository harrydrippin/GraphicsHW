#include "help.h"

#include "SOIL.h"

#include <iostream>
#include <ctime>
#include <chrono>

using namespace help;

class MainScene : public help::Scene {
public:
    static MainScene * create() {
        auto ret = new MainScene();
        return ret;
    }

    void start() {
        teapot = Obj3D::create("./data/desk_with_normals.obj", "./shader/texture_vert.glsl", "./shader/texture_frag.glsl");
        teapot->print();
        teapot->setPosition(Vec3(0, 0, 0));
        teapot->setScale(Vec3(2, 2, 2));

        prev = curr = std::chrono::system_clock::now();
    }

    void update() {
        curr = std::chrono::system_clock::now();

        std::chrono::duration<float> elaped_seconds = (curr - prev);

        angle += 100 * elaped_seconds.count();
        // teapot->setRotation(angle, Vec3(1, 0, 0));

        prev = curr;
    }

    void draw() {
        teapot->draw();
    }

    void end() {
        teapot->release();
    }

public:
    Obj3D *teapot;

    float angle = 0.0;

    std::chrono::time_point<std::chrono::system_clock> prev, curr;

};