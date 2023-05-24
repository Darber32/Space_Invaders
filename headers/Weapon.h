#pragma once
#include "../headers/Player.h"
#include "../headers/Game.h"

struct weapon {
    int size_x, size_y, dmg, cd, cur_time = 0, count;
    double pos_x, pos_y, vx, vy;
    SDL_Rect bullet_mas[50];
    int active_bullet[50] = { 0 };
    SDL_Rect rect = { 0, 0, 0, 0 };
    SDL_Texture* texture;
    bool is_NULL;
};

bool Is_Cooldown_Now(weapon& bullet, int dt);

void bullet_create(weapon& bullet);

void bullet_movement(weapon& bullet, int dt);

void bullet_spawn(weapon& bullet, Player player);

void bullet_draw(weapon bullet);