#pragma once



// quick square root
float qsqrt(const float &n);

// inverse quick square root !!! NO DIVISION !!!
float iqsqrt(const float &n);

float calculateAngle(float dx, float dy, float dist);

float calculateAngleI(float dx, float dy, float idist);

void normalizeAngle(float &a);
