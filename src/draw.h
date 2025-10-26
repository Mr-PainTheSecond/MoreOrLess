#ifndef DRAW_H_
#define DRAW_H_
#include "commons.h"
#include "globals.h"
void draw(TTF_Text* more, TTF_Text* less, Queue* queue, int score, int state);
SDL_FRect createRect(float x, float y, float w, float h);
float center(float pos, float size);
#endif