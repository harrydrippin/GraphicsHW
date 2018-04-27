#include "gg.h"

#include "MainScene.h"

#include <iostream>

int main(int argc, char * argv[]) {
    Application app;

    if (!app.initialize("test", 640, 640, &argc, argv)) return 1;

    return app.run(MainScene::create());
}