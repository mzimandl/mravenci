#define _USE_MATH_DEFINES // for using cmath constants in windows
#include <cmath>
#include <cstdlib>

#include "cObject.hpp"
#include "cTexture.hpp"

#define DEG_TO_RAD M_PI/180



bool inRange(StaticObject* obj1, StaticObject* obj2, int d) {
    int dx = obj1->pos.x - obj2->pos.x;
    int dy = obj1->pos.y - obj2->pos.y;

    if (abs(dx) < d and abs(dy) < d) if (dx*dx + dy*dy < d*d)
        return true;

    return false;
}

StaticObject::StaticObject(Texture* t) :
texture(t), angle(0)
{}

void StaticObject::setPos(float x, float y) {
    pos.x = x;
    pos.y = y;
}

void StaticObject::setAngle(float a) {
    angle = a;
}

void StaticObject::render(float scale) {
    texture->render((int)round(pos.x), (int)round(pos.y), angle, scale);
}

void StaticObject::setTexture(Texture* newTexture) {
    texture = newTexture;
}

Object::Object(Texture* t) :
StaticObject(t), speed(0)
{}

void Object::move(float dt) {
    pos.x += speed*cos(angle*DEG_TO_RAD)*dt;
    pos.y += speed*sin(angle*DEG_TO_RAD)*dt;
}

void Object::setSpeed(float s) {
    speed = s;
}
