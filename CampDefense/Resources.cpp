#include <fstream>
#include "Functions.h"
#include "Resources.h"

Resources::Resources(SDL_Renderer* ren, float scale, const char* weapons_file, const char* gui_file) : ren(ren), display(display)
{
	// fonts loading
	TTF_Font* calibri24(TTF_OpenFont("resources/fonts/calibri.ttf", 24 / scale));
	fonts["calibri24"] = calibri24;
	TTF_Font* calibri64(TTF_OpenFont("resources/fonts/calibri.ttf", 64 / scale));
	fonts["calibri64"] = calibri64;

	// read json for weapons
	std::ifstream in_weapons(weapons_file);
	if (in_weapons.is_open())
	{
		weapons_data = json::parse(in_weapons);
		in_weapons.close();
	}
	// loading weapons textures
	for (int i = 0; i < weapons_data["samples"].size(); i++)
	{
		std::string weapon = weapons_data["samples"][i];
		std::string file_tex_simple = weapons_data[weapon]["texture_simple"];
		SDL_Texture* texture_simple = CreateTextureFromFile(ren, file_tex_simple.c_str());
		std::string name_tex_simple = weapon + " S";
		weapons_textures[name_tex_simple].texture = texture_simple;
		SDL_QueryTexture(texture_simple, nullptr, nullptr, &weapons_textures[name_tex_simple].dstrect.w, &weapons_textures[name_tex_simple].dstrect.h);
		weapons_textures[name_tex_simple].dstrect.w /= scale;
		weapons_textures[name_tex_simple].dstrect.h /= scale;

		std::string file_tex_fire = weapons_data[weapon]["texture_fire"];
		SDL_Texture* texture_fire = CreateTextureFromFile(ren, file_tex_fire.c_str());
		std::string name_tex_fire = weapon + " F";
		weapons_textures[name_tex_fire].texture = texture_fire;
		SDL_QueryTexture(texture_fire, nullptr, nullptr, &weapons_textures[name_tex_fire].dstrect.w, &weapons_textures[name_tex_fire].dstrect.h);
		weapons_textures[name_tex_fire].dstrect.w /= scale;
		weapons_textures[name_tex_fire].dstrect.h /= scale;
	}

	// read json for gui
	std::ifstream in_gui(gui_file);
	if (in_gui.is_open())
	{
		gui_data = json::parse(in_gui);
		in_gui.close();
	}
	// loading gui textures
	for (int i = 0; i < gui_data["samples"].size(); i++)
	{
		std::string gui_object = gui_data["samples"][i];
		std::string file_gui_object = gui_data[gui_object];
		SDL_Texture* texture_object = CreateTextureFromFile(ren, file_gui_object.c_str());
		gui_textures[gui_object].texture = texture_object;
		SDL_QueryTexture(texture_object, nullptr, nullptr, &gui_textures[gui_object].dstrect.w, &gui_textures[gui_object].dstrect.h);
		gui_textures[gui_object].dstrect.w /= scale;
		gui_textures[gui_object].dstrect.h /= scale;
	}

}