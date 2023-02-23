﻿#include "Game.h"
#include <fstream>

extern Game* game;

// Button class
template<class O, class M>
Button::Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::string text, float scale, O* object, M method):
	ren(ren), dstrect(dstrect), text_color(text_color), font(font), text(text), scale(scale)
{
	this->dstrect.x /= scale;
	this->dstrect.y /= scale;
	this->dstrect.w /= scale;
	this->dstrect.h /= scale;
	generateTexture();
	setOnClick(object, method);
}

Button::Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::string text, float scale, CallBack onClick):
	ren(ren), dstrect(dstrect), text_color(text_color), font(font), text(text), scale(scale), onClick(onClick)
{
	this->dstrect.x /= scale;
	this->dstrect.y /= scale;
	this->dstrect.w /= scale;
	this->dstrect.h /= scale;
	generateTexture();
}

void Button::render()
{
	SDL_RenderCopy(ren, text_texture, 0, &text_rect); // render only text
}

bool Button::isPointed(SDL_Point point)
{
	if (getVisibility() == false)
		return false;
	if (point.x > this->dstrect.x && point.x < this->dstrect.x + dstrect.w && point.y > this->dstrect.y && point.y < this->dstrect.y + dstrect.h)
		return true;
	return false;
}

void Button::generateTexture() 
{
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, (const char*)text.c_str(), text_color);
	if (text_texture != nullptr)
		SDL_DestroyTexture(text_texture);
	text_texture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);

	// remember size of text texture
	SDL_Point tex_size = { 0 };
	SDL_QueryTexture(text_texture, nullptr, nullptr, &tex_size.x, &tex_size.y);
	text_rect = {dstrect.x + (dstrect.w - tex_size.x) / 2, dstrect.y + (dstrect.h - tex_size.y) / 2, tex_size.x, tex_size.y};
};

void Button::setText(std::string text)
{
	if (this->text != text)
	{
		this->text = text;
		generateTexture();
	}
}

// Image_Button class
Image_Button::~Image_Button() {};

void Image_Button::render() 
{
	if (getVisibility() == false)
		return;
	SDL_RenderCopy(ren, bg_texture, nullptr, &dstrect);
	Button::render();
};

// Color_Button class
Color_Button::~Color_Button() {};

void Color_Button::render() 
{
	if (getVisibility() == false)
		return;
	SDL_SetRenderDrawColor(ren, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderFillRect(ren, &dstrect);

	Button::render();
};

// Image class
Image::Image(SDL_Renderer* ren, SDL_Texture* texture, SDL_Rect dstrect, float scale) : ren(ren), texture(texture), dstrect(dstrect), scale(scale)
{
	this->dstrect.x /= scale;
	this->dstrect.y /= scale;
	this->dstrect.w /= scale;
	this->dstrect.h /= scale;
};

void Image::render()
{
	if (getVisibility() == false)
		return;
	SDL_RenderCopy(ren, texture, nullptr, &dstrect);
}

// List class
List::List(SDL_Renderer* ren, SDL_Rect list_rect, SDL_Rect item_rect, SDL_Color bg_list_color, SDL_Color bg_item_color, SDL_Color selected_item_color,
	SDL_Color text_color, float scale, TTF_Font* font, int scroll_bar_width) : ren(ren), list_rect(list_rect), item_rect(item_rect), bg_list_color(bg_list_color),
	text_color(text_color), bg_item_color(bg_item_color), selected_item_color(selected_item_color), scale(scale), font(font)
{
	this->list_rect.x /= scale;
	this->list_rect.y /= scale;
	this->list_rect.w /= scale;
	this->list_rect.h /= scale;

	this->item_rect.w /= scale;
	this->item_rect.h /= scale;
	this->item_rect.x = this->list_rect.x + this->item_rect.x / scale;

	scroll_bar_color_full.r = bg_list_color.r - 10;
	scroll_bar_color_full.g = bg_list_color.g - 10;
	scroll_bar_color_full.b = bg_list_color.b - 10;

	scroll_bar_color_current.r = bg_list_color.r - 30;
	scroll_bar_color_current.g = bg_list_color.g - 30;
	scroll_bar_color_current.b = bg_list_color.b - 30;

	scroll_bar_rect_full.x = this->list_rect.x + this->list_rect.w - scroll_bar_width / scale;
	scroll_bar_rect_full.y = this->list_rect.y;
	scroll_bar_rect_full.w = scroll_bar_width / scale;
	scroll_bar_rect_full.h = this->list_rect.h;

	// since the list is empty the size of current scroll bar is equal full scroll bar
	scroll_bar_rect_current.x = scroll_bar_rect_full.x;
	scroll_bar_rect_current.y = this->list_rect.y;
	scroll_bar_rect_current.w = scroll_bar_rect_full.w;
	scroll_bar_rect_current.h = scroll_bar_rect_full.h;
};

List::~List() {};

List::ItemData::ItemData(SDL_Renderer* ren, std::string item_name, SDL_Rect item_rect, TTF_Font* font, SDL_Color text_color, float scale) :
	item_name(item_name), item_rect(item_rect)
{
	// create text texture
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, (const char*)item_name.c_str(), text_color);
	if (text_texture != nullptr)
		SDL_DestroyTexture(text_texture);
	text_texture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);

	// remember size of text texture
	SDL_Point tex_size = { 0, 0 };
	SDL_QueryTexture(text_texture, nullptr, nullptr, &tex_size.x, &tex_size.y);
	SDL_Rect text_rect = { item_rect.x + 5 / scale, item_rect.y, tex_size.x, tex_size.y};
	this->text_rect = text_rect;
};

