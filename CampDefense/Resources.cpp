#include <fstream>
#include "Functions.h"
#include "Resources.h"

Resources::Resources(SDL_Renderer* ren, Settings* settings, const char* weapons_file, const char* gui_file, const char* lang_file) : ren(ren), display(display)
{
	// fonts loading
	TTF_Font* calibri24(TTF_OpenFont("resources/fonts/calibri.ttf", 24 / settings->getScale()));
	fonts["calibri24"] = calibri24;
	TTF_Font* calibri32(TTF_OpenFont("resources/fonts/calibri.ttf", 32 / settings->getScale()));
	fonts["calibri32"] = calibri32;
	TTF_Font* calibri64(TTF_OpenFont("resources/fonts/calibri.ttf", 64 / settings->getScale()));
	fonts["calibri64"] = calibri64;

	// lang loading
	std::ifstream in_lang(lang_file);
	bool lang_loaded = false;
	if (in_lang.is_open())
		if (bool is_file_correct = json::accept(in_lang))
		{
			in_lang.close();
			in_lang.open(lang_file);
			lang = json::parse(in_lang);
			in_lang.close();
			lang_loaded = true;
		}
	// loading full names of langs for settings
	std::string lang_list_file = "resources/data/lang_list.json";
	std::ifstream in_lang_list(lang_list_file);
	if (in_lang_list.is_open())
		if (bool is_file_correct = json::accept(in_lang_list))
		{
			in_lang_list.close();
			in_lang_list.open(lang_list_file);
			json lang_list = json::parse(in_lang_list);
			in_lang_list.close();
			for (int i = 0; i < lang_list["langs"].size(); i++)
			{
				if (lang_list["langs"][i].is_string())
				{
					std::string lang_short = lang_list["langs"][i];
					if (lang_list[lang_short].is_string())
						lang_list[lang_short].get_to(this->lang_list[lang_short]);
				}
			}
		}

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
		weapons_textures[name_tex_simple].dstrect.w /= settings->getScale();
		weapons_textures[name_tex_simple].dstrect.h /= settings->getScale();

		std::string file_tex_fire = weapons_data[weapon]["texture_fire"];
		SDL_Texture* texture_fire = CreateTextureFromFile(ren, file_tex_fire.c_str());
		std::string name_tex_fire = weapon + " F";
		weapons_textures[name_tex_fire].texture = texture_fire;
		SDL_QueryTexture(texture_fire, nullptr, nullptr, &weapons_textures[name_tex_fire].dstrect.w, &weapons_textures[name_tex_fire].dstrect.h);
		weapons_textures[name_tex_fire].dstrect.w /= settings->getScale();
		weapons_textures[name_tex_fire].dstrect.h /= settings->getScale();
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
		gui_textures[gui_object].dstrect.w /= settings->getScale();
		gui_textures[gui_object].dstrect.h /= settings->getScale();
	}

}