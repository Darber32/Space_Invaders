#pragma once
#include "../headers/Game.h"

const int stars_number = 30;
extern SDL_Rect stars_mass[stars_number];
extern double speed[stars_number];

void Create_Stars(const int n, SDL_Rect stars_mass[], double speed[]);

void Falling_Stars(const int n, SDL_Rect stars_mass[], double speed[], int dt);

struct Planet
{
    double x_pos = 0, y_pos = 0, speed_x = 0, speed_y = 0, angle = 0;
    int size = 0, type;
    SDL_Texture* texture;
    SDL_Rect model;
};

void Create_Planets(Planet& planet);

void Planet_Moving(Planet &planet, int dt);