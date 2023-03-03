#include <string>
#include <SDL.h>
#include <SDL_image.h>

SDL_Texture* CreateTextureFromFile(SDL_Renderer* renderer, const char* file);
SDL_Point GetTextureSize(SDL_Texture* texture);
std::string FloatToStr(float num, int sym);
float Calc2dDistance(float x1, float y1, float x2, float y2);

void operator /= (SDL_Rect& rect, float scale);
SDL_Rect operator / (SDL_Rect rect, float scale);
void operator /= (SDL_FRect& rect, float scale);
SDL_FRect operator / (SDL_FRect rect, float scale);