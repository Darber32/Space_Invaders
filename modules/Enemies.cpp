#include "../headers/Game.h"
#include "../headers/Player.h"
#include <stdlib.h>
#include <stdio.h>

struct Enemy
{
	bool is_null;
	int * hp, size, count = 0, x_offset, score, money;
	int frame = 0, frame_count, cur_frame_time = 0, max_frame_time;
	double x_pos, y_pos, speed, x_speed;
	int* time, *cur_time, *direction;
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

void Enemy_Animation(Enemy & enemy, int dt)
{
	enemy.cur_frame_time += dt;
	if (enemy.cur_frame_time > enemy.max_frame_time)
	{
		enemy.cur_frame_time -= enemy.max_frame_time;
		enemy.frame = (enemy.frame + 1) % enemy.frame_count;
		enemy.rect.x = enemy.rect.w * enemy.frame;
	}
	for (int i = 0; i < enemy.count; i++)
		SDL_RenderCopy(ren, enemy.texture, &enemy.rect, &enemy.enemy_mass[i]);
}

void Create_Meteorite(Enemy &meteorite, int count)
{
	meteorite.is_null = false;
	meteorite.texture = Load_Texture("meteor_sprite.png", &meteorite.rect);
	meteorite.rect.w = meteorite.rect.h;
	meteorite.size = 75;
	meteorite.score = 50;
	meteorite.money = 10;
	meteorite.count = count;
	meteorite.hp = (int*)malloc(sizeof(int) * meteorite.count);
	meteorite.enemy_mass = (SDL_Rect*)malloc(sizeof(SDL_Rect) * meteorite.count);
	meteorite.frame_count = 5;
	meteorite.max_frame_time = 150;
	meteorite.x_offset = 0;
	meteorite.speed = 60;
	meteorite.x_speed = 0;
	meteorite.y_pos = 0;
	meteorite.time = nullptr;
	meteorite.cur_time = nullptr;
	meteorite.direction = nullptr;
	for (int i = 0; i < meteorite.count; i++)
	{
		meteorite.hp[i] = 9;
		meteorite.x_pos = rand() % (win_width - meteorite.size + 1);
		meteorite.y_pos -= rand() % 100 + 100;
		meteorite.enemy_mass[i] = { (int) meteorite.x_pos, (int) meteorite.y_pos, meteorite.size, meteorite.size };
		
	}
}

void Meteorite_Movement(Enemy& meteorite, Player& player, int dt)
{
	
	
	int c = 0;
	for (int i = 0; i < meteorite.count; i++)
		if (meteorite.enemy_mass[i].y > win_height)
			c++;
	if (c == meteorite.count and not meteorite.is_null)
	{
		//meteorite.count = 0;
		meteorite.is_null = true;
		free(meteorite.enemy_mass);
		free(meteorite.hp);
	}
	if (not meteorite.is_null)
	{
		for (int i = 0; i < meteorite.count; i++)
		{
			if (meteorite.enemy_mass[i].y <= win_height)
			{
				meteorite.enemy_mass[i].y += meteorite.speed * dt / 1000;
				if (meteorite.enemy_mass[i].y > win_height)
				{
					player.hp--;
					meteorite.hp[i] = 0;
				}
			}
			if (meteorite.hp[i] <= 0)
				meteorite.enemy_mass[i].y = win_height + 1;
		}
	}
}

void Meteorite_Draw(Enemy & meteorite, int dt)
{
	if (not meteorite.is_null)
			Enemy_Animation(meteorite, dt);
	else if (meteorite.texture != NULL)
	{
		SDL_DestroyTexture(meteorite.texture);
		meteorite.texture = NULL;
	}
	
}

void Create_Moving_Ship(Enemy& ship, int count)
{
	ship.is_null = false;
	ship.size = 80;
	ship.score = 100;
	ship.money = 20;
	ship.count = count;
	ship.enemy_mass = (SDL_Rect*)malloc(sizeof(SDL_Rect) * ship.count);
	ship.hp = (int*)malloc(sizeof(int) * ship.count);
	ship.frame_count = 0;
	ship.max_frame_time = 0;
	ship.x_offset = 100;
	ship.speed = 80;
	ship.x_speed = 80;
	ship.y_pos = 0;		
	ship.time = nullptr;
	ship.cur_time = nullptr;
	ship.direction = nullptr;
	for (int i = 0; i < count; i++)
	{
		ship.hp[i] = 5;
		ship.x_pos = rand() % (win_width - ship.size - ship.x_offset - ship.x_offset + 1) + ship.x_offset;
		ship.y_pos -= rand() % 100 + 100;
		ship.enemy_mass[i] = { (int)ship.x_pos, (int)ship.y_pos, ship.size, ship.size };
	}
}

void Ship_Movement(Enemy& ship, Player& player, int dt)
{
	
	int c = 0;
	for (int i = 0; i < ship.count; i++)
		if (ship.enemy_mass[i].y > win_height)
			c++;
	if (c == ship.count and not ship.is_null)
	{
		//ship.count = 0;
		ship.is_null = true;
		free(ship.enemy_mass);
		free(ship.hp);
	}
	if (not ship.is_null)
	{
		ship.texture = Load_Texture("enemy_ship_1.jpg", &ship.rect);
		if (ship.enemy_mass[ship.count - 1].x > ship.x_pos + ship.x_offset)
		{
			ship.x_speed *= -1;
			double d = ship.enemy_mass[ship.count - 1].x - (ship.x_pos + ship.x_offset);
			for (int i = 0; i < ship.count; i++)
				ship.enemy_mass[i].x -= d;
		}
		if (ship.enemy_mass[ship.count - 1].x <= ship.x_pos - ship.x_offset)
		{
			ship.x_speed *= -1;
			double d = ship.enemy_mass[ship.count - 1].x - (ship.x_pos - ship.x_offset);
			for (int i = 0; i < ship.count; i++)
				ship.enemy_mass[i].x -= d;
		}
		for (int i = 0; i < ship.count; i++)
		{
			ship.enemy_mass[i].x += ship.x_speed * dt / 1000;
			if (ship.enemy_mass[i].y <= win_height)
			{
				ship.enemy_mass[i].y += ship.speed * dt / 1000;
				if (ship.enemy_mass[i].y > win_height)
				{
					player.hp--;
					ship.hp[i] = 0;
				}
			}
			if (ship.hp[i] <= 0)
			{
				ship.enemy_mass[i].y = win_height + 1;
			}
		}
	}
}

void Ship_Draw(Enemy& ship, int dt)
{
	if (not ship.is_null)
	{
		for (int i = 0; i < ship.count; i++)
			SDL_RenderCopy(ren, ship.texture, NULL, &ship.enemy_mass[i]);
		SDL_DestroyTexture(ship.texture);
	}
		
}

void Create_Explosion_Ship(Enemy& ship, int count)
{
	ship.is_null = false;
	ship.texture = Load_Texture("explosion_ship.jpg", &ship.rect);
	ship.size = 80;
	ship.score = 150;
	ship.money = 15;
	ship.count = count;
	ship.hp = (int*)malloc(sizeof(int) * ship.count);
	ship.enemy_mass = (SDL_Rect*)malloc(sizeof(SDL_Rect) * ship.count);
	ship.frame_count = 0;
	ship.max_frame_time = 0;
	ship.x_offset = 0;
	ship.speed = 150;
	ship.x_speed = 0;
	ship.y_pos = -50;
	ship.time = nullptr;
	ship.cur_time = nullptr;
	ship.direction = nullptr;
	for (int i = 0; i < ship.count; i++)
	{
		ship.hp[i] = 4;
		ship.x_pos = rand() % (win_width - ship.size + 1);
		ship.y_pos -= rand() % 200 + 100;
		ship.enemy_mass[i] = { (int)ship.x_pos, (int)ship.y_pos, ship.size, ship.size };
	}
}

void Explosion_Ship_Movement(Enemy& ship, Player& player, int dt)
{
	int c = 0;
	for (int i = 0; i < ship.count; i++)
		if (ship.enemy_mass[i].y > win_height)
			c++;
	if (c == ship.count and not ship.is_null)
	{
		//ship.count = 0;
		ship.is_null = true;
		free(ship.enemy_mass);
		free(ship.hp);
	}
	if (not ship.is_null)
	{
		for (int i = 0; i < ship.count; i++)
		{
			if (ship.enemy_mass[i].y <= win_height)
			{
				ship.enemy_mass[i].y += ship.speed * dt / 1000;
				if (ship.enemy_mass[i].y > win_height)
				{
					player.hp--;
					ship.hp[i] = 0;
				}
			}
			if (ship.hp[i] <= 0)
				ship.enemy_mass[i].y = win_height + 1;
		}
	}
}

void Explosion_Ship_Draw(Enemy& ship, int dt)
{
	if (not ship.is_null)
		for (int i = 0; i < ship.count; i++)
			SDL_RenderCopy(ren, ship.texture, NULL, &ship.enemy_mass[i]);
	else if (ship.texture != NULL)
	{
		SDL_DestroyTexture(ship.texture);
		ship.texture = NULL;
	}
}

void Create_Changing_Ship(Enemy& ship, int count)
{
	ship.is_null = false;
	ship.texture = Load_Texture("enemy_ship_2.png", &ship.rect);
	ship.size = 80;
	ship.score = 200;
	ship.money = 20;
	ship.count = count;
	ship.hp = (int*)malloc(sizeof(int) * ship.count);
	ship.enemy_mass = (SDL_Rect*)malloc(sizeof(SDL_Rect) * ship.count);
	ship.frame_count = 0;
	ship.x_offset = 0;
	ship.speed = 250;
	ship.x_speed = 150;
	ship.y_pos = -ship.size;
	ship.time = (int*)malloc(sizeof(int) * ship.count);
	ship.cur_time = (int*)malloc(sizeof(int) * ship.count);;
	ship.direction = (int*)malloc(sizeof(int) * ship.count);;
	for (int i = 0; i < ship.count; i++)
	{
		ship.hp[i] = 6;
		ship.x_pos = rand() % (win_width - ship.size + 1);
		ship.y_pos -= rand() % 150 + 100;
		ship.enemy_mass[i] = { (int)ship.x_pos, (int)ship.y_pos, ship.size, ship.size };
		ship.time[i] = rand() % 5001 + 5000;
		ship.cur_time[i] = 0;
		ship.direction[i] = 1;
	}
}

void Changing_Ship_Moving(Enemy& ship, Player & player, int dt)
{
	int c = 0;
	for (int i = 0; i < ship.count; i++)
		if (ship.enemy_mass[i].y > win_height)
			c++;
	if (c == ship.count and not ship.is_null)
	{
		ship.is_null = true;
		free(ship.enemy_mass);
		free(ship.hp);
		free(ship.time);
		free(ship.cur_time);
		free(ship.direction);
	}
	if (not ship.is_null)
	{
		for (int i = 0; i < ship.count; i++)
		{
			if (ship.enemy_mass[i].y < 20)
				ship.enemy_mass[i].y += ship.speed * dt / 1000;
			else if (ship.cur_time[i] < ship.time[i])
			{
				ship.cur_time[i] += dt;
				ship.enemy_mass[i].x += ship.x_speed * dt * ship.direction[i] / 1000;
				if (ship.enemy_mass[i].x + ship.size >= win_width or ship.enemy_mass[i].x <= 0)
					ship.direction[i] *= -1;
			}
			else if (ship.enemy_mass[i].y <= win_height)
			{
				ship.enemy_mass[i].y += ship.speed * dt / 1000;
				if (ship.enemy_mass[i].y > win_height)
				{
					player.hp--;
					ship.hp[i] = 0;
				}
			}
			if (ship.hp[i] <= 0)
				ship.enemy_mass[i].y = win_height + 1;
		}
	}
}

void Changing_Ship_Draw(Enemy& ship)
{
	if (not ship.is_null)
		for (int i = 0; i < ship.count; i++)
			SDL_RenderCopy(ren, ship.texture, NULL, &ship.enemy_mass[i]);
	else if (ship.texture != NULL)
	{
		SDL_DestroyTexture(ship.texture);
		ship.texture = NULL;
	}
}

void Enemy_Bullet_Create(Enemy_Bullet& bullet)
{
	bullet.texture = Load_Texture("enemy_bullet.png", &bullet.rect);
	bullet.size_x = 20;
	bullet.size_y = 60;
	bullet.dmg = 1;
	bullet.speed = 250;
	bullet.count = 100;
	for (int i = 0; i < 100; i++)
		bullet.bullet_mas[i] = { 0, win_height + 1, 0, 0 };
	bullet.is_NULL = false;
}

void Enemy_Bullet_Movement(Enemy_Bullet& bullet, int dt)
{
	int c = 0;
	for (int i = 0; i < bullet.count; i++) {
		if (bullet.active_bullet[i] == 0)
			c++;
	}
	if (c == bullet.count)
		bullet.is_NULL = true;
	else
		bullet.is_NULL = false;
	if (not bullet.is_NULL)
	{
		for (int i = 0; i < bullet.count; i++)
		{
			if (bullet.active_bullet[i] == 0 and bullet.bullet_mas[i].y != win_width + 1)
				bullet.bullet_mas[i] = { 0, win_width + 1, 0, 0 };
			if (bullet.active_bullet[i] == 1)
			{
				bullet.bullet_mas[i].y += bullet.speed * dt / 1000;
				if (bullet.bullet_mas[i].y > win_height)
				{
					bullet.bullet_mas[i] = { 0, win_width + 1, 0, 0 };
					bullet.active_bullet[i] = 0;
				}
			}
		}
	}
}

void Enemy_Bullet_Spawn(Enemy_Bullet& bullet, Enemy &ship)
{
	for (int i = 0; i < bullet.count; i++)
	{
		if (bullet.bullet_mas[i].y > win_height and bullet.active_bullet[i] == 1)
		{
			bullet.pos_x = ship.enemy_mass[i].x + ship.size / 2 - bullet.size_x;
			bullet.pos_y = ship.enemy_mass[i].y + ship.size;
			bullet.bullet_mas[i] = { (int)bullet.pos_x, (int)bullet.pos_y, bullet.size_x, bullet.size_y };
		}
	}
}

void Enemy_Bullet_Draw(Enemy_Bullet &bullet)
{
	for (int i = 0; i < bullet.count; i++)
		if (bullet.active_bullet[i] == 1)
			SDL_RenderCopy(ren, bullet.texture, NULL, &bullet.bullet_mas[i]);
}

void Create_Shooting_Ship(Enemy& ship, int count)
{
	ship.is_null = false;
	ship.texture = Load_Texture("shooting_ship.jpg", &ship.rect);
	ship.size = 80;
	ship.score = 250;
	ship.money = 25;
	ship.count = count;
	ship.hp = (int*)malloc(sizeof(int) * ship.count);
	ship.enemy_mass = (SDL_Rect*)malloc(sizeof(SDL_Rect) * ship.count);
	ship.frame_count = 0;
	ship.x_offset = 0;
	ship.speed = 90;
	ship.x_speed = 0;
	ship.y_pos = -ship.size;
	ship.time = NULL;
	ship.cur_time = (int*)malloc(sizeof(int) * ship.count);
	ship.direction = nullptr;
	for (int i = 0; i < ship.count; i++)
	{
		ship.hp[i] = 7;
		ship.x_pos = rand() % (win_width - ship.size + 1);
		ship.y_pos -= 150;
		ship.enemy_mass[i] = { (int)ship.x_pos, (int)ship.y_pos, ship.size, ship.size };
		ship.cur_time[i] = 0;
	}
}

void Shooting_Ship_Movement(Enemy& ship, Player& player, int dt, Enemy_Bullet & bullet)
{
	int c = 0;
	for (int i = 0; i < ship.count; i++)
		if (ship.enemy_mass[i].y > win_height)
			c++;
	if (c == ship.count and not ship.is_null)
	{
		ship.is_null = true;
		free(ship.hp);
		free(ship.enemy_mass);
		free(ship.cur_time);
	}
	if (not ship.is_null)
	{
		for (int i = 0; i < ship.count; i++)
		{
			if (ship.enemy_mass[i].y <= win_height)
			{
				ship.enemy_mass[i].y += ship.speed * dt / 1000;
				if (ship.enemy_mass[i].y > win_height)
				{
					player.hp--;
					ship.hp[i] = 0;
				}
				if (ship.enemy_mass[i].y > 0 and ship.enemy_mass[i].y < win_height)
				{
					ship.cur_time[i] += dt;
					if (ship.cur_time[i] >= 3000 and bullet.active_bullet[i] == 0)
					{
						ship.cur_time[i] -= 3000;
						bullet.active_bullet[i] = 1;
						Enemy_Bullet_Spawn(bullet, ship);

					}
				}
			}
			if (ship.hp[i] <= 0)
				ship.enemy_mass[i].y = win_height + 1;
		}
	}
}

void Shooting_Ship_Draw(Enemy& ship)
{
	if (not ship.is_null)
		for (int i = 0; i < ship.count; i++)
			SDL_RenderCopy(ren, ship.texture, NULL, &ship.enemy_mass[i]);
	else if (ship.texture != NULL)
	{
		SDL_DestroyTexture(ship.texture);
		ship.texture = NULL;
	}
}
