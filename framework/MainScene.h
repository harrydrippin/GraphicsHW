#include "gg.h"

#include <vector>

class MainScene : public Scene {
public:
    static MainScene * create();

    void initialized();
    void update();
    void released();

private:
    std::vector<Vec3> v;
    std::vector<Color3F> c;

};