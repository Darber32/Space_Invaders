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
};

void Create_Boss(Boss& boss);

void Boss_Movement(Boss& boss);

void Boss_Draw(Boss& boss);