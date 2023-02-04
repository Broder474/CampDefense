#include <string>
#include <vector>
#include <memory>
#include <map>
#include <SDL.h>
#include <SDL_ttf.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct TextureData
{
	SDL_Texture* texture;
	SDL_Rect dstrect = { 0, 0, 0, 0 };
};

class Resources
{
public:
	Resources(SDL_Renderer* ren, float scale, const char* weapons_file, const char* gui_file);
	~Resources() {};

	std::map<std::string, TTF_Font*> fonts;
	json weapons_data;
	std::map<std::string, TextureData> weapons_textures;
	json entities_data;
	std::map<std::string, TextureData> entities_textures;
	json gui_data;
	std::map<std::string, TextureData> gui_textures;
private:

	SDL_Renderer* ren;
	SDL_Point* display;
};