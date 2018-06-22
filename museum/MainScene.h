#include "help.h"

#include "SOIL.h"

#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace help;

class MainScene : public help::Scene {
public:
    static MainScene * create() {
        auto ret = new MainScene();
        return ret;
    }

    void start() {
        std::ifstream input("data.sav");

        if (!input.is_open()) {
            std::cerr << "failed to open data file" << std::endl;
            exit(1);
        }

        std::stringstream ss;
        std::string line;

        while (!input.eof()) {
            std::getline(input, line);

            if (line[0] == '#') continue;

            ss.clear();
            ss.str(line);

            std::string path, tag;
            float yangle;
            Vec3 pos, scale, min, max;
            ss >> path >> tag >> yangle >> 
                     scale.x >> scale.y >> scale.z >> 
                     pos.x >> pos.y >> pos.z;

            auto obj = Obj3D::create(path, "./shader/phong_vert.glsl", "./shader/phong_frag.glsl");
            obj->setPosition(pos);
            obj->setRotation(yangle, Vec3(0, 1, 0));
            obj->setScale(scale);
            obj->setTag(tag);
            addChild(obj);
            _objects.push_back(obj);

            ss >> min.x >> min.y >> min.z >> max.x >> max.y >> max.z;
            auto aabb = new AABB(min, max);
            _aabbs.push_back(aabb);
            aabb->setPosition(obj->_position);
        }

        prev = curr = std::chrono::system_clock::now();

        playerAABB = new AABB(Vec3(-1.2, -2.5, -1.2), Vec3(1.2, 2.5, +1.2));

        // glutSetCursor(GLUT_CURSOR_NONE);
    }

    void update() {
        glutWarpPointer(480 / 2, 480 / 2);

        curr = std::chrono::system_clock::now();
        std::chrono::duration<float> elaped_seconds = (curr - prev);

        if (fanSwitch) {
            angle += 400 * elaped_seconds.count();

            for (auto &i : _objects) {
                if (i->getTag() == "fan") i->setRotation(angle, Vec3(0, 1, 0));
            }
        }

        tempDirection = Vec3(0);

        // 충돌 처리를 위해 키를 눌렀을 때 카메라를 즉시 이동시키지 않고 임시 변수에 담아둔 뒤 정규화 해서 일정한 속도로만 움직이게 함.
        // 점프가 없으므로 y축은 계산에 포함시키지 않음.
        if (moveKey[0]) {
            tempDirection.x += Camera::get()->_frontDirection.x * 0.1;
            tempDirection.z += Camera::get()->_frontDirection.z * 0.1;
        }
        if (moveKey[1]) {
            tempDirection.x -= Camera::get()->_frontDirection.x * 0.1;
            tempDirection.z -= Camera::get()->_frontDirection.z * 0.1;
        }
        if (moveKey[2]) {
            // frontDirection의 90도 방향 벡터 vec2(-y, x);
            tempDirection -= Vec3(-Camera::get()->_frontDirection.z, 0, Camera::get()->_frontDirection.x) * 0.1;
        }
        if (moveKey[3]) {
            tempDirection += Vec3(-Camera::get()->_frontDirection.z, 0, Camera::get()->_frontDirection.x) * 0.1;
        }

        // 이동 방향을 정규화 해줌.
        if (tempDirection.x != 0 && tempDirection.y != 0 && tempDirection.z != 0) tempDirection.normalize();
        
        // 카메라 위치에 실제 적용
        Camera::get()->_position.x += tempDirection.x;
        Camera::get()->_position.z += tempDirection.z;

        auto cp = Camera::get()->getPosition();
        // 플레이어 충돌체가 카메라를 따라다니게 함.
        playerAABB->setPosition(Vec3(cp.x, cp.y, cp.z));

        // collision check
        for (auto &aabb : _aabbs) {
            if (playerAABB->isIntersect(aabb)) {
                float angle = atan2(playerAABB->_position.x - aabb->_position.x, playerAABB->_position.z - aabb->_position.z) * 57.2958;
                float frontAngle = aabb->_frontAngle;

                Vec3 n;
                Vec3 p = tempDirection;

                // 충돌했으면 방금 이동한만큼 반대로 빼줌
                Camera::get()->_position.x -= p.x;
                Camera::get()->_position.z -= p.z;

                // 충돌면의 법선벡터 값을 정해줌. 앞과 뒤에 부딪히면 Vec3(0, 0, 1) 왼쪽, 오른쪽에 부딪히면 Vec3(1, 0, 0)
                if ((angle >= -frontAngle && angle < frontAngle) || 
                    (angle < -(180 - frontAngle)) || (angle >= 180 - frontAngle)) n = Vec3(0, 0, 1);
                else n = Vec3(1, 0, 0);
                
                // 입사각과 충돌면의 법선벡터를 이용하여 미끄러짐 벡터를 구함
                Vec3 s = p - n * (p.dot(n));

                // 카메라의 위치에 더해주어 미끄러질 수 있도록 함
                Camera::get()->_position.x += s.x;
                Camera::get()->_position.z += s.z;
            }
        }

        prev = curr;
    }

