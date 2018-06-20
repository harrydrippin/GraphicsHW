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
        rect = Primitive2D::create(2);
        rect->drawRectangle(Vec3(x, y), Vec3(x + 100, y + 100), Color4F::RED);
        addChild(rect);
    }

    void update() {
        rect->setPosition(Vec3(x, y));
        x++; y++;
    }

    void end() {
        
    }

public:
    Primitive2D * rect;
    float x = 100, y = 100;

};