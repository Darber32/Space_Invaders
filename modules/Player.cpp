#define _CRT_SECURE_NO_WARNINGS
#include "../headers/Game.h"
#include "../headers/Background.h"


struct Player
{
    int hp, max_hp, size = 75, money = 0;
    double speed, x_pos = center_x - size / 2, y_pos = win_height - 100;
    SDL_Rect model = { (int)x_pos, (int)y_pos, size, size }, rect = { 0, 0, 0, 0 };
    SDL_Texture* texture;
};

void Create_Light_Ship(Player& ship)
{ 
    ship.max_hp = 5;
    ship.hp = ship.max_hp;
    ship.speed = 250;
    ship.texture = Load_Texture("space_ship.png", &ship.rect);
}

void Create_Heavy_Ship(Player& ship)
{
    ship.max_hp = 8;
    ship.hp = ship.max_hp;
    ship.speed = 175;
    ship.texture = Load_Texture("space_ship_2.png", &ship.rect);
}

void Player_Movement(const Uint8 * state, Player& ship, int dt, Planet & planet, const int n, SDL_Rect stars_mass[])
{
    if (state[SDL_SCANCODE_UP])
    {
        if (ship.y_pos - ship.speed * dt / 1000 >= 0)
            ship.y_pos -= ship.speed * dt / 1000;
        else
            ship.y_pos = 0;
    }
    if (state[SDL_SCANCODE_DOWN])
        if (ship.y_pos + ship.size + ship.speed * dt / 1000 <= win_height)
            ship.y_pos += ship.speed * dt / 1000;
        else
            ship.y_pos = win_height - ship.size;
    if (state[SDL_SCANCODE_RIGHT] and not state[SDL_SCANCODE_LEFT])
    {
        if (ship.x_pos + ship.size + ship.speed * dt / 1000 <= win_width)
            ship.x_pos += ship.speed * dt / 1000;
        else
            ship.x_pos = win_width-ship.size;
        for (int i = 0; i < n; i++)
        {
            stars_mass[i].x -= ship.speed * 0.75 * dt / 1000;
            if (stars_mass[i].x <= -stars_mass[i].w)
                stars_mass[i].x = win_width - stars_mass[i].w;
        }
        planet.model.x -= ship.speed * planet.speed_x * dt / 1000;
    }
    if (state[SDL_SCANCODE_LEFT] and not state[SDL_SCANCODE_RIGHT])
    {
        if (ship.x_pos - ship.speed * dt / 1000 >= 0)
            ship.x_pos -= ship.speed * dt / 1000;
        else
            ship.x_pos = 0;
        for (int i = 0; i < n; i++)
        {
            stars_mass[i].x += ship.speed * 0.75 * dt / 1000;
            if (stars_mass[i].x >= win_width)
                stars_mass[i].x = 1 - stars_mass[i].w;
        }
        planet.model.x += ship.speed * planet.speed_x * dt / 1000;
    }
}

void Save_Player_Stats(Player ship, const char * filename, int texture)
{
    FILE* save = fopen(filename, "w");
    fprintf_s(save, "%i ", texture);
    fprintf_s(save, "%i ", ship.max_hp);
    fprintf_s(save, "%i ", ship.hp);
    fprintf_s(save, "%i ", ship.money);
    fprintf_s(save, "%lf ", ship.speed);
    fclose(save);
}

void Load_Player_Stats(Player& ship, const char* filename, int &ship_type)
{
    FILE* save = fopen(filename, "r");
    fscanf_s(save, "%i ", &ship_type);
    switch(ship_type)
    {
    case 1:
        ship.texture = Load_Texture("space_ship.png", &ship.rect);
        break;
    case 2:
        ship.texture = Load_Texture("space_ship_2.png", &ship.rect);
        break;
    }
    fscanf_s(save, "%i ", &ship.max_hp);
    fscanf_s(save, "%i ", &ship.hp);
    fscanf_s(save, "%i ", &ship.money);
    fscanf_s(save, "%lf ", &ship.speed);
    fclose(save);
}