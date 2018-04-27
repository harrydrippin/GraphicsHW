#include "gg.h"

#include <vector>

class MainScene : public Scene {
public:
    static MainScene * create();

    void initialized();
    void draw();
    void released();

    void drawRect(const Vec4 &pos, float size);

    void computeHilbertCurve(int iter, float length, float offset, const Vec4 &pos);

private:
    Primitives * rect;

    std::vector<Vec4> vertices;

};