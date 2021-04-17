#include "cObject.h"

#include <cmath>
#include <cstdlib>
#include "cTexture.h"



Object::Object(Texture* t, int g = 0) {
    texture = t;
    type = g;
}

void Object::move(float dt) {
    pos.x += v*cos(a*M_PI/180)*dt;
    pos.y += v*sin(a*M_PI/180)*dt;
}

void Object::randomTurn(float maxA) {
    a += (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * maxA - maxA/2;
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
