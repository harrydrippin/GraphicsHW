#include "gg.h"

#include <vector>

class MainScene : public Scene {
public:
    int smallSquareCount, mediumSquareCount, bigSquareCount;

    static MainScene * create();

    void initialized();
    void draw();
    void released();

    void drawScreen();

    void makeRects(const std::vector<Vec3> &vertices, float size, int period, int count);

    void drawRect(const Vec3 &pos, float size, int n);

    std::vector<Vec3> getHilbertCurve(int iter, float length, float offset, const Vec3 &pos);

    static void menuCallback(int value);

private:
    Primitive2D * rect;

    std::vector<Vec3> vertices;
    std::vector<Color4F> preColors;
};