List::ItemData::~ItemData()
{
	SDL_DestroyTexture(text_texture);
}

bool List::ItemData::isPointed(int x, int y)
{
	if (x > item_rect.x && x < item_rect.x + item_rect.w && y > item_rect.y && y < item_rect.y + item_rect.h)
		return true;
	return false;
}

void List::addItem(std::string item_name)
{
	SDL_Rect item_rect = this->item_rect;
	item_rect.y = list_rect.y + items.size() * this->item_rect.h;
	std::unique_ptr<ItemData> item(new ItemData(ren, item_name, item_rect, font, text_color, scale));
	items.push_back(std::move(item));
	total_height = items.size() * this->item_rect.h;
	total_height < this->list_rect.h ? scroll_bar_rect_current.h = this->list_rect.h : scroll_bar_rect_current.h = 
		((float)this->list_rect.h / total_height) * list_rect.h;
}

void List::removeItem(std::string item_name)
{
	bool item_found = false;
	for (auto item = items.begin(); item != items.end(); item++)
	{
		if (item->get()->item_name == item_name && item_found == false)
		{
			item_found = true;
			selected_item = nullptr;

			// move scroll bar to prevent the loss of items
			setScrollBarPointed({ scroll_bar_rect_current.x + 1, scroll_bar_rect_current.y + 1});
			scrollBarMove({ scroll_bar_rect_full.x + 1, scroll_bar_rect_full.y + 1 });

			items.erase(item);
			total_height = items.size() * this->item_rect.h;
			total_height < this->list_rect.h ? scroll_bar_rect_current.h = this->list_rect.h : scroll_bar_rect_current.h =
				((float)this->list_rect.h / total_height) * list_rect.h;
			if (item == items.end())
				break;
		}
		if (item_found == true)
		{
			item->second->item_rect.y -= item_rect.h;
			item->get()->text_rect.y -= item_rect.h;
		}
	}
}

