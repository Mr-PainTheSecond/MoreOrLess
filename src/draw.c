#include "draw.h"
#include "font.h"


SDL_FRect get_rect_center(SDL_FRect dst, SDL_FRect src) {
	dst.x = src.x + (src.w / 2) - (dst.w / 2);
	dst.y = src.y + (src.h / 2) - (dst.h / 2);
	return dst;
}


void get_text_center(SDL_FRect rect, TTF_Text* text, int* x, int* y) {
	if (!TTF_GetTextSize(text, x, y)) {
		fprintf(stderr, "Failed cause of %s\n", SDL_GetError());
		exit(-1);
	}
	if (x != NULL) {
		*x = rect.x + (rect.w / 2) - (*x / 2);
	}
	if (y != NULL) {
		*y = rect.y + (rect.h / 2) - (*y / 2);
	}
}

float center(float pos, float size) {
	return pos - (size / 2);
}

/*Initializes all the variables in the SDL_FRect struct
Not hard, but pretty repetitive task*/
SDL_FRect createRect(float x, float y, float w, float h) {
	SDL_FRect rect;
	rect.x = center(x, w);
	rect.y = center(y, h);
	rect.w = w;
	rect.h = h;
	return rect;
}

/*Draw the main component of the more or less game.
It is just a skelaton for now*/
void drawMoreOrLess(TTF_Text* moreTxt, TTF_Text* lessTxt, Queue* queue, int score, int state) {
	static TTF_Text* views = NULL;
	static TTF_Text* txtScore = NULL;
	static TTF_Font* smallFont;
	static int data;
	char* textScr = NULL;
	static char* viewChar = NULL;
	if (views == NULL) {
		viewChar = format(queue->front->sViews);
		smallFont = createFont("../assets/font/stentiga.TTF", 80.0);
		views = TTF_CreateText(textEngine, smallFont, viewChar, strlen(viewChar) + 1);
		txtScore = TTF_CreateText(textEngine, smallFont, "0", strlen("0") + 1);
		data = queue->front->views;
	}
	if (data != queue->front->views) {
		zstr_free(&textScr);
		viewChar = format(queue->front->sViews);
		views = TTF_CreateText(textEngine, smallFont, viewChar, strlen(viewChar) + 1);
		if (score != 0) {
			char* textScr = malloc(sizeof(char) * (int)log10(score) + 2);
			if (textScr == NULL) {
				exit(1);
			}
			int temp = score;
			for (int a = log10(score); a >= 0; a--) {
				int digit = (int)(temp / pow(10, a)) % 10;
				textScr[(int)log10(score) - a] = (int)(digit + 48);
				temp -= pow(10, a) * digit;
			}

			textScr[(int)log10(score) + 1] = '\0';
			txtScore = TTF_CreateText(textEngine, smallFont, textScr, strlen(textScr) + 1);
		}
		else {
			txtScore = TTF_CreateText(textEngine, smallFont, "0", strlen("0") + 1);
		}
		data = queue->front->views;
	}
	float w = screen->w;
	float h = screen->h;
	SDL_FRect moreImg = createRect(w / 4, h * 3 /8, w / 4, h / 2);
	SDL_FRect lessImg = createRect(w * 3 / 4, h * 3 / 8, w / 4, h / 2);

	SDL_FRect more = createRect(w * 3 / 4, h * 3 / 4, w / 4, h / 8);
	SDL_FRect less = createRect(w * 3 / 4, h * 7 / 8, w / 4, h / 8);

	SDL_FRect view = createRect(w / 4, h * 3 / 4, w / 4, h / 8);
	SDL_FRect scoreRect = createRect(w / 24, h * 23 / 24, w / 12, h / 12);

	
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &moreImg);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &lessImg);

	SDL_SetRenderDrawColor(renderer, 0, 83, 10, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &more);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderRect(renderer, &more);
	SDL_SetRenderDrawColor(renderer, 8, 39, 245, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &less);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderRect(renderer, &less);

	SDL_RenderFillRect(renderer, &view);
	SDL_SetRenderDrawColor(renderer, 197, 179, 88, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &scoreRect);

	int x = 0;
	int y = 0;
	get_text_center(more, moreTxt, &x, &y);

	if (state == moreWrong || state == moreRight) {
		TTF_SetTextColor(moreTxt, 255, 233, 0, SDL_ALPHA_OPAQUE);
	}
	else {
		TTF_SetTextColor(moreTxt, 255, 255, 255, SDL_ALPHA_OPAQUE);
	}

	if (!TTF_DrawRendererText(moreTxt, (float)x, (float)y)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		exit(-1);
	}

	get_text_center(less, lessTxt, &x, &y);


	if (state == lessWrong || state == lessRight) {
		TTF_SetTextColor(lessTxt, 255, 233, 0, SDL_ALPHA_OPAQUE);
	}
	else {
		TTF_SetTextColor(lessTxt, 255, 255, 255, SDL_ALPHA_OPAQUE);
	}

	if (!TTF_DrawRendererText(lessTxt, (float)x, (float)y)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		exit(-1);
	}

	if (state != normal) {
		TTF_SetTextColor(views, 255, 233, 0, SDL_ALPHA_OPAQUE);
	}
	else {
		TTF_SetTextColor(views, 255, 255, 255, SDL_ALPHA_OPAQUE);
	}

	get_text_center(view, views, &x, &y);

	if (!TTF_DrawRendererText(views, (float)x, (float)y)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		exit(-1);
	}

	if (state == lessWrong || state == moreWrong) {
		TTF_SetTextColor(txtScore, 150, 0, 24, SDL_ALPHA_OPAQUE);
	}
	else if (state == lessRight || state == moreRight) {
		TTF_SetTextColor(txtScore, 21, 71, 52, SDL_ALPHA_OPAQUE);
	}
	else {
		TTF_SetTextColor(txtScore, 255, 255, 255, SDL_ALPHA_OPAQUE);
	}
	get_text_center(scoreRect, txtScore, &x, &y);

	if(!TTF_DrawRendererText(txtScore, (float)x, (float)y)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		exit(-1);
	}

	SDL_RenderTexture(renderer, queue->front->img, NULL, &moreImg);
	YTNode* nextThing = queue->front->next;
	if (nextThing != NULL) {
		SDL_RenderTexture(renderer, nextThing->img, NULL, &lessImg);
	}
}

void draw(TTF_Text* more, TTF_Text* less, Queue* queue, int score, int state) {
	// Clears the renderer
	static bool firstIter = true;
	SDL_SetRenderDrawColor(renderer, 128, 0, 32, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	drawMoreOrLess(more, less, queue, score, state);
	if (firstIter) {
		SDL_ShowWindow(window);
		firstIter = false;
	}

	SDL_RenderPresent(renderer);
}