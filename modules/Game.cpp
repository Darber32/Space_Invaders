#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <limits.h>
#pragma comment (lib, "winmm.lib")
#include <Windows.h>
#include "../headers/Game.h"
#include "../headers/Player.h"
#include "../headers/Background.h"
#include "../headers/Enemies.h"
#include "../headers/Weapon.h"
#include "../headers/Boss.h"

//PlaySound(L"music.wav", NULL, SND_ASYNC | SND_LOOP);

int win_width = 1000;
int win_height = 800;
int center_x = win_width / 2;
int center_y = win_height / 2;

SDL_Window* win;
SDL_Renderer* ren;

void deinit() {
    if (ren != NULL) SDL_DestroyRenderer(ren);
    if (win != NULL) SDL_DestroyWindow(win);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void throwFatalError(const char* text) {
    printf("%s\n", text);
    system("pause");
    deinit();
    SDL_Quit();
    exit(1);
}

void initSDL() {
    int status = SDL_Init(SDL_INIT_VIDEO);

    if (status != 0) throwFatalError("Error on initializing SDL");

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_JPG) == 0) throwFatalError("Error on initializing SDL_image");

    if (TTF_Init() != 0)
    {
        printf("%s\n", TTF_GetError());
        throwFatalError("Error on initializing SDL_ttf");
    }

    win = SDL_CreateWindow("Main window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        win_width, win_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (win == NULL) throwFatalError("Error on creating the main window");

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == NULL) throwFatalError("Error on creating renderer");

    if (ren != NULL && win != NULL) {
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);
        SDL_RenderPresent(ren);
    }
}

void Quit(bool& is_running, SDL_Event events)
{
    bool is_quit = true;
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);
    while (is_quit)
        while (SDL_PollEvent(&events))
        {
            switch (events.type)
            {
            case SDL_QUIT:
                is_running = false;
                is_quit = false;
                break;
            case SDL_KEYDOWN:
                is_quit = false;
                break;
            }
        }
}

void Change_Window(SDL_Event events)
{
    if (events.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    {
        win_width = events.window.data1;
        win_height = events.window.data2;
    }
}

void Draw(Player& ship, Enemy enemies[], int dt, weapon bullet, Planet &planet, const int n, SDL_Rect stars_mass[], double speed[], Enemy_Bullet & enemy_bullet, Boss & boss)
{
    SDL_SetRenderDrawColor(ren, 0, 0, 80, 0);
    SDL_RenderClear(ren);
    Planet_Moving(planet, dt);
    Falling_Stars(n, stars_mass, speed, dt);
    Meteorite_Draw(enemies[0], dt);
    Ship_Draw(enemies[1], dt);
    Explosion_Ship_Draw(enemies[2], dt);
    Changing_Ship_Draw(enemies[3]);
    Shooting_Ship_Draw(enemies[4]);
    bullet_draw(bullet);
    Enemy_Bullet_Draw(enemy_bullet);
    Boss_Draw(boss);
    ship.model = { int(ship.x_pos), (int)ship.y_pos, ship.size, ship.size };
    SDL_RenderCopy(ren, ship.texture, NULL, &ship.model);
}

void Explosion_Animation(SDL_Texture* texture, SDL_Rect **pos, SDL_Rect ** model, int ** frame, int ** cur_time, int dt, Enemy enemy[], int ** stop)
{
    int max_frame = 10, max_time = 150;
    for (int i = 0; i < 5; i++)
    {
        if (enemy[i].count != 0)
            for (int j = 0; j < enemy[i].count; j++)
            {
                if (stop[i][j] < max_frame - 1)
                {
                    cur_time[i][j] += dt;
                    if (cur_time[i][j] >= max_time)
                    {
                        cur_time[i][j] -= max_time;
                        frame[i][j] = (frame[i][j] + 1) % max_frame;
                        pos[i][j].x = pos[i][j].w * frame[i][j];
                        stop[i][j]++;
                    }
                    SDL_RenderCopy(ren, texture, &pos[i][j], &model[i][j]);
                }
                else
                    model[i][j] = { 0, 0, 0, 0 };
            }
    }
}

void Boss_Explosion(SDL_Texture* texture, SDL_Rect** pos, SDL_Rect** model, int** frame, int** cur_time, int dt, int** stop)
{
    int max_frame = 10, max_time = 150;
    for (int i = 0; i < 3; i++)
                if (stop[0][i] < max_frame - 1)
                {
                    cur_time[0][i] += dt;
                    if (cur_time[0][i] >= max_time)
                    {
                        cur_time[0][i] -= max_time;
                        frame[0][i] = (frame[0][i] + 1) % max_frame;
                        pos[0][i].x = pos[0][i].w * frame[0][i];
                        stop[0][i]++;
                    }
                    SDL_RenderCopy(ren, texture, &pos[0][i], &model[0][i]);
                }
                else
                    model[0][i] = { 0, 0, 0, 0 };
}

void Start_Explosion(SDL_Rect old_pos, SDL_Rect& new_pos, int& frame, int& cur_time)
{
    new_pos = old_pos;
    frame = 0;
    cur_time = 0;
}

void Start_Damage_Explosion(SDL_Rect old_pos, SDL_Rect& new_pos, int& frame, int& cur_time)
{
    new_pos = {0, 0, 0, 0};
    new_pos.y = old_pos.y - old_pos.h / 2;
    new_pos.x = old_pos.x - old_pos.w / 2;
    new_pos.w = old_pos.w * 2;
    new_pos.h = old_pos.h * 2;
    frame = 0;
    cur_time = 0;
}

void Collision_Player_And_Enemy(Player& ship, Enemy enemy[], SDL_Rect ** exp, int **frame, int** cur_time, int & score)
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < enemy[i].count; j++)
            if (ship.model.y <= enemy[i].enemy_mass[j].y + enemy[i].size and ship.model.y + ship.size >= enemy[i].enemy_mass[j].y)
                if (ship.model.x <= enemy[i].enemy_mass[j].x + enemy[i].size and ship.model.x + ship.size >= enemy[i].enemy_mass[j].x)
                {
                    ship.money += enemy[i].money;
                    score += enemy[i].score / 2;
                    ship.hp--;
                    enemy[i].hp[j] = 0;
                    if (i == 2)
                        Start_Damage_Explosion(enemy[i].enemy_mass[j], exp[i][j], frame[i][j], cur_time[i][j]);
                    else
                        Start_Explosion(enemy[i].enemy_mass[j], exp[i][j], frame[i][j], cur_time[i][j]);
                    enemy[i].enemy_mass[j].y = win_height + 1;
                }
    }
}

