#include "gg.h"

#include <vector>

class MainScene : public Scene {
public:
    static MainScene * create();

    void initialized();
    void update();
    void draw();
    void released();

private:
    // std::vector<Vec4> v, v2;
    // std::vector<Color4F> c, c2;

    // Mat4 modelMatrix;

    // Shader * shader;

    Primitives * rect;

};