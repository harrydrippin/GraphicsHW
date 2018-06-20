#include "gg.h"

#include <vector>

class MainScene : public Scene {
public:
    static MainScene * create();

    void initialized();
    void update();
    void draw();
    void released();

    void keyInput(unsigned char key, int x, int y);

private:
    Shader * shader;

    std::vector<Vec3> vertices;
    std::vector<Color4F> colors;

    Mat4 vm, mm;

    float ex = 0, ey = 0, cx = 0, cy = 0, fovy = 120;

};