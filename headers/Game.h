#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <limits.h>

extern int win_width;
extern int win_height;
extern int center_x;
extern int center_y;

extern SDL_Window* win;
extern SDL_Renderer* ren;

SDL_Texture* Load_Texture(const char* name, SDL_Rect* rect);

void Start_Explosion(SDL_Rect old_pos, SDL_Rect& new_pos, int& frame, int& cur_time);