void Collision_Bullet_And_Enemy(Player& ship, weapon& bullet, Enemy enemy[], SDL_Rect** exp, int** frame, int** cur_time, int &score)
{
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < enemy[i].count; j++)
                for (int k = 0; k < bullet.count; k++)
                    if (bullet.active_bullet[k] == 1)
                        if (bullet.bullet_mas[k].y <= enemy[i].enemy_mass[j].y + enemy[i].size and bullet.bullet_mas[k].y + bullet.size_y >= enemy[i].enemy_mass[j].y)
                            if (bullet.bullet_mas[k].x <= enemy[i].enemy_mass[j].x + enemy[i].size and bullet.bullet_mas[k].x + bullet.size_x >= enemy[i].enemy_mass[j].x)
                            {
                                enemy[i].hp[j] -= bullet.dmg;
                                if (enemy[i].hp[j] <= 0)
                                {
                                    ship.money += enemy[i].money;
                                    score += enemy[i].score;
                                    if (i == 2)
                                        Start_Damage_Explosion(enemy[i].enemy_mass[j], exp[i][j], frame[i][j], cur_time[i][j]);
                                    else
                                        Start_Explosion(enemy[i].enemy_mass[j], exp[i][j], frame[i][j], cur_time[i][j]);
                                    enemy[i].enemy_mass[j].y = win_height + 1;
                                }
                                bullet.active_bullet[k] = 0;
                            }
    }

void Collision_Player_And_Explosion(Player& ship, Enemy enemy, SDL_Rect* explosion, int collision[])
{
    for (int i = 0; i < enemy.count; i++)
        if (ship.model.y <= explosion[i].y + explosion[i].h and ship.model.y + ship.size >= explosion[i].y)
            if (ship.model.x <= explosion[i].x + explosion[i].x and ship.model.x + ship.size >= explosion[i].x)
                if (collision[i] == 0)
                {
                    ship.hp -= 3;
                    collision[i]++;
                }
}

void Collision_Player_And_Bullet(Player& ship, Enemy_Bullet& bullet)
{
    for (int i = 0; i < bullet.count; i++)
        if (bullet.active_bullet[i] == 1)
            if (ship.y_pos <= bullet.bullet_mas[i].y + bullet.size_y and ship.y_pos + ship.size >= bullet.bullet_mas[i].y)
                if (ship.x_pos <= bullet.bullet_mas[i].x + bullet.size_x and ship.x_pos + ship.size >= bullet.bullet_mas[i].x)
                {
                    ship.hp -= bullet.dmg;
                    bullet.active_bullet[i] = 0;
                    bullet.bullet_mas[i] = { 0, win_width + 1, 0, 0 };
                }

}