    void onMouseDown(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            bool escape = false;
            AABB * selected = nullptr;
            int idx = 0;
            // 16까지 0.1 단위로 검사함
            for (float x = 0; x < 16; x += 0.1) {
                for (int i = 0; i < _aabbs.size(); i++) {
                    auto aabb = _aabbs[i];

                    // 현재 보고 있는 위치에서 보고있는 방향으로 x만큼씩 진행하면서 충돌 검사를 함.
                    if (aabb->isIntersect(Camera::get()->_position + Camera::get()->_frontDirection * x)) {
                        selected = aabb;
                        idx = i;
                        escape = true;
                        break;
                    }
                }
                if (escape) break;
            }

            if (selected) {
                if (_objects[idx]->getTag() == "fan") {
                    fanSwitch = !fanSwitch;
                } else if (_objects[idx]->getTag() == "moveable") {
                    _objects[idx]->setPosition(_objects[idx]->getPosition() + Vec3(Camera::get()->_frontDirection.x, 0, Camera::get()->_frontDirection.z));
                    _aabbs[idx]->setPosition(_objects[idx]->getPosition());
                } else {
                    eraseObject(idx);
                }
                // std::cout << "selected: " << _objects[idx]->getTag() << std::endl;
            }
        }
    }

    void draw() {
        playerAABB->debugDraw();
        for (auto &i : _aabbs) i->debugDraw();
    }

    void end() {
        delete playerAABB;
        for (int i = 0; i < _aabbs.size(); i++) delete _aabbs[i];
    }

    void onKeyboardPress(unsigned char keycode, int x, int y) {
        if (keycode == 'w' || keycode == 'W') moveKey[0] = true;
        if (keycode == 's' || keycode == 'S') moveKey[1] = true;
        if (keycode == 'a' || keycode == 'A') moveKey[2] = true;
        if (keycode == 'd' || keycode == 'D') moveKey[3] = true;
        if (keycode == 't' || keycode == 'T') exit(0);
    }

    void onKeyboardRelease(unsigned char keycode, int x, int y) {
        if (keycode == 'w' || keycode == 'W') moveKey[0] = false;
        if (keycode == 's' || keycode == 'S') moveKey[1] = false;
        if (keycode == 'a' || keycode == 'A') moveKey[2] = false;
        if (keycode == 'd' || keycode == 'D') moveKey[3] = false;
    }

    void onMouseMove(int x, int y) {
        int dx = 240 - x, dy = 240 - y;
        float speed = 0.5f;
        if (dx != 0 || dy != 0) {
            if (dx > 0) dx = 1;
            if (dx < 0) dx = -1;
            if (dy > 0) dy = 1;
            if (dy < 0) dy = -1;

            // 카메라가 마우스의 delta값에 따라 움직이게 함...
            Camera::get()->rotate(dx * speed, dy * speed);
        }
    }

    void eraseObject(int index) {
        removeChild(_objects[index]);
        _objects.erase(_objects.begin() + index);
        delete _aabbs[index];
        _aabbs.erase(_aabbs.begin() + index);
    }

public:
    std::vector<Obj3D *> _objects;
    std::vector<AABB *> _aabbs;
    AABB *playerAABB, *sofaAABB;

    float angle = 0.0;
    int cx = 0, cy = 0, px = 0, py = 0;

    Vec3 tempDirection;

    // 0 up, 1 down, 2 left, 3 right
    bool moveKey[4];

    bool fanSwitch = false;
    
    std::chrono::time_point<std::chrono::system_clock> prev, curr;

};