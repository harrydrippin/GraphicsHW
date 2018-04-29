#include "gg.h"

#include <vector>

class MainScene : public Scene {
public:
    static MainScene * create();

    void initialized();
    void draw();
    void released();

    void makeRects(float size, int period, int count);

    void drawRect(const Vec4 &pos, float size, int n, float z);

    std::vector<Vec4> getHilbertCurve(int iter, float length, float offset, const Vec4 &pos);

    int random(int min, int max);

private:
    Primitives * rect;

    std::vector<Color4F> preColors;

};