void Collision_Bullet_And_Boss(weapon& bullet, Boss& boss, SDL_Rect* exp, int* frame, int* cur_time, int& score)
{
    for (int i = 0; i < bullet.count; i++)
        if (bullet.active_bullet[i] == 1)
        {
            if (bullet.bullet_mas[i].y <= boss.weak_point_left.y + boss.model.h / 3 * 2 - 30 and bullet.bullet_mas[i].y + bullet.size_y >= boss.weak_point_left.y)
                if (bullet.bullet_mas[i].x <= boss.weak_point_left.x + boss.offset and bullet.bullet_mas[i].x + bullet.size_x >= boss.weak_point_left.x)
                {
                    boss.hp_left -= bullet.dmg;
                    boss.left.w -= boss.offset / 15 * bullet.dmg;
                    if (boss.left.w < 0)
                        boss.left.w = 0;
                    bullet.active_bullet[i] = 0;
                    if (boss.hp_left <= 0)
                    {
                        boss.speed_x = fabs(boss.speed_x) + 50;
                        boss.speed_y = fabs(boss.speed_y) + 50;
                        Start_Explosion(boss.weak_point_left, exp[0], frame[0], cur_time[0]);
                        boss.rect.x += boss.offset_rect;
                        boss.rect.w -= boss.offset_rect;
                        boss.model.x += boss.offset;
                        boss.model.w -= boss.offset;
                    }
                }
            if (bullet.bullet_mas[i].y <= boss.weak_point_right.y + boss.model.h / 3 * 2 - 30 and bullet.bullet_mas[i].y + bullet.size_y >= boss.weak_point_right.y)
                if (bullet.bullet_mas[i].x <= boss.weak_point_right.x + boss.offset and bullet.bullet_mas[i].x + bullet.size_x >= boss.weak_point_right.x)
                {
                    boss.hp_right -= bullet.dmg;
                    boss.right.w -= boss.offset / 15 * bullet.dmg;
                    if (boss.right.w < 0)
                        boss.right.w = 0;
                    bullet.active_bullet[i] = 0;
                    if (boss.hp_right <= 0)
                    {
                        boss.speed_x = fabs(boss.speed_x) + 50;
                        boss.speed_y = fabs(boss.speed_y) + 50;
                        Start_Explosion(boss.weak_point_right, exp[1], frame[1], cur_time[1]);
                        boss.rect.w -= boss.offset_rect;
                        boss.model.w -= boss.offset;
                    }
                }
            if (boss.second_phase)
                if (bullet.bullet_mas[i].y <= boss.weak_point_center.y + boss.model.h / 3 * 2 and bullet.bullet_mas[i].y + bullet.size_y >= boss.weak_point_center.y)
                    if (bullet.bullet_mas[i].x <= boss.weak_point_center.x + boss.offset and bullet.bullet_mas[i].x + bullet.size_x >= boss.weak_point_center.x)
                    {
                        boss.hp_center -= bullet.dmg;
                        boss.center.w -= boss.offset / 30 * bullet.dmg;
                        if (boss.center.w < 0)
                            boss.center.w = 0;
                        bullet.active_bullet[i] = 0;
                        if (boss.hp_center <= 0)
                        {
                            Start_Explosion(boss.weak_point_center, exp[2], frame[2], cur_time[2]);
                        }
                    }
        }
}

void Collision_Player_And_Boss(Player& ship, Boss& boss)
{
    if (ship.model.y <= boss.model.y + boss.model.h and ship.model.y + ship.size >= boss.model.y)
        if (ship.model.x <= boss.model.x + boss.model.w and ship.model.x + ship.size >= boss.model.x)
            ship.hp = 0;
}

void Collision_Player_And_Laser(Player& ship, Boss& boss)
{
    if (ship.model.y <= boss.laser_model_down.y + 100 and ship.model.y + ship.size >= boss.laser_model_down.y)
        if (boss.collision_down == 0)
        {
            boss.collision_down++;
            ship.hp -= 5;
        }
    if (ship.model.y <= boss.laser_model_up.y + 100 and ship.model.y + ship.size >= boss.laser_model_up.y)
        if (boss.collision_up == 0)
        {
            boss.collision_up++;
            ship.hp -= 5;
        }
}

SDL_Texture* Load_Texture(const char* name, SDL_Rect * rect)
{
    SDL_Surface* surface = IMG_Load(name);
    if (surface == NULL) throwFatalError("Couldn't load image");
    SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, 255, 255, 255));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
    *rect = { 0, 0, surface->w, surface->h };
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture* Load_Texture_Font(const char* text, TTF_Font* font, SDL_Rect* rect, SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    *rect = { 0, 0, surface->w, surface->h };
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void Level(Enemy enemies[], int wave[])
{
    Create_Meteorite(enemies[0], wave[0]);
    Create_Moving_Ship(enemies[1], wave[1]);
    Create_Explosion_Ship(enemies[2], wave[2]);
    Create_Changing_Ship(enemies[3], wave[3]);
    Create_Shooting_Ship(enemies[4], wave[4]);
}

void Get_Enemies_In_Wave(int wave_1[], int wave_2[], const char * filename)
{
    FILE* level = fopen(filename, "r");
    for (int i = 0; i < 5; i++)
        fscanf_s(level, "%i ", &wave_1[i]);
    for (int i = 0; i < 5; i++)
        fscanf_s(level, "%i ", &wave_2[i]);
    fclose(level);

}

void Get_Max_Score(int mass[], const int n, const char* filename)
{
    FILE* score = fopen(filename, "r");
    for (int i = 0; i < n - 1; i++)
        fscanf_s(score, "%i ", &mass[i]);
    fclose(score);
}

void Sort_Records(int mass[], const int n)
{
    int x = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n - 1; j++)
            if (mass[j] < mass[j + 1])
            {
                x = mass[j + 1];
                mass[j + 1] = mass[j];
                mass[j] = x;
            }
}

