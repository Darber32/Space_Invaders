#pragma once
#include "../headers/Game.h"

struct Boss
{
	SDL_Texture* texture = NULL;
	SDL_Rect model = { 0, 0, 0, 0 };
	SDL_Rect rect = { 0, 0, 0 ,0 };
	int size_x, size_y;
	double pos_x, pos_y;
	SDL_Rect weak_point_left, weak_point_right, weak_point_center;
	int hp_left, hp_right, hp_center, offset, offset_rect;
	bool second_phase;
	SDL_Rect left, right, center;
	double speed_x, speed_y;
	SDL_Texture* laser_texture = NULL;
	SDL_Rect laser_rect, laser_model_up, laser_model_down;
	int cooldown, laser_time, warning_time;
	SDL_Texture* warning_texture = NULL;
	SDL_Rect warning_rect, warning_up, warning_down;
	int collision_up, collision_down;
};

void Create_Boss(Boss& boss);

void Boss_Movement(Boss& boss, int dt);

void Boss_Draw(Boss& boss);