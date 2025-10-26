#include "commons.h"
#include "font.h"
#include "server.h"
#include "draw.h"
#include "globals.h"


Queue* createQueue() {
	Queue* ytData = malloc(sizeof(Queue));
	if (ytData == NULL) {
		exit(1);
	}
	ytData->front = NULL;
	ytData->back = NULL;
	return ytData;
}

void deleteQueue(Queue* queue) {
	while (queue->front != NULL) {
		YTNode* oldFront = queue->front;
		queue->front = queue->front->next;
		free(oldFront->filePath);
		free(oldFront->sViews);
		free(oldFront);
	}

	free(queue);
}

void deQueue(Queue* queue, YTNode* next) {
	YTNode* oldFront = queue->front;
	queue->front = next;
	free(oldFront);
}

int moreOrLess(bool more, Queue* queue, int score, int* state) {

	int viewPublic = queue->front->views;
	YTNode* nextNode = queue->front->next;
	int privateViews = nextNode->views;
	deQueue(queue, nextNode);
	if (more) {
		if (privateViews >= viewPublic) {
			if (nextNode->next == NULL) {
				return -1;
			}

			*state = moreRight;
			return score + 1;
		}
		else {
			if (nextNode->next == NULL) {
				return -1;
			}

			*state = moreWrong;
			return 0;
		}
	}
	else {
		if (privateViews <= viewPublic) {
			if (nextNode->next == NULL) {
				return -1;
			}

			*state = lessRight;
			return score + 1;
		}
		else {
			if (nextNode->next == NULL) {
				return -1;
			}

			*state = lessWrong;
			return 0;
		}
	}
}

bool isPressed(SDL_MouseButtonEvent mouse, SDL_FRect rect) {
	return mouse.x > rect.x && mouse.x < (rect.x + rect.w) && mouse.y > rect.y && mouse.y < (rect.y + rect.h);
}

/*Initializes the video, renderer, window, and font engine
needed for SDL*/
void SDL_Init_All() {

	TTF_Init();
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		//printf("Initialization of video failed: %s\n", SDL_GetError());
		exit(-1);
	}

	if (!SDL_CreateWindowAndRenderer("More Or Less", 1, 1, SDL_WINDOW_HIDDEN, &window, &renderer)) {
		//printf("Initialization of window or renderer failed %s", SDL_GetError());
		exit(-1);
	}

	textEngine = TTF_CreateRendererTextEngine(renderer);
	if (textEngine == NULL) {
		//fprintf(stderr, "%s%s", "The engine failed to load due to: ", SDL_GetError());
		exit(-1);
	}

	if (!SDL_SetWindowFullscreen(window, true)) {
		//fprintf(stderr, "%s%s", "Could not set the window to full screen", SDL_GetError());
		exit(-1);
	}
	

	screen = malloc(sizeof(Screen));
	if (screen == NULL) {
		//fprintf(stderr, "%s\n", "Malloc for screen struct failed");
		exit(-1);
	}

	SDL_DisplayMode* display = SDL_GetCurrentDisplayMode(1);
	if (display == NULL) {
		exit(-1);
	}
	screen->w = display->w;
	screen->h = display->h;

	screen->surface = SDL_CreateSurface(screen->w, screen->h, SDL_PIXELFORMAT_UNKNOWN);

	if (screen->surface == NULL) {
		exit(-1);
	}
}

int expandQueue(zsock_t* requester, Queue* queue, int counter) {
	counter++;
	if (counter % 15 == 0) {
		// Let the server know we are ready for more
		zstr_send(requester, "Roger");
		getYtData(requester, queue);
	}

	return counter;
}


int main() {
	SDL_Init_All();
	createFontArray();
	bool gameRunning = true;
	
	zsock_t* requester =  establishConnection();
	Queue* queue = createQueue();

	getYtData(requester, queue);
	TTF_Font* moreLessFont = createFont("../assets/font/stentiga.TTF", 160.0);
	SDL_FRect more = createRect(screen->w * 3 / 4, screen->h * 3 / 4, screen->w / 4, screen->h / 8);
	SDL_FRect less = createRect(screen->w * 3 / 4, screen->h * 7 / 8, screen->w / 4, screen->h / 8);
	TTF_Text* moreText = TTF_CreateText(textEngine, moreLessFont, "More", strlen("More") + 1);
	TTF_Text* lessText = TTF_CreateText(textEngine, moreLessFont, "Less", strlen("Less") + 1);
	if (moreText == NULL || lessText == NULL) {
		printf(SDL_GetError());
		exit(-1);
	}

		
	time_t timer = time(NULL);
	time_t cooldown = time(NULL);

	int score = 0;
	int counter = 0;
	int state = normal;
	bool canClick = true;
	while (gameRunning) {
		while (SDL_PollEvent(&event)) {
			time_t currentTime = time(NULL);
			if (currentTime - timer >= 1) {
				state = normal;
			}
			if (currentTime - cooldown >= 0.05) {
				canClick = true;
			}
			draw(moreText, lessText, queue, score, state);
			if (event.type == SDL_EVENT_QUIT) {
				gameRunning = false;
			}
			// Key is the code of the key press within the key struct
			if (event.type == SDL_EVENT_KEY_DOWN && canClick) {
				canClick = false;
				cooldown = time(NULL);
				if (event.key.key == SDLK_ESCAPE) {
					gameRunning = false;
				}
				else if (event.key.key == SDLK_W) {
					score = moreOrLess(true, queue, score, &state);
					counter = expandQueue(requester, queue, counter);
					timer = time(NULL);
					cooldown = time(NULL);
				}
				else if (event.key.key == SDLK_S) {
					score = moreOrLess(false, queue, score, &state);
					counter = expandQueue(requester, queue, counter);
					timer = time(NULL);
					cooldown = time(NULL);
				}
			}
			
			if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && canClick) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					canClick = false;
					cooldown = time(NULL);
					if (isPressed(event.button, more)) {
						score = moreOrLess(true, queue, score, &state);
						counter = expandQueue(requester, queue, counter);
						timer = time(NULL);
					}
					else if (isPressed(event.button, less)) {
						score = moreOrLess(false, queue, score, &state);
						counter = expandQueue(requester, queue, counter);
						timer = time(NULL);
					}
				}
			}
		}
		
		if (score == -1) {
			gameRunning = false;	
		}
	}

	zsock_destroy(&requester);
	free(screen);
	freeFontArray();
	deleteQueue(queue);
	SDL_Quit();
}