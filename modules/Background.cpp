#include "../headers/Game.h"
#include <stdlib.h>
#include <stdio.h>

const int stars_number = 30;
SDL_Rect stars_mass[stars_number] = { 0 };
double speed[stars_number] = { 0 };


void Create_Stars(const int n, SDL_Rect stars_mass[], double speed[])
{
    double x_pos, y_pos;
    int stars_size;
    SDL_Rect rect = { 0, 0, 0, 0 };
    SDL_Texture* texture = Load_Texture("star.png", &rect);
    for (int i = 0; i < n; i++)
    {
        x_pos = rand() % (win_width - 10 + 1) + 10;
        y_pos = rand() % (win_height / 4 * 3 + 1) - 20;
        stars_size = rand() % 26 + 10;
        stars_mass[i] = { (int)x_pos , (int)y_pos, stars_size, stars_size };
        speed[i] = rand() % 360 + 150;
        SDL_RenderCopy(ren, texture, NULL, &stars_mass[i]);
    }
    SDL_DestroyTexture(texture);
}
void Falling_Stars(const int n, SDL_Rect stars_mass[], double speed[],  int dt)
{
    SDL_Rect rect = { 0, 0, 0, 0 };
    SDL_Texture* texture = Load_Texture("star.png", &rect);
    for (int i = 0; i < n; i++)
    {
        if (stars_mass[i].y >= win_height)
        {
            stars_mass[i].x = rand() % (win_width - 10) + 10;
            stars_mass[i].y = rand() % (50 + 1) - 20;
            stars_mass[i].w = stars_mass[i].h = rand() % 26 + 10;
            speed[i] = rand() % 360 + 150;
            SDL_RenderCopy(ren, texture, NULL, &stars_mass[i]);
        }
        else
        {
            stars_mass[i].y += speed[i] * dt / 1000;
            //stars.stars_mass[i] = { mass[i].x , mass[i].y, mass[i].w, mass[i].w };
            SDL_RenderCopy(ren, texture, NULL, &stars_mass[i]);
        }
    }
    SDL_DestroyTexture(texture);
}

struct Planet
{
    double x_pos = 0, y_pos = 0, speed_x = 0, speed_y = 0, angle = 0;
    int size = 0, type;
    SDL_Texture* texture;
    SDL_Rect model;
};

void Create_Planets(Planet &planet)
{
    char planet_texture[30] = "planet_.";
    planet.type = rand() % 12 + 1;
    sprintf_s(planet_texture, "planet_%i.png", planet.type);
    SDL_Rect rect = { 0, 0, 0, 0 };
    planet.texture = Load_Texture(planet_texture, &rect);
    planet.size = rand() % 80 + 70;
    planet.x_pos = rand() % (win_width - planet.size + 1);
    planet.y_pos = 0 - (rand() % 300 + 100);
    planet.speed_x = (double) (rand() % 26 + 25) / 100;
    planet.speed_y = rand() % 20 + 60;
    planet.model = { (int)planet.x_pos, (int)planet.y_pos, planet.size, planet.size };
    if (planet.type != 12)
        SDL_RenderCopy(ren, planet.texture, NULL, &planet.model);
    else
        SDL_RenderCopyEx(ren, planet.texture, NULL, &planet.model, planet.angle, NULL, SDL_FLIP_NONE);
}

void Planet_Moving(Planet &planet, int dt)
{
        planet.model.y += planet.speed_y * dt / 1000.0;
        SDL_RenderFillRect(ren, &planet.model);
        if (planet.type != 12)
            SDL_RenderCopy(ren, planet.texture, NULL, &planet.model);
        else
        {
            planet.angle += 0.5;
            SDL_RenderCopyEx(ren, planet.texture, NULL, &planet.model, planet.angle, NULL, SDL_FLIP_NONE);
        }
        if (planet.model.y > win_height)
        {
            SDL_DestroyTexture(planet.texture);
            Create_Planets(planet);
        }
}