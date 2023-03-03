#include "Functions.h"
#include <iostream>

SDL_Texture* CreateTextureFromFile(SDL_Renderer* renderer, const char* file)
{
	SDL_Surface* surface = IMG_Load(file);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

SDL_Point GetTextureSize(SDL_Texture* texture)
{
	SDL_Point size;
	SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
	return size;
}

std::string FloatToStr(float num, int sym)
{
	std::string str = std::to_string(num);
	auto erase_to = std::find(str.begin(), str.end(), '.');
	for (int i = 0; i < sym + 1 && erase_to != str.end(); i++)
		erase_to++;
	str.erase(erase_to, str.end());
	return str;
}

float Calc2dDistance(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void operator /= (SDL_Rect& rect, float scale)
{
	rect.x = (int)(rect.x / scale);
	rect.y = (int)(rect.y / scale);
	rect.w = (int)(rect.w / scale);
	rect.h = (int)(rect.h / scale);
}

SDL_Rect operator / (SDL_Rect rect, float scale)
{
	rect.x = (int)(rect.x / scale);
	rect.y = (int)(rect.y / scale);
	rect.w = (int)(rect.w / scale);
	rect.h = (int)(rect.h / scale);
	return rect;
}

void operator /= (SDL_FRect& rect, float scale)
{
	rect.x = (rect.x / scale);
	rect.y = (rect.y / scale);
	rect.w = (rect.w / scale);
	rect.h = (rect.h / scale);
}

SDL_FRect operator / (SDL_FRect rect, float scale)
{
	rect.x = (rect.x / scale);
	rect.y = (rect.y / scale);
	rect.w = (rect.w / scale);
	rect.h = (rect.h / scale);
	return rect;
}