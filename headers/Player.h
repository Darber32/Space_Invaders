#include "../headers/Game.h"
#include "../headers/Background.h"
#pragma once

struct Player
{
    int hp, max_hp, size = 75, money = 0;
    double speed, x_pos = center_x - size / 2, y_pos = win_height - 100;
    SDL_Rect model = { (int)x_pos, (int)y_pos, size, size }, rect = { 0, 0, 0, 0 };
    SDL_Texture* texture;
};

void Create_Light_Ship(Player& ship);

void Create_Heavy_Ship(Player& ship);

void Player_Movement(const Uint8* state, Player& ship, int dt, Planet& planet, const int n, SDL_Rect stars_mass[]);