void List::render()
{
	if (visible)
	{
		SDL_SetRenderDrawColor(ren, bg_list_color.r, bg_list_color.g, bg_list_color.b, bg_list_color.a);
		SDL_RenderFillRect(ren, &list_rect);

		for (auto item : items)
		{
			if (item->item_rect.y < list_rect.y) // if item crosses the top of list
			{
				if (item->item_rect.y + item->item_rect.h < list_rect.y)
					continue;
				SDL_Rect bg_rect = item->item_rect;
				bg_rect.y = list_rect.y;
				bg_rect.h -= list_rect.y - bg_rect.y;

				if (selected_item != item)
					SDL_SetRenderDrawColor(ren, bg_item_color.r, bg_item_color.g, bg_item_color.b, bg_item_color.a);
				else
					SDL_SetRenderDrawColor(ren, selected_item_color.r, selected_item_color.g, selected_item_color.b, selected_item_color.a);
				SDL_RenderFillRect(ren, &bg_rect);
				SDL_SetRenderDrawColor(ren, bg_item_color.r + 20, bg_item_color.g + 20, bg_item_color.b + 20, bg_item_color.a);
				SDL_RenderDrawRect(ren, &bg_rect);

				// render text
				if (item->text_rect.y > list_rect.y)
					SDL_RenderCopy(ren, item->text_texture, nullptr, &item->text_rect);
				else
				{
					SDL_Rect text_srcrect = item->text_rect;
					text_srcrect.x = 0;
					text_srcrect.y = 0;

					text_srcrect.y = list_rect.y - item->text_rect.y;
					text_srcrect.h -= text_srcrect.y;

					SDL_Rect text_dstrect = item->text_rect;
					text_dstrect.h = text_srcrect.h;
					text_dstrect.y += text_srcrect.y;

					SDL_RenderCopy(ren, item->text_texture, &text_srcrect, &text_dstrect);
				}
				continue;
			}
			else if (item->item_rect.y + item->item_rect.h > list_rect.y + list_rect.h) // if item crosses the bottom of list
			{
				SDL_Rect bg_rect = item->item_rect;
				bg_rect.h = list_rect.y + list_rect.h - bg_rect.y;

				if (selected_item != item)
					SDL_SetRenderDrawColor(ren, bg_item_color.r, bg_item_color.g, bg_item_color.b, bg_item_color.a);
				else
					SDL_SetRenderDrawColor(ren, selected_item_color.r, selected_item_color.g, selected_item_color.b, selected_item_color.a);
				SDL_RenderFillRect(ren, &bg_rect);
				SDL_SetRenderDrawColor(ren, bg_item_color.r + 20, bg_item_color.g + 20, bg_item_color.b + 20, bg_item_color.a);
				SDL_RenderDrawRect(ren, &bg_rect);

				// render text
				if (item->text_rect.y + item->text_rect.h < list_rect.y + list_rect.h)
					SDL_RenderCopy(ren, item->text_texture, nullptr, &item->text_rect);
				else
				{
					SDL_Rect text_srcrect = item->text_rect;
					text_srcrect.x = 0;
					text_srcrect.y = 0;
					text_srcrect.h = list_rect.y + list_rect.h - item->text_rect.y;
					SDL_Rect text_dstrect = item->text_rect;
					text_dstrect.h = text_srcrect.h;

					SDL_RenderCopy(ren, item->text_texture, &text_srcrect, &text_dstrect);
				}
				break;
			}

			if (selected_item != item)
				SDL_SetRenderDrawColor(ren, bg_item_color.r, bg_item_color.g, bg_item_color.b, bg_item_color.a);
			else
				SDL_SetRenderDrawColor(ren, selected_item_color.r, selected_item_color.g, selected_item_color.b, selected_item_color.a);
			SDL_RenderFillRect(ren, &item->item_rect);
			SDL_SetRenderDrawColor(ren, bg_item_color.r + 20, bg_item_color.g + 20, bg_item_color.b + 20, bg_item_color.a);
			SDL_RenderDrawRect(ren, &item->item_rect);
			SDL_RenderCopy(ren, item->text_texture, nullptr, &item->text_rect);
		}

		// render scroll bar
		SDL_SetRenderDrawColor(ren, scroll_bar_color_full.r, scroll_bar_color_full.g, scroll_bar_color_full.b, scroll_bar_color_full.a);
		SDL_RenderFillRect(ren, &scroll_bar_rect_full);

		SDL_SetRenderDrawColor(ren, scroll_bar_color_current.r, scroll_bar_color_current.g, scroll_bar_color_current.b, scroll_bar_color_current.a);
		SDL_RenderFillRect(ren, &scroll_bar_rect_current);
	}
}

void List::setScrollBarPointed(SDL_Point point)
{
	if (point.x > scroll_bar_rect_current.x && point.x < scroll_bar_rect_current.x + scroll_bar_rect_current.w && point.y > scroll_bar_rect_current.y &&
		point.y < scroll_bar_rect_current.y + scroll_bar_rect_current.h)
	{
		is_scroll_bar_pointed = true;
		scroll_bar_click = point;
	}
	else
		is_scroll_bar_pointed = false;
}

void List::setScrollBarPointed(bool is_scroll_bar_pointed)
{
	this->is_scroll_bar_pointed = is_scroll_bar_pointed;
}

void List::scrollBarMove(SDL_Point point)
{
		int shift = point.y - scroll_bar_click.y; // scroll bar shift
		if (scroll_bar_rect_current.y + shift >= scroll_bar_rect_full.y && scroll_bar_rect_current.y + scroll_bar_rect_current.h + shift <= 
			scroll_bar_rect_full.y + scroll_bar_rect_full.h)
		{
			scroll_bar_rect_current.y += shift;
			int items_shift = shift / (float)scroll_bar_rect_full.h * total_height;
			for (auto item : items)
			{
				item->text_rect.y -= items_shift;
				item->item_rect.y -= items_shift;
			}
			setScrollBarPointed(point);
		}
		// if cursor is outside scroll bar area then stop scroll bar moving
		else if (point.y > scroll_bar_rect_full.y + scroll_bar_rect_full.h && point.y < scroll_bar_rect_full.y)
			setScrollBarPointed(false);
}

