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

bool Is_Cooldown_Now(weapon& bullet, int dt)
{
    bullet.cur_time += dt;
    if (bullet.cur_time >= bullet.cd)
    {
        bullet.cur_time -= bullet.cd;
        return false;
    }
    return true;
}

void bullet_create(weapon& bullet) {
    bullet.texture = Load_Texture("bullet.png", &bullet.rect);
    bullet.size_x = 20;
    bullet.size_y = 50;
    bullet.dmg = 10;
    bullet.cd = 500;
    bullet.vy = 400;
    bullet.vx = 100;
    bullet.count = 50;
    for (int i = 0; i < bullet.count; i++)
        bullet.bullet_mas[i] = { 0, win_height + bullet.size_y + 1, 0, 0 };
    bullet.is_NULL = false;
}

void bullet_movement(weapon& bullet, int dt) {
    int cnt = 0;
    for (int i = 0; i < bullet.count; i++) {
        if (bullet.active_bullet[i] == 0)
            cnt++;
    }
    if (cnt == bullet.count && !bullet.is_NULL)
        bullet.is_NULL = true;
    else
        bullet.is_NULL = false;
    if (not bullet.is_NULL)
        for (int i = 0; i < bullet.count; i++) {
            if (bullet.active_bullet[i] == 0 and bullet.bullet_mas[i].y != win_height + bullet.size_y + 1)
                bullet.bullet_mas[i] = { 0, win_height + bullet.size_y + 1, 0, 0 };
            if (bullet.active_bullet[i] == 1)
            {
                bullet.bullet_mas[i].y -= bullet.vy * dt / 1000;
                if (bullet.bullet_mas[i].y + bullet.size_y < 0)
                {
                    bullet.bullet_mas[i] = { 0, win_height + bullet.size_y + 1, 0, 0 };
                    bullet.active_bullet[i] = 0;
                }
            }
        }
}

void bullet_spawn(weapon& bullet, Player player) {
    for (int i = 0; i < 50; i++)
        if (bullet.bullet_mas[i].y > win_height + bullet.size_y && bullet.active_bullet[i] == 1) {
            bullet.pos_x = player.x_pos + player.size / 2 - 10;
            bullet.pos_y = player.y_pos;
            bullet.bullet_mas[i] = { (int)bullet.pos_x, (int)bullet.pos_y, bullet.size_x, bullet.size_y };
        }

}

void bullet_draw(weapon bullet)
{
    for (int i = 0; i < bullet.count; i++)
        if (bullet.active_bullet[i] == 1)
             SDL_RenderCopy(ren, bullet.texture, NULL, &bullet.bullet_mas[i]);
}