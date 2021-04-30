#include "../math.h"

#include "cTexture.h"
#include "cPheromones.h"
#include "cAnt.h"



Ant::Ant(Texture* t) :
Object(t), alive(false), moving(true), type(ANT_TYPE_EMPTY), follow(ANT_TYPE_EMPTY)
{}

void Ant::randomTurn(float maxA) {
    angle += (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * maxA - maxA/2;
}

void Ant::deflect(float x, float y, int dangerDist, int criticalDist) {
    float dx = x - pos.x;
    float dy = y - pos.y;
    if (abs(dx) < dangerDist and abs(dy) < dangerDist) {
        float dist2 = dx*dx + dy*dy;
        if (dist2 < dangerDist*dangerDist) {
            float idist = iqsqrt(dist2);

            // cursor position and ant velocity angle
            float dA = calculateAngleI(dx, dy, idist) - angle;
            if (dA > 180) dA -= 360;
            else if (dA < -180) dA += 360;

            float diff = (180 - abs(dA))*std::min((float)1.0, criticalDist*idist);
            if (dA > 0) angle -= diff;
            else if (dA < 0) angle += diff;
        }
    }
}

void Ant::checkWallCollision(int width, int height, BorderMode borderMode) {
    switch (borderMode) {
        case BORDER_THROUGH:
            if (pos.x < 0) pos.x += width-1; else if (pos.x > width-1) pos.x -= width-1;
            if (pos.y < 0) pos.y += height-1; else if (pos.y > height-1) pos.y -= height-1;
            break;

        case BORDER_BOUNCE:
            if (pos.x < 0 or pos.x > width-1) {
                pos.x = pos.x < 0 ? -pos.x : 2*(width-1)-pos.x;
                angle = 180 - angle;
            }

            if (pos.y < 0 or pos.y > height-1) {
                pos.y = pos.y < 0 ? -pos.y : 2*(height-1)-pos.y;
                angle = 360 - angle;
            }
            break;
        
        case BORDER_KILL:
            if (pos.x < 0 or pos.x > width-1 or pos.y < 0 or pos.y > height-1) alive = false;
            break;
    }
}

void Ant::followPhCount(Pheromones* pheromones, int area, Uint8 maxA, float strength, BorderMode borderMode) {
    int x = (int)round(pos.x);
    int y = (int)round(pos.y);

    const int area2 = area*area;
    const float arc = 2*(float)maxA/3;
    int count1 = 0, count2 = 0, count3 = 0;

    float* p = pheromones->value[follow];

    for (int j = y-area; j <= y+area; j++) {
        for (int i = x-area; i <= x+area; i++) {

            int ii = i, jj = j;
            switch (borderMode) {
                case BORDER_THROUGH:
                    if (i < 0) ii += pheromones->width-1; else if (i > pheromones->width-1) ii -= pheromones->width-1;
                    if (j < 0) jj += pheromones->height-1; else if (j > pheromones->height-1) jj -= pheromones->height-1;
                    break;
                
                case BORDER_BOUNCE:
                case BORDER_KILL:
                    if (i < 0 or i > pheromones->width-1 or j < 0 or j > pheromones->height-1) continue;
                    break;
            }

            if (p[ii + jj*pheromones->width] > 0) {
                float dx = i - pos.x;
                float dy = j - pos.y;

                if (dx != 0 or dy != 0) {
                    float dist2 = dx*dx + dy*dy;
                    if (dist2 < area2) {
                        float dA = calculateAngleI(dx, dy, iqsqrt(dist2)) - angle;

                        if (dA > 180) dA -= 360;
                        else if (dA < -180) dA += 360;

                        if (abs(dA) < maxA) {
                            if (dA < -arc/2) count1++;
                            else if (dA > arc/2) count3++;
                            else count2++;
                        }
                    }
                }
            }
        }
    }

    if (count1 or count2 or count3) {
        if (count1 > count2 and count1 > count3) angle -= strength*arc;
        else if (count3 > count1 and count3 > count2) angle += strength*arc;
        else if (count1 == count2 < count3) angle -= strength*arc/2;
        else if (count1 < count2 == count3) angle += strength*arc/2;
    }
}

void Ant::followPhAverage(Pheromones* pheromones, int area, Uint8 maxA, float strength, BorderMode borderMode) {
    int x = (int)round(pos.x);
    int y = (int)round(pos.y);

    const int area2 = area*area;

    float diffA = 0;
    float total = 0;

    float* p = pheromones->value[follow];

   for (int j = y-area; j <= y+area; j++) {
        for (int i = x-area; i <= x+area; i++) {

            int ii = i, jj = j;
            switch (borderMode) {
                case BORDER_THROUGH:
                    if (i < 0) ii += pheromones->width-1; else if (i > pheromones->width-1) ii -= pheromones->width-1;
                    if (j < 0) jj += pheromones->height-1; else if (j > pheromones->height-1) jj -= pheromones->height-1;
                    break;
                
                case BORDER_BOUNCE:
                case BORDER_KILL:
                    if (i < 0 or i > pheromones->width-1 or j < 0 or j > pheromones->height-1) continue;
                    break;
            }

            const float intensity = p[ii + jj*pheromones->width];
            if (intensity > 0) {
                float dx = i - pos.x;
                float dy = j - pos.y;

                if (dx != 0 or dy != 0) {
                    float dist2 = dx*dx + dy*dy;
                    if (dist2 < area2) {
                        float dA = calculateAngleI(dx, dy, iqsqrt(dist2)) - angle;

                        if (dA > 180) dA -= 360;
                        else if (dA < -180) dA += 360;

                        if (abs(dA) < maxA) {
                            diffA += dA*intensity;
                            total += intensity;
                        }
                    }
                }
            }
        }
    }

    if (total > 0) angle += strength * diffA / total;
}

void Ant::producePh(Pheromones* pheromones, float rate) {
    int x = (int)round(pos.x);
    int y = (int)round(pos.y);
    if (x >= 0 and x < pheromones->width and y >= 0 and y < pheromones->height) {
        float& p = pheromones->value[type][x + y*pheromones->width];
        p = std::min(p + rate, (float)1);
    }
}
