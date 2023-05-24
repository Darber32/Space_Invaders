#pragma once
#include "../headers/Game.h"
#include <stdlib.h>

struct Enemy
{
	bool is_null;
	int* hp, size, count = 0, x_offset, score, money;
	int frame = 0, frame_count, cur_frame_time = 0, max_frame_time;
	double x_pos, y_pos, speed, x_speed;
	int* time, * cur_time, * direction;
	SDL_Rect* enemy_mass, rect = { 0, 0, 0, 0 };
	SDL_Texture* texture = NULL;
};

struct Enemy_Bullet
{
	int size_x, size_y, dmg, count;
	double pos_x, pos_y, speed;
	SDL_Rect bullet_mas[100];
	int active_bullet[100] = { 0 };
	SDL_Rect rect = { 0, 0, 0, 0 };
	SDL_Texture* texture;
	bool is_NULL;
};

void Create_Meteorite(Enemy& meteorite, int count);

void Meteorite_Movement(Enemy& meteorite, Player& player, int dt);

void Meteorite_Draw(Enemy & meteorite, int dt);

void Create_Moving_Ship(Enemy& ship, int count);

void Ship_Movement(Enemy& ship, Player& player, int dt);

void Ship_Draw(Enemy& ship, int dt);

void Create_Explosion_Ship(Enemy& ship, int count);

void Explosion_Ship_Movement(Enemy& ship, Player& player, int dt);

void Explosion_Ship_Draw(Enemy& ship, int dt);

void Create_Changing_Ship(Enemy& ship, int count);

void Changing_Ship_Moving(Enemy& ship, Player& player, int dt);

void Changing_Ship_Draw(Enemy& ship);

void Create_Shooting_Ship(Enemy& ship, int count);

void Shooting_Ship_Movement(Enemy& ship, Player& player, int dt, Enemy_Bullet& bullet);

void Shooting_Ship_Draw(Enemy& ship);

void Enemy_Bullet_Create(Enemy_Bullet& bullet);

void Enemy_Bullet_Movement(Enemy_Bullet& bullet, int dt);

void Enemy_Bullet_Spawn(Enemy_Bullet& bullet, Enemy &ship);

void Enemy_Bullet_Draw(Enemy_Bullet& bullet);