void Save_Records(int mass[], const int n, const char* filename)
{
    FILE* score = fopen(filename, "w");
    for (int i = 0; i < n - 1; i++)
        fprintf_s(score, "%i ", mass[i]);
    fclose(score);
}

SDL_Texture* New_Memory(Enemy enemies[], SDL_Rect ** explosion_model, SDL_Rect ** pos, int ** frame, int ** cur_time, int ** stop_animation)
{
    for (int i = 0; i < 5; i++)
        if (enemies[i].count != 0)
        {
            explosion_model[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect) * enemies[i].count);
            pos[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect) * enemies[i].count);
            frame[i] = (int*)malloc(sizeof(int) * enemies[i].count);
            cur_time[i] = (int*)malloc(sizeof(int) * enemies[i].count);
            stop_animation[i] = (int*)malloc(sizeof(int) * enemies[i].count);
            for (int j = 0; j < enemies[i].count; j++)
            {
                frame[i][j] = INT_MIN;
                cur_time[i][j] = INT_MIN;
                stop_animation[i][j] = 0;
            }
        }
        else
        {
            explosion_model[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
            pos[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
            frame[i] = (int*)malloc(sizeof(int));
            cur_time[i] = (int*)malloc(sizeof(int));
            stop_animation[i] = (int*)malloc(sizeof(int));
            stop_animation[i][0] = 0;
        }
    SDL_Rect rect = { 0, 0, 0, 0 };
    SDL_Texture* exp = Load_Texture("explosion.png", &rect);
    rect.w = rect.h;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < enemies[i].count; j++)
            pos[i][j] = rect;
    return exp;
}

SDL_Texture* Realloc_Memory(Enemy enemies[], SDL_Rect** explosion_model, SDL_Rect** pos, int** frame, int** cur_time, int** stop_animation)
{
    for (int i = 0; i < 5; i++)
        if (enemies[i].count != 0)
        {
            explosion_model[i] = (SDL_Rect*)realloc(explosion_model[i], sizeof(SDL_Rect) * enemies[i].count);
            pos[i] = (SDL_Rect*)realloc(pos[i], sizeof(SDL_Rect) * enemies[i].count);
            frame[i] = (int*)realloc(frame[i], sizeof(int) * enemies[i].count);
            cur_time[i] = (int*)realloc(cur_time[i], sizeof(int) * enemies[i].count);
            stop_animation[i] = (int*)realloc(stop_animation[i], sizeof(int) * enemies[i].count);
            for (int j = 0; j < enemies[i].count; j++)
            {
                frame[i][j] = INT_MIN;
                cur_time[i][j] = INT_MIN;
                stop_animation[i][j] = 0;
                pos[i][j] = { 0, 0, 0, 0 };
            }
        }
        else
        {
            explosion_model[i] = (SDL_Rect*)realloc(explosion_model[i], sizeof(SDL_Rect));
            pos[i] = (SDL_Rect*)realloc(pos[i], sizeof(SDL_Rect));
            frame[i] = (int*)realloc(frame[i], sizeof(int));
            cur_time[i] = (int*)realloc(cur_time[i], sizeof(int));
            stop_animation[i] = (int*)realloc(stop_animation[i], sizeof(int));
        }
    SDL_Rect rect = { 0, 0, 0, 0 };
    SDL_Texture* exp = Load_Texture("explosion.png", &rect);
    rect.w = rect.h;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < enemies[i].count; j++)
            pos[i][j] = rect;
    return exp;
}

SDL_Texture* Boss_Memory(SDL_Rect** explosion_model, SDL_Rect** pos, int** frame, int** cur_time, int** stop_animation)
{
    explosion_model[0] = (SDL_Rect*)realloc(explosion_model[0], sizeof(SDL_Rect) * 3);
    pos[0] = (SDL_Rect*)realloc(pos[0], sizeof(SDL_Rect) * 3);
    frame[0] = (int*)realloc(frame[0], sizeof(int) * 3);
    cur_time[0] = (int*)realloc(cur_time[0], sizeof(int) * 3);
    stop_animation[0] = (int*)realloc(stop_animation[0], sizeof(int) * 3);
    for (int j = 0; j < 3; j++)
    {
        frame[0][j] = INT_MIN;
        cur_time[0][j] = INT_MIN;
        stop_animation[0][j] = 0;
        pos[0][j] = { 0, 0, 0, 0 };
    }
    SDL_Rect rect = { 0, 0, 0, 0 };
    SDL_Texture* exp = Load_Texture("explosion.png", &rect);
    rect.w = rect.h;
        for (int j = 0; j < 3; j++)
            pos[0][j] = rect;
    return exp;
}

