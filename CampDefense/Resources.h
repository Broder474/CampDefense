#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <nlohmann/json.hpp>
#include "Settings.h"

using json = nlohmann::json;

struct TextureData
{
	~TextureData() {SDL_DestroyTexture(texture); }
	SDL_Texture* texture = nullptr;
	SDL_Rect dstrect = { 0, 0, 0, 0 };
};

class Resources
{
public:
	Resources(SDL_Renderer* ren, Settings* settings, const char* weapons_file, const char* gui_file, const char* lang_file);
	~Resources() 
	{
		for (auto it = fonts.begin(); it != fonts.end(); it++)
			TTF_CloseFont(it->second);
	};

	// fonts resources
	std::map<std::string, TTF_Font*> fonts;

	// lang resources
	json lang;
	std::map<std::string, std::string> lang_list;

	// weapons resources
	json weapons_data;
	std::map<std::string, TextureData> weapons_textures;

	// entities resources
	json entities_data;
	std::map<std::string, TextureData> entities_textures;

	// gui resources
	json gui_data;
	std::map<std::string, TextureData> gui_textures;
private:

	SDL_Renderer* ren;
	SDL_Point* display;
};