std::string List::getClickedItem(SDL_Point point)
{
	if (point.x > list_rect.x && point.x < list_rect.x + list_rect.w && point.y > list_rect.y && point.y < list_rect.y + list_rect.h)
		for (auto item : items)
			if (point.x > item->item_rect.x && point.x < item->item_rect.x + item->item_rect.w && point.y > item->item_rect.y &&
				point.y < item->item_rect.y + item->item_rect.h)
			{
				selected_item = item;
				return selected_item->item_name;
			}
	return "";
}

// TextOutput class
TextOutput::TextOutput(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color) : ren(ren), font(font), 
text(text), scale(scale), x(x), y(y), text_color(text_color) 
{
	this->x /= scale;
	this->y /= scale;
};

TextOutput::TextOutput(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color) : 
	ren(ren), font(font), text(text), scale(scale), x(x), y(y), text_color(text_color), bg_color(bg_color) 
{
	this->x /= scale;
	this->y /= scale;
};

// TextOutputSingleLine class
TextOutputSingleLine::TextOutputSingleLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color) :
	TextOutput(ren, font, text, scale, x, y, text_color) 
{
	generateTexture(0);
};

TextOutputSingleLine::TextOutputSingleLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, 
	SDL_Color bg_color) : TextOutput(ren, font, text, scale, x, y, text_color, bg_color) 
{
	generateTexture(1);
};

TextOutputSingleLine::~TextOutputSingleLine() 
{
	SDL_DestroyTexture(text_texture);
};

void TextOutputSingleLine::generateTexture(int type)
{

	SDL_Surface* surface = nullptr;
	if (type == 0)
		surface = TTF_RenderUTF8_Blended(font, (const char*)text.c_str(), text_color);
	else if (type == 1)
		surface = TTF_RenderUTF8_Shaded(font, (const char*)text.c_str(), text_color, bg_color);
	if (text_texture != nullptr)
		SDL_DestroyTexture(text_texture);
	text_texture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);

	// remember texture size
	SDL_Point texture_size = { 0, 0 };
	SDL_QueryTexture(text_texture, nullptr, nullptr, &texture_size.x, &texture_size.y);
	text_dstrect = { x, y, texture_size.x, texture_size.y };
}

void TextOutputSingleLine::render()
{
	if (getVisibility() == false)
		return;
	SDL_RenderCopy(ren, text_texture, nullptr, &text_dstrect);
}

// TextOutputMultiline class
TextOutputMultiLine::TextOutputMultiLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, int width) :
	TextOutput(ren, font, text, scale, x, y, text_color), width(width) 
{
	this->width /= scale;
	generateTexture(0);
};

TextOutputMultiLine::TextOutputMultiLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color, int width) :
	TextOutput(ren, font, text, scale, x, y, text_color, bg_color), width(width) 
{
	this->width /= scale;
	generateTexture(1);
};

TextOutputMultiLine::~TextOutputMultiLine() 
{
	SDL_DestroyTexture(text_texture);
};

void TextOutputMultiLine::generateTexture(int type)
{
	SDL_Surface* surface = nullptr;
	if (type == 0)
		surface = TTF_RenderUTF8_Blended_Wrapped(font, (const char*)text.c_str(), text_color, width);
	else if (type == 1)
		surface = TTF_RenderUTF8_Shaded_Wrapped(font, (const char*)text.c_str(), text_color, bg_color, width);
	text_texture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);
	SDL_Point texture_size = { 0, 0 };
	SDL_QueryTexture(text_texture, nullptr, nullptr, &texture_size.x, &texture_size.y);
	text_dstrect = { x, y, texture_size.x, texture_size.y };
}

void TextOutputMultiLine::render()
{
	if (getVisibility() == false)
		return;
	SDL_RenderCopy(ren, text_texture, nullptr, &text_dstrect);
}

