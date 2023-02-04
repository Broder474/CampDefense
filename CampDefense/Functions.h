#include <SDL.h>
#include <SDL_image.h>

SDL_Texture* CreateTextureFromFile(SDL_Renderer* renderer, const char* file);
SDL_Point GetTextureSize(SDL_Texture* texture);