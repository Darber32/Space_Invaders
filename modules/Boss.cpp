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
};

void Create_Boss(Boss& boss)
{
	boss.speed_x = 200;
	boss.speed_y = 50;
	boss.second_phase = false;
	boss.texture = Load_Texture("boss.png", &boss.rect);
	boss.offset_rect = boss.rect.w / 3;
	boss.size_x = win_width / 6 * 4;
	boss.size_y = win_height / 4;
	boss.pos_x = win_width / 6;
	boss.pos_y = 0;
	boss.model = { (int)boss.pos_x, (int)boss.pos_y, boss.size_x, boss.size_y };
	boss.offset = boss.model.w / 3;
	boss.hp_left =  15;
	boss.hp_right = 15;
	boss.hp_center = 30;
	boss.weak_point_left = { (int)boss.pos_x, (int)boss.pos_y + 30 ,boss.offset, boss.model.h / 3 * 2 - 30 };
	boss.weak_point_right = { (int)boss.pos_x + boss.size_x - boss.offset ,  (int)boss.pos_y + 30, boss.offset, boss.model.h / 3 * 2 - 30 };
	boss.weak_point_center = { (int)boss.pos_x + boss.offset, (int)boss.pos_y + 30, boss.size_x - 2 * boss.offset, boss.size_y };
	boss.left = { (int)boss.pos_x, (int)boss.pos_y + 15 ,boss.offset, 10 };
	boss.right = { (int)boss.pos_x + boss.size_x - boss.offset ,  (int)boss.pos_y + 15, boss.offset, 10 };
	boss.center = { (int)boss.pos_x + boss.offset, (int)boss.pos_y + 15, boss.size_x - 2 * boss.offset, 10 };
}

void Boss_Movement(Boss& boss, int dt)
{
	if (boss.hp_left <= 0 and boss.weak_point_left.y <= win_height)
	{
		boss.weak_point_left = { 0, win_height + 1, 0, 0 };
		boss.left = { 0, win_height + 1, 0, 0 };
	}
	if (boss.hp_right <= 0 and boss.weak_point_right.y <= win_height)
	{
		boss.weak_point_right = { 0, win_height + 1, 0, 0 };
		boss.right = { 0, win_height + 1, 0, 0 };
	}
	if (boss.hp_left <= 0 and boss.hp_right <= 0 and not boss.second_phase)
		boss.second_phase = true;
	if (boss.hp_center <= 0 and boss.weak_point_center.y <= win_height)
	{
		boss.weak_point_center = { 0, win_height + 1, 0, 0 };
		boss.model = { 0, win_height + 1, 0, 0 };
		boss.center = { 0, win_height + 1, 0, 0 };
	}
	
		boss.model.x += boss.speed_x * dt / 1000;
		boss.left.x += boss.speed_x * dt / 1000;
		boss.right.x += boss.speed_x * dt / 1000;
		boss.center.x += boss.speed_x * dt / 1000;
		boss.weak_point_left.x += boss.speed_x * dt / 1000;
		boss.weak_point_right.x += boss.speed_x * dt / 1000;
		boss.weak_point_center.x += boss.speed_x * dt / 1000;

	if (boss.hp_left <= 0 or boss.hp_right <= 0 and not boss.second_phase)
	{
		boss.model.y += boss.speed_y * dt / 1000;
		boss.left.y += boss.speed_y * dt / 1000;
		boss.right.y += boss.speed_y * dt / 1000;
		boss.center.y += boss.speed_y * dt / 1000;
		boss.weak_point_left.y += boss.speed_y * dt / 1000;
		boss.weak_point_right.y += boss.speed_y * dt / 1000;
		boss.weak_point_center.y += boss.speed_y * dt / 1000;
	}

	if (boss.model.x < 0 or boss.model.x + boss.model.w > win_width)
		boss.speed_x *= -1;
	if (boss.model.y < 0 or boss.model.y + boss.model.h > win_height / 2)
		boss.speed_y *= -1;
}

void Boss_Draw(Boss& boss)
{
	SDL_RenderCopy(ren, boss.texture, &boss.rect, &boss.model);
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 130);
	SDL_RenderFillRect(ren, &boss.left);
	SDL_RenderFillRect(ren, &boss.right);
	if (boss.second_phase)
		SDL_RenderFillRect(ren, &boss.center);
}