// WinMainMenu class
WinMainMenu::WinMainMenu(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings):
	Window(ren, windows, res, settings)
{
	std::unique_ptr<Image_Button> btnNewGame(new Image_Button(ren, { 810, 470, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnNewGame"], settings->getScale(), [&]() {
			std::unique_ptr<WinGame>winGame(new WinGame(this->ren, this->windows, this->resources, this->settings));
			this->windows.clear();
			this->windows.push_back(std::move(winGame));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnNewGame));

	std::unique_ptr<Image_Button> btnLoadGame(new Image_Button(ren, { 810, 540, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnLoadGame"], settings->getScale(), [&]() {
			std::unique_ptr<WinLoadGame>winLoadGame(new WinLoadGame(this->ren, this->windows, this->resources, this->settings));
			this->windows.push_back(std::move(winLoadGame));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnLoadGame));

	std::unique_ptr<Image_Button> btnSettings(new Image_Button(ren, { 810, 610, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnSettings"], settings->getScale(), [&]() {	
			std::unique_ptr<WinSettings>winSettings(new WinSettings(this->ren, this->windows, this->resources, this->settings));
			this->windows.push_back(std::move(winSettings));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnSettings));

	std::unique_ptr<Image_Button> btnExit(new Image_Button(ren, { 810, 680, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnExit"], settings->getScale(), [&]() {windows.clear(); }, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnExit));
};

WinMainMenu::~WinMainMenu() {};

void WinMainMenu::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	SDL_Point point_click{ event.button.x, event.button.y };

	switch (event.type)
	{
	case SDL_QUIT:
	{
		windows.clear();
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			for (auto& btn : buttons)
				if (btn->isPointed(point_click))
					btn->onClick(); 
			break;
		}
		break;
	}
	default:
		break;
	}
};

void WinMainMenu::render()
{
	SDL_SetRenderDrawColor(ren, 40, 30, 70, 255);
	SDL_RenderClear(ren);

	for (auto& btn : buttons)
		btn->render();
};

// WinGame class
WinGame::WinGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings) : 
	Window(ren, windows, res, settings)
{
	world = new World(0, 6);
	game->world = this->world;
	for (int i = 0; i < 1 + rand() % 3; i++)
	{
		int gender = rand() % 2;
		unsigned int health = 50 + rand() % 11 * 10;
		if (gender == Character::Male)
			health += 10;
		float speed_per_tact = 0.8f + (float)(rand() % 41) / 100; // min - 0.8f, max - 1.2f
		unsigned int strength = 5; // minimum melee attack
		unsigned int chance = rand() % 101;
		if (chance > 90)
			strength += rand() % 41;
		else if (chance > 66)
			strength += rand() % 31;
		else if (chance > 33)
			strength += rand() % 21;
		else
			strength += rand() % 16;
		unsigned int consumption = 5 + (health - 50) * 0.05f; // min - 5, max - 10
		std::unique_ptr<Character> character(new Character(gender, health, speed_per_tact, strength, res, nullptr,
			consumption, (unsigned int)0, (unsigned int)0, (unsigned int)0, (unsigned int)0));
		world->entities.push_back(std::move(character));
	}

	// gui initialization
	std::unique_ptr<Image>bg_image(new Image(ren, res->gui_textures["background2"].texture, res->gui_textures["background2"].dstrect, 1.0f));
	images.push_back(std::move(bg_image));

	std::string day = res->lang["WinGame.day"];
	day += " " + std::to_string(world->getDay());
	TextOutputSingleLine* text_day = new TextOutputSingleLine(ren, res->fonts["calibri32"], day, settings->getScale(), 5, 5, { 60, 60, 200, 0 });
	texts["day"].reset(text_day);

	std::string hour = std::to_string(world->getHour()) + ":00";
	TextOutputSingleLine* text_hour = new TextOutputSingleLine(ren, res->fonts["calibri32"], hour, settings->getScale(), 5, 45, { 60, 60, 200, 0 });
	texts["hour"].reset(text_hour);

	std::string materials = res->lang["WinGame.materials"];
	materials += ": " + std::to_string(world->getMaterials());
	TextOutputSingleLine* text_materials = new TextOutputSingleLine(ren, res->fonts["calibri32"], materials, settings->getScale(), 5, 85, { 60, 60, 200, 0 });
	texts["materials"].reset(text_materials);

	std::string products = res->lang["WinGame.products"];
	products += ": " + std::to_string(world->getProducts());
	TextOutputSingleLine* text_products = new TextOutputSingleLine(ren, res->fonts["calibri32"], products, settings->getScale(), 5, 125, { 60, 60, 200, 0 });
	texts["products"].reset(text_products);
	
	Image_Button* btnActions(new Image_Button(ren, { 0, 1020, 300, 60 }, { 60, 60, 200, 0 }, res->fonts["calibri32"],
		res->lang["WinGame.btnActions"], settings->getScale(), this, &WinGame::BtnActions, res->gui_textures["button1"].texture));
	buttons["Actions"].reset(btnActions);
};

WinGame::~WinGame() {};

void WinGame::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	SDL_Point point_click{ event.button.x, event.button.y };

	switch (event.type)
	{
	case SDL_QUIT:
		windows.clear();
		break;
	case SDL_KEYDOWN:
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			std::unique_ptr<WinGameMenu>winSaveGame(new WinGameMenu(ren, windows, resources, settings));
			windows.push_back(std::move(winSaveGame));
			break;
		}
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			for (auto& btn : buttons)
				if (btn.second->isPointed(point_click))
					btn.second->onClick();
			break;
		}
		break;
	}
	}
	if (SDL_GetTicks64() >= update_timer && update_status == true)
		update();
}

