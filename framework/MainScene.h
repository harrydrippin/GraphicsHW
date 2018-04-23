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
    std::vector<Vec4> v;
    std::vector<Color4F> c;

    Mat4 modelMatrix;

    Shader * shader;

};