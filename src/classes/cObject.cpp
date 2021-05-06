#define _USE_MATH_DEFINES // for using cmath constants in windows
#include <cmath>
#include <cstdlib>

#include "cObject.hpp"
#include "cTexture.hpp"

#define DEG_TO_RAD M_PI/180



StaticObject::StaticObject(Texture* t) :
texture(t), angle(0), radius(0)
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

bool StaticObject::inRange(StaticObject &obj) {
    int dx = pos.x - obj.pos.x;
    int dy = pos.y - obj.pos.y;

    if (abs(dx) < radius and abs(dy) < radius) if (dx*dx + dy*dy < radius*radius)
        return true;

    return false;
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
