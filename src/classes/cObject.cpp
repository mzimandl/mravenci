#define _USE_MATH_DEFINES // for using cmath constants in windows
#include <cmath>
#include <cstdlib>

#include "cObject.h"
#include "cTexture.h"

#define DEG_TO_RAD M_PI/180



bool inRange(Object* obj1, Object* obj2, int d) {
    int dx = obj1->pos.x - obj2->pos.x;
    int dy = obj1->pos.y - obj2->pos.y;

    if (abs(dx) < d and abs(dy) < d) if (dx*dx + dy*dy < d*d)
        return true;

    return false;
}

Object::Object(Texture* t) {
    texture = t;
}

void Object::move(float dt) {
    pos.x += v*cos(a*DEG_TO_RAD)*dt;
    pos.y += v*sin(a*DEG_TO_RAD)*dt;
}

void Object::setValues(float ix, float iy, float iv, float ia) {
    pos.x = ix;
    pos.y = iy;
    v = iv;
    a = ia;
}

void Object::render(float scale) {
    texture->render((int)round(pos.x), (int)round(pos.y), a, scale);
}

void Object::setTexture(Texture* newTexture) {
    texture = newTexture;
}