void WinGame::update()
{
	update_timer = SDL_GetTicks64() + 50;
	world->update();

	std::string hour = std::to_string(world->getHour()) + ":00";
	if (texts["hour"]->getText() != hour)
	{
		texts["hour"]->setText(hour);
		texts["hour"]->generateTexture(0);
	}
	std::string day = resources->lang["WinGame.day"];
	day += " " + std::to_string(world->getDay());
	if (texts["day"]->getText() != day)
	{
		texts["day"]->setText(day);
		texts["day"]->generateTexture(0);
	}
	std::string materials = resources->lang["WinGame.materials"];
	materials += ": " + std::to_string(world->getMaterials());
	if (texts["materials"]->getText() != materials)
	{
		texts["materials"]->setText(materials);
		texts["materials"]->generateTexture(0);
	}
	std::string products = resources->lang["WinGame.products"];
	products += ": " + std::to_string(world->getProducts());
	if (texts["products"]->getText() != products)
	{
		texts["products"]->setText(products);
		texts["products"]->generateTexture(0);
	}
}

void WinGame::render()
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);

	for (auto& image : images)
		image->render();

	for (auto& btn : buttons)
		btn.second->render();

	for (auto& text : texts)
		text.second->render();
}

void WinGame::BtnActions()
{
	
}

// class WinGameMenu
WinGameMenu::WinGameMenu(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings) :
	Window(ren, windows, res, settings)
{
	std::unique_ptr<Image_Button>btnSaveGame(new Image_Button(ren, { 810, 420, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinGameMenu.btnSaveGame"], settings->getScale(), [&]() {
			std::unique_ptr<WinSaveGame>winSaveGame(new WinSaveGame(this->ren, this->windows, this->resources, this->settings));
			this->windows.push_back(std::move(winSaveGame));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnSaveGame));

	std::unique_ptr<Image_Button>btnLoadGame(new Image_Button(ren, { 810, 500, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinGameMenu.btnLoadGame"], settings->getScale(), [&]() {
			std::unique_ptr<WinLoadGame>winLoadGame(new WinLoadGame(this->ren, this->windows, this->resources, this->settings));
			this->windows.push_back(std::move(winLoadGame));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnLoadGame));

	std::unique_ptr<Image_Button>btnMainMenu(new Image_Button(ren, { 810, 580, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinGameMenu.btnMainMenu"], settings->getScale(), [&]() {
			std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(this->ren, this->windows, this->resources, this->settings));
			this->windows.clear();
			this->windows.push_back(std::move(winMainMenu));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnMainMenu));

	std::unique_ptr<Image>bg_image(new Image(ren, res->gui_textures["background1"].texture, { 260, 140, 1400, 800 }, settings->getScale()));
	images.push_back(std::move(bg_image));
}

WinGameMenu::~WinGameMenu() {};

void WinGameMenu::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	SDL_Point point_click{ event.button.x, event.button.y };

	switch (event.type)
	{
	case SDL_QUIT:
		windows.clear();
		break;
	case SDL_MOUSEBUTTONDOWN:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			for (auto& btn : buttons)
				if (btn->isPointed(point_click))
					btn->onClick();
			break;
		}
		break;
	}
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			windows.pop_back();
			break;
		}
	}
}

void WinGameMenu::render()
{
	for (auto& image : images)
		image->render();

	for (auto& btn : buttons)
		btn->render();
}

