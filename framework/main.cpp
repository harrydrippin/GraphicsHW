#include "help.h"

#include "MainScene.h"

using namespace help;

int main(int argc, char ** argv) {
    Application app("title", 640, 640);

    if (!app.initialize(argc, argv)) return 1;

    return app.run(MainScene::create());
}