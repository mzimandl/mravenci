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

Object::Object(Texture* t) :
texture(t)
{ vel.v = 0; vel.a = 0; }

void Object::move(float dt) {
    pos.x += vel.v*cos(vel.a*DEG_TO_RAD)*dt;
    pos.y += vel.v*sin(vel.a*DEG_TO_RAD)*dt;
}

void Object::setPos(float x, float y) {
    pos.x = x;
    pos.y = y;
}

void Object::setVel(float v, float a) {
    vel.v = v;
    vel.a = a;
}

void Object::render(float scale) {
    texture->render((int)round(pos.x), (int)round(pos.y), vel.a, scale);
}

void Object::setTexture(Texture* newTexture) {
    texture = newTexture;
}
