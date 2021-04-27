#include "../math.h"

#include "cTexture.h"
#include "cAnt.h"



Ant::Ant(Texture* t) :
Object(t), alive(false), moving(true), type(ANT_TYPE_EMPTY), follow(ANT_TYPE_EMPTY)
{}

void Ant::randomTurn(float maxA) {
    a += (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * maxA - maxA/2;
}

void Ant::deflect(float x, float y, int dangerDist, int criticalDist) {
    float dx = x - pos.x;
    float dy = y - pos.y;
    if (abs(dx) < dangerDist and abs(dy) < dangerDist) {
        float dist2 = dx*dx + dy*dy;
        if (dist2 < dangerDist*dangerDist) {
            float idist = iqsqrt(dist2);

            // cursor position and ant velocity angle
            float dA = calculateAngleI(dx, dy, idist) - a;
            if (dA > 180) dA -= 360;
            else if (dA < -180) dA += 360;

            float diff = (180 - abs(dA))*std::min((float)1.0, criticalDist*idist);
            if (dA > 0) a -= diff;
            else if (dA < 0) a += diff;
        }
    }
}

void Ant::wallCollision(int width, int height) {
    if (pos.x < 0 or pos.x > width-1) {
        pos.x = pos.x < 0 ? -pos.x : 2*(width-1)-pos.x ;
        a = 180 - a;
    }

    if (pos.y < 0 or pos.y >= height) {
        pos.y = pos.y < 0 ? -pos.y : 2*(height-1)-pos.y;
        a = 360 - a;
    }
}
