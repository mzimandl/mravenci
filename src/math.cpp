#include "math.h"

#include <cmath>



// inverse quick square root !!! NO DIVISION !!!
float iqsqrt(const float &n) 
{
   static union{int i; float f;} u;
   u.i = 0x5F375A86 - (*(int*)&n >> 1);
   return (int(3) - n * u.f * u.f) * u.f * 0.5f;
}

// quick square root
float qsqrt(const float &n) 
{   
   return n * iqsqrt(n);
}

float calculateAngle(float dx, float dy, float dist) {
    float pointA = 180 * asin(dy/dist) / M_PI;
    if (dx < 0) {
        pointA = 180 - pointA;
    } else if (dx > 0 and dy < 0) {
        pointA = 360 + pointA;
    }
    return pointA;
}

void normalizeAngle(float &a) {
    if (a < 0) {
        a += 360;
    } else if (a > 360) {
        a -= 360;
    }
}