void Free_Memory(Enemy enemies[], SDL_Rect** explosion_model, SDL_Rect** pos, int** frame, int** cur_time, int** stop_animation)
{
    for (int i = 0; i < 5; i++)
    {
        if(explosion_model[i] != nullptr)
            free(explosion_model[i]);
        if (frame[i] != nullptr)
            free(frame[i]);
        if (cur_time[i] != nullptr)
            free(cur_time[i]);
        if (stop_animation[i] != nullptr)
            free(stop_animation[i]);
        if (pos[i] != nullptr)
            free(pos[i]);
    }
    if (explosion_model != nullptr)
        free(explosion_model);
    if (frame != nullptr)
        free(frame);
    if (cur_time != nullptr)
        free(cur_time);
    if (stop_animation != nullptr)
        free(stop_animation);
    if (pos != nullptr)
        free(pos);
}

bool Is_Not_Enemies_In_Screen(Enemy enemies[])
{
    int c = 0;
    for (int i = 0; i < 5; i++)
    {
        if (enemies[i].is_null)
        {
            c++;
        }
        
    }
    return c == 5;
}

void Get_Time(int & new_time, int & last_time, int & dt)
{
    new_time = SDL_GetTicks();
    dt = new_time - last_time;
    if (dt < 16)
    {
        SDL_Delay(16 - dt);
        new_time = SDL_GetTicks();
        dt = new_time - last_time;
    }
    last_time = new_time;
}

void Save(Player ship, const char* filename, int texture, int score, int level_1, int level_2, int level_3, int level_4, int level_5)
{
    FILE* save = fopen(filename, "w");
    fprintf_s(save, "%i ", texture);
    fprintf_s(save, "%i ", ship.max_hp);
    fprintf_s(save, "%i ", ship.hp);
    fprintf_s(save, "%i ", ship.money);
    fprintf_s(save, "%lf ", ship.speed);
    fprintf_s(save, "%i\n", score);
    fprintf_s(save, "%i ", level_1);
    fprintf_s(save, "%i ", level_2);
    fprintf_s(save, "%i ", level_3);
    fprintf_s(save, "%i ", level_4);
    fprintf_s(save, "%i\n", level_5);
    fclose(save);
}

void Load_Save(Player& ship, const char* filename, int& ship_type, int &score, int &level_1, int &level_2, int &level_3, int &level_4, int &level_5)
{
    FILE* save = fopen(filename, "r");
    fscanf_s(save, "%i ", &ship_type);
    switch (ship_type)
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
    fscanf_s(save, "%i\n", &score);
    fscanf_s(save, "%i ", &level_1);
    fscanf_s(save, "%i ", &level_2);
    fscanf_s(save, "%i ", &level_3);
    fscanf_s(save, "%i ", &level_4);
    fscanf_s(save, "%i\n", &level_5);
    fclose(save);
}

void Level_Play(bool & get_enemies, int wave_1[], int wave_2[], Enemy enemies[], SDL_Texture** exp, SDL_Rect** explosion_model, SDL_Rect** pos,
    int** frame, int** cur_time, int** stop_animation, int collision[], bool & is_new_wave, int & new_wave_time, bool &is_not_enemies, int &next, int &level_choose, const char * level)
{
    if (not get_enemies)
    {
        Get_Enemies_In_Wave(wave_1, wave_2, level);
        Level(enemies, wave_1);
        SDL_DestroyTexture(*exp);
        *exp = Realloc_Memory(enemies, explosion_model, pos, frame, cur_time, stop_animation);
        get_enemies = true;
    }

    if (new_wave_time >= 3000 and not is_new_wave)
    {
        Level(enemies, wave_2);
        SDL_DestroyTexture(*exp);
        *exp = Realloc_Memory(enemies, explosion_model, pos, frame, cur_time, stop_animation);
        for (int i = 0; i < 100; i++)
            collision[i] = 0;
        is_new_wave = true;
        is_not_enemies = false;
        new_wave_time = 0;
    }

    if (is_new_wave and new_wave_time >= 5000)
    {
        is_new_wave = false;
        new_wave_time = 0;
        level_choose = 0;
        if (next == 0)
            next++;
        get_enemies = false;
    }
}

