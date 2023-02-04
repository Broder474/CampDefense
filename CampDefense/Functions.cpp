#include "Functions.h"

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