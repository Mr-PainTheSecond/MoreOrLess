#ifndef GLOBALS_H_
#define GLOBALS_H_
#include "commons.h"

SDL_Renderer* renderer;
SDL_Window* window;
typedef struct screen {
	int w;
	int h;
	SDL_Surface* surface;
} Screen;
Screen* screen;
typedef struct fonts {
	TTF_Font** fonts;
	int fontIndex;
	int fontSize;
} Fonts;

typedef struct ytNode {
	struct ytNnode* next;
	int views;
	char* filePath;
	char* sViews;
	SDL_Texture* img;
} YTNode;

typedef struct queue {
	YTNode* front;
	YTNode* back;
} Queue;

enum states {normal, moreRight, moreWrong, lessRight, lessWrong};

Fonts* fontArray;
TTF_TextEngine* textEngine;
SDL_Event event;
#endif


