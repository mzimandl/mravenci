#pragma once

const float STEP_SIZE = 0.1;
const int STEPS_PER_FRAME = 1;

//Screen dimension constants
const int SCREEN_WIDTH = 1680;
const int SCREEN_HEIGHT = 1010;
const int SCREEN_FPS = 30;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

//Cursor deflection
const int CURSOR_DANGER = 500;
const int CURSOR_CRITICAL = 2;

//Ant properties
const float ANT_SCALE_RENDER = 0.25; // size of rendered ant (of original image size)
const int ANT_MAX_POPULATION = 1000; // ant population
const int ANT_SPEED = 25; // minimum speed
const int ANT_SPEED_VARIATION = 5; // random speed increase
const int ANT_RANDOM_TURN = 30; // max angle of ants random turn
const char ANT_CHANCE_TO_MOVE = 80; // 0-100 percent
const int ANT_REVIVE_RATE = 30;

const float PHEROMONE_PRODUCTION_RATE = 0.3; // pheromone amount ant produces every step
const float PHEROMONE_DECAY_RATE = 0.001; // amount of decay of pheromones every step
const int PHEROMONES_DISTANCE = 15; // distance to which ant smells pheromones
const int PHEROMONES_ANGLE = 25; // 0-180 +/- angle in front of ant where he smels pheromones
const float PHEROMONES_FOLLOW_STRENGTH = 0.8; // .0-1.
const int PHEROMONES_FOLLOW_STEP = 1; // follow pheromones every Nth step

const int SOUND_CONTROL_DISTURB_LEVEL = 10;