// class WinSaveGame
WinSaveGame::WinSaveGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings):
	Window(ren, windows, res, settings)
{
	std::unique_ptr<Image_Button>btnSaveGame(new Image_Button(ren, { 605, 820, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinSaveGame.btnSaveGame"], settings->getScale(), this, & WinSaveGame::BtnSaveGame, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnSaveGame));

	std::unique_ptr<Image_Button>btnDeleteGame(new Image_Button(ren, { 1115, 820, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinSaveGame.btnDeleteGame"], settings->getScale(), this, & WinSaveGame::BtnDeleteSave, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnDeleteGame));

	std::unique_ptr<Image>bg_image(new Image(ren, res->gui_textures["background1"].texture, { 260, 140, 1400, 800 }, settings->getScale()));
	images.push_back(std::move(bg_image));
};

WinSaveGame::~WinSaveGame() {};

void WinSaveGame::handleEvents() 
{
	SDL_Event event;
	SDL_PollEvent(&event);
	SDL_Point point_click{ event.button.x, event.button.y };

	switch (event.type)
	{
	case SDL_QUIT:
	{
		windows.clear();
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			for (auto& btn : buttons)
				if (btn->isPointed(point_click))
					btn->onClick();
			break;
		}
		break;
	}
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			windows.pop_back();
			break;
		}
	}
};

void WinSaveGame::render() 
{
	for (auto& image : images)
		image->render();
	for (auto& btn : buttons)
		btn->render();
};

void WinSaveGame::BtnSaveGame() {};

void WinSaveGame::BtnDeleteSave() {};

// class WinLoadGame
WinLoadGame::WinLoadGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings) :
	Window(ren, windows, res, settings)
{
	std::unique_ptr<Image_Button>btnLoadGame(new Image_Button(ren, { 560, 900, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinLoadGame.btnLoadGame"], settings->getScale(), this, &WinLoadGame::BtnLoadGame, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnLoadGame));

	std::unique_ptr<Image_Button>btnDeleteGame(new Image_Button(ren, { 1060, 900, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinLoadGame.btnDeleteGame"], settings->getScale(), this, &WinLoadGame::BtnDeleteSave, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnDeleteGame));

	List* saves_list = new List(ren, { 460, 70, 1000, 820 }, { 0, 0, 970, 100 }, { 170, 125, 125, 0 }, { 110, 110, 180, 0 }, { 100, 100, 100, 0 }, 
		{ 50, 50, 50, 0}, settings->getScale(), res->fonts["calibri32"], 30);
	this->saves_list.reset(saves_list);

	LoadSaves();
};

WinLoadGame::~WinLoadGame() {};

void WinLoadGame::handleEvents() 
{
	SDL_Event event;
	SDL_PollEvent(&event);
	SDL_Point point_click{ event.button.x, event.button.y };

	switch (event.type)
	{
	case SDL_QUIT:
	{
		windows.clear();
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			saves_list->setScrollBarPointed(point_click);
			saves_list->getClickedItem(point_click);

			for (auto& btn : buttons)
				if (btn->isPointed(point_click))
					btn->onClick();
			break;
		}
		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			if (saves_list->getVisibility())
				saves_list->setScrollBarPointed(false);
			break;
		}
	}
	case SDL_MOUSEMOTION:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			if (saves_list->getVisibility())
				if (saves_list->getScrollBarPointed())
					saves_list->scrollBarMove(point_click);
		}
	}
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			windows.pop_back();
			break;
		}
	}
};

void WinLoadGame::render() 
{
	SDL_SetRenderDrawColor(ren, 40, 30, 70, 255);
	SDL_RenderClear(ren);

	saves_list->render();
	for (auto& btn : buttons)
		btn->render();
};

void WinLoadGame::BtnLoadGame() {};

void WinLoadGame::LoadSaves()
{
	fs::path saves_dir = fs::current_path().string() + "/saves";
	if (!fs::exists(saves_dir))
		fs::create_directory(saves_dir);
	for (auto save_path : fs::directory_iterator(saves_dir))
		if (save_path.path().extension() == ".zomb")
			saves_list->addItem(save_path.path().stem().string());
}

void WinLoadGame::BtnDeleteSave() 
{
	if (saves_list->selected_item != nullptr)
		if (saves_list->selected_item->item_name != "")
		{
			std::unique_ptr<WinDelSave>winDelSave(new WinDelSave(ren, windows, resources, settings, saves_list));
			windows.push_back(std::move(winDelSave));
		}
}

// class WinDelSave
WinDelSave::WinDelSave(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings,
	std::unique_ptr<List>& saves_list): Window(ren, windows, res, settings), saves_list(saves_list)
{
	std::unique_ptr<Image_Button>btnDelete(new Image_Button(ren, { 560, 900, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinDelSave.btnDelete"], settings->getScale(), this, &WinDelSave::BtnDelete, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnDelete));

	std::unique_ptr<Image_Button>btnCancel(new Image_Button(ren, { 1060, 900, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinDelSave.btnCancel"], settings->getScale(), [&]() {
			this->windows.pop_back();
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnCancel));

	std::unique_ptr<TextOutputMultiLine>textAreYouSure(new TextOutputMultiLine(ren, res->fonts["calibri32"],
		res->lang["WinDelSave.textAreYouSure"], settings->getScale(), 460, 400, {0, 0, 255, 0}, {200, 128, 0, 200}, 1000));
	static_text.push_back(std::move(textAreYouSure));
}

WinDelSave::~WinDelSave() {};

void WinDelSave::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	SDL_Point point_click{ event.button.x, event.button.y };

	switch (event.type)
	{
	case SDL_QUIT:
	{
		windows.clear();
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			for (auto& btn : buttons)
				if (btn->isPointed(point_click))
					btn->onClick();
			break;
		}
		break;
	}
	}
};

void WinDelSave::render()
{
	for (auto& text : static_text)
		text->render();

	for (auto& btn : buttons)
		btn->render();
};

void WinDelSave::BtnDelete()
{
	fs::path path = "saves/" + saves_list->selected_item->item_name + ".zomb";
	fs::remove(path);
	saves_list->removeItem(saves_list->selected_item->item_name);
	windows.pop_back();
}

// class WinSettings
WinSettings::WinSettings(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings) :
	Window(ren, windows, res, settings)
{
	std::unique_ptr<Image_Button>btnSave(new Image_Button(ren, { 810, 900, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinSettings.btnSave"], settings->getScale(), this, &WinSettings::BtnSave, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnSave));

	List* lang_list = new List(ren, { 200, 160, 300, 300 }, { 0, 0, 300, 60 }, { 200, 128, 128, 0 }, { 170, 128, 128, 0 }, { 170, 170, 128, 0 },
		{ 90, 90, 140, 0 }, settings->getScale(), res->fonts["calibri32"], 20);
	this->lang_list.reset(lang_list);
	for (auto& lang : res->lang_list)
		lang_list->addItem(lang.second);
	lang_list->setVisibility(false);

	Image_Button* btnLang(new Image_Button(ren, { 200, 100, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"], res->lang_list[settings->getLang()],
		settings->getScale(), [&]() {
			this->lang_list->setVisibility(!this->lang_list->getVisibility());
		}, res->gui_textures["button2"].texture));
	this->btnLang.reset(btnLang);

	std::unique_ptr<TextOutputSingleLine>textLang(new TextOutputSingleLine(ren, res->fonts["calibri32"], res->lang["WinSettings.textLang"],
		settings->getScale(), 200, 60, { 90, 90, 140, 0 }));
	static_text.push_back(std::move(textLang));
}

WinSettings::~WinSettings() {};

void WinSettings::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	SDL_Point point_click{ event.button.x, event.button.y };

	switch (event.type)
	{
	case SDL_QUIT:
	{
		windows.clear();
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			lang_list->setScrollBarPointed(point_click);
			std::string selected_lang = lang_list->getClickedItem(point_click);
			if (selected_lang != "")
				btnLang->setText(selected_lang);
			if (btnLang->isPointed(point_click))
				btnLang->onClick();

			for (auto& btn : buttons)
				if (btn->isPointed(point_click))
					btn->onClick();
			break;
		}
		break;
	}
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			windows.pop_back();
			break;
		}
	}
}

void WinSettings::render()
{
	SDL_SetRenderDrawColor(ren, 40, 30, 70, 255);
	SDL_RenderClear(ren);

	lang_list->render();
	btnLang->render();

	for (auto& btn : buttons)
		btn->render();

	for (auto& text : static_text)
		text->render();
}

void WinSettings::BtnSave()
{
	std::string full_lang = btnLang->getText();
	for (auto& it : resources->lang_list)
		if (full_lang == it.second)
		{
			settings->setLang(it.first);
			break;
		}
	
	// settings saving
	json settings = this->settings->convert_to_json();
	game->saveSettings(&settings);

	// resources loading
	game->loadResources();

	std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(ren, windows, game->resources, this->settings));
	windows.clear();
	windows.push_back(std::move(winMainMenu));
}