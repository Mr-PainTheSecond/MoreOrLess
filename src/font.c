#include "font.h"


void createFontArray() {
	fontArray = malloc(sizeof(Fonts));
	if (fontArray == NULL) {
		//fprintf(stderr, "%s\n", "Could not allocate memory for fonts");
		exit(1);
	}

	fontArray->fonts = malloc(sizeof(TTF_Font*) * 8);
	if (fontArray->fonts == NULL) {
		//fprintf(stderr, "%s\n", "Could not allocate memory for fonts");
		exit(1);
	}

	fontArray->fontIndex = 0;
	fontArray->fontSize = 8;
}

void freeFontArray() {
	for (int a = 0; a < fontArray->fontIndex; a++) {
		free(fontArray->fonts[a]);
	}

	free(fontArray);
}

char* format(char* original) {
	int dataPoints = 0;
	int dotPlace = strlen(original) - 9;
	char place = ' ';
	if (strlen(original) > 9) {
		dotPlace = strlen(original) - 9;
		dataPoints = 4;
		place = 'B';
	}
	else if (strlen(original) > 6) {
		dotPlace = strlen(original) - 6;
		dataPoints = 4;
		place = 'M';
	}
	else if (strlen(original) > 3) {
		dotPlace = strlen(original) - 3;
		dataPoints = 4;
		place = 'K';
	}
	else {
		dataPoints = strlen(original);
	}
	if (strlen(original) % 3 == 0) {
		dataPoints = 3;
		
	}
	char* newChar = malloc(sizeof(char) * dataPoints + strlen(" Views") + 2);
	if (newChar == NULL) {
		exit(1);
	}
	int a;
	int ogIndex = 0;
	for (a = 0; a < dataPoints; a++) {
		if (a == dotPlace && a + 1 < dataPoints && place != ' ') {
			newChar[a] = '.';
		}
		else {
			newChar[a] = original[ogIndex];
			ogIndex++;
		}
	}

	if (place != ' ') {
		newChar[a] = place;
		a++;
	}
	newChar[a] = '\0';
	strcat(newChar, " Views");
	return newChar;
}

TTF_Font* createFont(char* file_name, float size) {
	TTF_Font* font;
	font = TTF_OpenFont(file_name, 100);
	if (font == NULL) {
		//fprintf(stderr, "%s\n", SDL_GetError());
		exit(-1);
	}

	if (!TTF_SetFontSize(font, size)) {
		//fprintf(stderr, "%s\n", SDL_GetError());
		exit(-1);
	}

	fontArray->fonts[fontArray->fontIndex] = font;
	(fontArray->fontSize)++;

	return font;
}