int main(int argc, char* argv[])
{
    system("chcp 1251 > 0");
    srand(time(0));
    const Uint8* state = SDL_GetKeyboardState(NULL);
    SDL_Event events;
    Player ship;
    Enemy enemies[5] = { 0 };
    weapon bullet;
    Enemy_Bullet enemy_bullet;
    Planet planet;
    Boss boss;
    bool is_running = true, is_free = false, is_new_wave = false, is_cooldown = true, is_game = true, is_record = false;
    int wave_1[5], wave_2[5];
    int score = 0, ship_type = 0, key = 0;
    const int n = 6;
    int score_mass[n] = { 0 };
    int level = 1;
    Get_Max_Score(score_mass, n, "score_records.txt");
    int level_1 = 1, level_2 = 0, level_3 = 0, level_4 = 0, level_5 = 0, level_choose, save_choose = 0;
    do
    {
        printf("Новая игра - 1.\n\
Загрузить игру - 2.\n");
        scanf_s("%i", &key);
    } while (key != 1 and key != 2);

    initSDL();
    switch (key)
    {
    case 1:
        do
        {
            printf("1- лёгкий корабль.\n\
2- тяжёлый корабль.\n");
            scanf_s("%i", &ship_type);
        } while (ship_type != 1 and ship_type != 2);
        switch (ship_type)
        {
        case 1:
            Create_Light_Ship(ship);
            break;
        case 2:
            Create_Heavy_Ship(ship);
        }
        break;
    case 2:
        do
        {
            printf("Введите ячейку сохранения: ");
            scanf_s("%i", &save_choose);
        } while (save_choose < 1 or save_choose > 3);
        switch (save_choose)
        {
        case 1:
            Load_Save(ship, "save_1.txt", ship_type, score, level_1, level_2, level_3, level_4, level_5);
            break;
        case 2:
            Load_Save(ship, "save_2.txt", ship_type, score, level_1, level_2, level_3, level_4, level_5);
            break;
        case 3:
            Load_Save(ship, "save_3.txt", ship_type, score, level_1, level_2, level_3, level_4, level_5);
            break;
        }
        break;
    }

    do
    {
        printf("Введите номер уровня: ");
        scanf_s("%i", &level_choose);
        if (level_choose == 2 and level_2 == 0 or level_choose == 3 and level_3 == 0 or level_choose == 4 and level_4 == 0 or level_choose == 5 and level_5 == 0)
            printf("Уровень не открыт. Пройдите предыдущие, чтобы открыть его\n");
    } while ((level_choose < 1 or level_choose > 5) or (level_choose == 2 and level_2 == 0 or level_choose == 3 and level_3 == 0 or level_choose == 4 and level_4 == 0 or level_choose == 5 and level_5 == 0));
    Create_Stars(stars_number, stars_mass, speed);
    bullet_create(bullet);
    Enemy_Bullet_Create(enemy_bullet);
    Create_Planets(planet);
    Level(enemies, wave_1);
    int last_time = SDL_GetTicks(), new_time = 0, dt = 0, new_wave_time = 0;
    SDL_Rect** pos = (SDL_Rect**)malloc(sizeof(SDL_Rect*) * 5);
    SDL_Rect** explosion_model = (SDL_Rect**)malloc(sizeof(SDL_Rect*) * 5);
    int** frame = (int**)malloc(sizeof(int*) * 5);
    int** cur_time = (int**)malloc(sizeof(int*) * 5);
    int** stop_animation = (int**)malloc(sizeof(int*) * 5);
    int collision[100] = { 0 };
    SDL_Texture* exp = New_Memory(enemies, explosion_model, pos, frame, cur_time, stop_animation);
    TTF_Font* hp_font = TTF_OpenFont("calibri.ttf", 25);
    SDL_Texture* hp_texture = NULL;
    SDL_Rect hp_rect = { 0, 0, 0, 0 };
    char hp_str[10] = "HP: %i";
    TTF_Font* score_font = TTF_OpenFont("calibri.ttf", 25);
    SDL_Texture* score_texture = NULL;
    SDL_Rect score_rect = { 0, 0, 0, 0 };
    char score_str[50] = "SCORE: %i";
    TTF_Font* record_font = TTF_OpenFont("calibri.ttf", 75);
    SDL_Texture* first = NULL;
    SDL_Texture* second = NULL;
    SDL_Texture* third = NULL;
    SDL_Texture* fourth = NULL;
    SDL_Texture* fifth = NULL;
    SDL_Texture* score_texture_2 = NULL;
    SDL_Rect score_mass_rect[n] = { 0 };
    char first_score[50] = "FIRST: %i";
    char second_score[50] = "SECOND: %i";
    char third_score[50] = "THIRD: %i";
    char fourth_score[50] = "FOURTH: %i";
    char fifth_score[50] = "FIFTH: %i";
    char cur_score[50] = "CURRENT: %i";
    int over = 0;
    bool is_not_enemies = false, get_enemies = false;
    bool is_boss = false;
    while (is_running)
    {
        while (is_game)
        {
            while (SDL_PollEvent(&events))
            {
                switch (events.type)
                {
                case SDL_QUIT:
                    Quit(is_running, events);
                    if (not is_running)
                        is_game = false;
                    break;
                case SDL_WINDOWEVENT:
                    Change_Window(events);
                    break;
                case SDL_KEYDOWN:
                    switch (events.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_SPACE:
                         for (int i = 0; i < bullet.count; i++)
                        {
                            if (bullet.active_bullet[i] == 0 and not is_cooldown)
                            {
                                bullet.active_bullet[i] = 1;
                                bullet_spawn(bullet, ship);
                                is_cooldown = true;
                                break;
                            }
                        }
                    }
                }
            }

            if (ship.hp <= 0)
            {
                score_mass[n - 1] = score;
                Sort_Records(score_mass, n);
                is_game = false;
                is_record = true;
            }

            switch (level_choose)
            {
            case 0:
                do
                {
                    printf("Введите номер уровня: ");
                    scanf_s("%i", &level_choose);
                    if (level_choose == 2 and level_2 == 0 or level_choose == 3 and level_3 == 0 or level_choose == 4 and level_4 == 0 or level_choose == 5 and level_5 == 0)
                        printf("Уровень не открыт. Пройдите предыдущие, чтобы открыть его\n");
                } while ((level_choose < 1 or level_choose > 5) or (level_choose == 2 and level_2 == 0 or level_choose == 3 and level_3 == 0 or level_choose == 4 and level_4 == 0 or level_choose == 5 and level_5 == 0));
                is_game = true;
                break;
            case 1:
                Level_Play(get_enemies, wave_1, wave_2, enemies, &exp, explosion_model, pos, frame, cur_time, stop_animation, collision, is_new_wave, new_wave_time, is_not_enemies, level_2, level_choose, "level_1.txt");
                break;
            case 2:
                Level_Play(get_enemies, wave_1, wave_2, enemies, &exp, explosion_model, pos, frame, cur_time, stop_animation, collision, is_new_wave, new_wave_time, is_not_enemies, level_3, level_choose, "level_2.txt");
                break;
            case 3:
                Level_Play(get_enemies, wave_1, wave_2, enemies, &exp, explosion_model, pos, frame, cur_time, stop_animation, collision, is_new_wave, new_wave_time, is_not_enemies, level_4, level_choose, "level_3.txt");
                break;
            case 4:
                Level_Play(get_enemies, wave_1, wave_2, enemies, &exp, explosion_model, pos, frame, cur_time, stop_animation, collision, is_new_wave, new_wave_time, is_not_enemies, level_5, level_choose, "level_4.txt");
                break;
            case 5:
                if (not is_boss)
                {
                    SDL_DestroyTexture(exp);
                    exp = Boss_Memory(explosion_model, pos, frame, cur_time, stop_animation);
                    for (int i = 0; i < 5; i++)
                        wave_1[i] = 0;
                    Level(enemies, wave_1);
                    Create_Boss(boss);
                    is_boss = true;
                    break;
                }
            }

                is_not_enemies = Is_Not_Enemies_In_Screen(enemies);
                if (is_not_enemies and not is_new_wave)
                    new_wave_time += dt;
                if (is_not_enemies and is_new_wave)
                    new_wave_time += dt;

                if (is_running)
                {
                    Get_Time(new_time, last_time, dt);

                    if (not Is_Cooldown_Now(bullet, dt))
                        is_cooldown = false;

                    sprintf_s(hp_str, "HP: %i", ship.hp);
                    if (hp_texture != NULL)
                        SDL_DestroyTexture(hp_texture);
                    hp_texture = Load_Texture_Font(hp_str, hp_font, &hp_rect, { 255, 255, 255, 255 });
                    hp_rect.x = 10;
                    hp_rect.y = win_height - 50;

                    sprintf_s(score_str, "SCORE: %i", score);
                    if (score_texture != NULL)
                        SDL_DestroyTexture(score_texture);
                    score_texture = Load_Texture_Font(score_str, score_font, &score_rect, { 255, 255, 255, 255 });
                    score_rect.x = 10;
                    score_rect.y = 10;

                    Player_Movement(state, ship, dt, planet, stars_number, stars_mass);
                    Meteorite_Movement(enemies[0], ship, dt);
                    Ship_Movement(enemies[1], ship, dt);
                    Explosion_Ship_Movement(enemies[2], ship, dt);
                    Changing_Ship_Moving(enemies[3], ship, dt);
                    Shooting_Ship_Movement(enemies[4], ship, dt, enemy_bullet);
                    bullet_movement(bullet, dt);
                    if (level_choose == 5)
                        Boss_Movement(boss, dt);
                    Enemy_Bullet_Movement(enemy_bullet, dt);
                    Collision_Player_And_Enemy(ship, enemies, explosion_model, frame, cur_time, score);
                    Collision_Bullet_And_Enemy(ship, bullet, enemies, explosion_model, frame, cur_time, score);
                    Collision_Player_And_Bullet(ship, enemy_bullet);
                    Collision_Player_And_Explosion(ship, enemies[2], explosion_model[2], collision);
                    if (level_choose == 5)
                    {
                        Collision_Bullet_And_Boss(bullet, boss, explosion_model[0], frame[0], cur_time[0], score);
                        Collision_Player_And_Boss(ship, boss); 
                        Collision_Player_And_Laser(ship, boss);
                    }
                    Draw(ship, enemies, dt, bullet, planet, stars_number, stars_mass, speed, enemy_bullet, boss);
                    if (level_choose == 5)
                        Boss_Explosion(exp, pos, explosion_model, frame, cur_time, dt, stop_animation);
                    else
                        Explosion_Animation(exp, pos, explosion_model, frame, cur_time, dt, enemies, stop_animation);
                    SDL_RenderCopy(ren, hp_texture, NULL, &hp_rect);
                    SDL_RenderCopy(ren, score_texture, NULL, &score_rect);
                    SDL_RenderPresent(ren);
                }
            }
            while (is_record)
            {
                while (SDL_PollEvent(&events))
                {
                    switch (events.type)
                    {
                    case SDL_QUIT:
                        Quit(is_running, events);
                        if (not is_running)
                            is_record = false;
                        break;
                    case SDL_WINDOWEVENT:
                        Change_Window(events);
                        break;
                    }
                }
                if (is_record)
                {
                    Get_Time(new_time, last_time, dt);

                    over += dt;
                    if (over >= 10000)
                        is_record = false;
                    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
                    SDL_RenderClear(ren);
                    sprintf_s(first_score, "FIRST: %i", score_mass[0]);
                    if (first != NULL)
                        SDL_DestroyTexture(first);
                    first = Load_Texture_Font(first_score, score_font, &score_mass_rect[0], { 255, 255, 255, 255 });
                    score_mass_rect[0].x = win_width / 2 - 50;
                    score_mass_rect[0].y = win_height / 7;
                    SDL_RenderCopy(ren, first, NULL, &score_mass_rect[0]);

                    sprintf_s(second_score, "SECOND: %i", score_mass[1]);
                    if (second != NULL)
                        SDL_DestroyTexture(second);
                    second = Load_Texture_Font(second_score, score_font, &score_mass_rect[1], { 255, 255, 255, 255 });
                    score_mass_rect[1].x = win_width / 2 - 50;
                    score_mass_rect[1].y = win_height / 7 * 2;
                    SDL_RenderCopy(ren, second, NULL, &score_mass_rect[1]);

                    sprintf_s(third_score, "THIRD: %i", score_mass[2]);
                    if (third != NULL)
                        SDL_DestroyTexture(third);
                    third = Load_Texture_Font(third_score, score_font, &score_mass_rect[2], { 255, 255, 255, 255 });
                    score_mass_rect[2].x = win_width / 2 - 50;
                    score_mass_rect[2].y = win_height / 7 * 3;
                    SDL_RenderCopy(ren, third, NULL, &score_mass_rect[2]);

                    sprintf_s(fourth_score, "FOURTH: %i", score_mass[3]);
                    if (fourth != NULL)
                        SDL_DestroyTexture(fourth);
                    fourth = Load_Texture_Font(fourth_score, score_font, &score_mass_rect[3], { 255, 255, 255, 255 });
                    score_mass_rect[3].x = win_width / 2 - 50;
                    score_mass_rect[3].y = win_height / 7 * 4;
                    SDL_RenderCopy(ren, fourth, NULL, &score_mass_rect[3]);

                    sprintf_s(fifth_score, "FIFTH: %i", score_mass[4]);
                    if (fifth != NULL)
                        SDL_DestroyTexture(fifth);
                    fifth = Load_Texture_Font(fifth_score, score_font, &score_mass_rect[4], { 255, 255, 255, 255 });
                    score_mass_rect[4].x = win_width / 2 - 50;
                    score_mass_rect[4].y = win_height / 7 * 5;
                    SDL_RenderCopy(ren, fifth, NULL, &score_mass_rect[4]);

                    sprintf_s(cur_score, "CURRENT: %i", score);
                    if (score_texture_2 != NULL)
                        SDL_DestroyTexture(score_texture);
                    score_texture_2 = Load_Texture_Font(cur_score, score_font, &score_mass_rect[5], { 255, 255, 255, 255 });
                    score_mass_rect[5].x = win_width / 2 - 50;
                    score_mass_rect[5].y = win_height / 7 * 6;
                    SDL_RenderCopy(ren, score_texture_2, NULL, &score_mass_rect[5]);

                    SDL_RenderPresent(ren);
                }
            }
            if (not is_game and not is_record)
                is_running = false;
        }

        Sort_Records(score_mass, n);
        Save_Records(score_mass, n, "score_records.txt");
        /* do
         {
             printf("В какую ячейку сохранить: ");
             scanf_s("%i", &save_choose);
         } while (save_choose < 1 or save_choose > 3);
         switch (save_choose)
         {
         case 1:
             Save(ship, "save_1.txt", ship_type, score, level_1, level_2, level_3, level_4, level_5);
             break;
         case 2:
             Save(ship, "save_2.txt", ship_type, score, level_1, level_2, level_3, level_4, level_5);
             break;
         case 3:
             Save(ship, "save_3.txt", ship_type, score, level_1, level_2, level_3, level_4, level_5);
             break;
         }*/
        Free_Memory(enemies, explosion_model, pos, frame, cur_time, stop_animation);
        SDL_DestroyTexture(ship.texture);
        SDL_DestroyTexture(bullet.texture);
        SDL_DestroyTexture(enemy_bullet.texture);
        SDL_DestroyTexture(exp);
        SDL_DestroyTexture(planet.texture);
        SDL_DestroyTexture(hp_texture);
        SDL_DestroyTexture(score_texture);
        SDL_DestroyTexture(first);
        SDL_DestroyTexture(second);
        SDL_DestroyTexture(third);
        SDL_DestroyTexture(fourth);
        SDL_DestroyTexture(fifth);
        SDL_DestroyTexture(boss.texture);
        SDL_DestroyTexture(boss.laser_texture);
        SDL_DestroyTexture(boss.warning_texture);
        deinit();
        return 0;
}