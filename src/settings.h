#pragma once

const float STEP_SIZE = 0.1;
const int STEPS_PER_FRAME = 1;

//Screen dimension constants
const int SCREEN_WIDTH = 1680;
const int SCREEN_HEIGHT = 1010;
const int SCREEN_FPS = 30;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

//Ant properties
const float ANT_SCALE_RENDER = 0.2;
const int ANT_MIN_SPEED = 25;
const int ANT_RANDOM_SPEED = 5;
const int NUMBER_OF_ANTS = 1000;
const int MAX_RANDOM_TURN = 30;
const int CURSOR_DANGER = 500;
const int CURSOR_CRITICAL = 2;
const float FEROMONE_PRODUCTION = 0.3;
const float FEROMONE_DECAY = 0.005;
const int FEROMONES_AREA = 15;
const int FEROMONES_ANGLE = 25;
