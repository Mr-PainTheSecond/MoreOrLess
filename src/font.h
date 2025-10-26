#ifndef FONT_H_
#define FONT_H_

#include "commons.h"
#include "globals.h"
void createFontArray();
char* format(char* original);
void freeFontArray();
TTF_Font* createFont(char* file_name, float size);
#endif