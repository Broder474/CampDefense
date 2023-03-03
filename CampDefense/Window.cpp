#include "Game.h"
#include <fstream>

extern Game* game;

// Button class
template<class O, class M>
Button::Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::string text, float scale, O* object, M method):
	ren(ren), dstrect(dstrect), text_color(text_color), font(font), text(text), scale(scale)
{
	this->dstrect /= scale;
	generateTexture();
	setOnClick(object, method);
}

Button::Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::string text, float scale, CallBack onClick):
	ren(ren), dstrect(dstrect), text_color(text_color), font(font), text(text), scale(scale), onClick(onClick)
{
	this->dstrect /= scale;
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
	this->dstrect /= scale;
};

void Image::render()
{
	if (getVisibility() == false)
		return;
	SDL_RenderCopy(ren, texture, nullptr, &dstrect);
}

void Image::setTexture(SDL_Texture* texture)
{
	this->texture = texture;
	SDL_Point tex_size{ 0 };
	SDL_QueryTexture(texture, nullptr, nullptr, &tex_size.x, &tex_size.y);
	tex_size.x = (int)(tex_size.x / scale);
	tex_size.y = (int)(tex_size.y / scale);
	dstrect.w = tex_size.x;
	dstrect.h = tex_size.y;
}

// List class
List::List(SDL_Renderer* ren, SDL_Rect list_rect, SDL_Rect item_rect, SDL_Color bg_list_color, SDL_Color bg_item_color, SDL_Color selected_item_color,
	SDL_Color text_color, float scale, TTF_Font* font, int scroll_bar_width) : ren(ren), list_rect(list_rect), item_rect(item_rect), bg_list_color(bg_list_color),
	text_color(text_color), bg_item_color(bg_item_color), selected_item_color(selected_item_color), scale(scale), font(font)
{
	this->list_rect /= scale;

	this->item_rect.w /= scale;
	this->item_rect.h /= scale;
	this->item_rect.x = this->list_rect.x + (int)(this->item_rect.x / scale);

	scroll_bar_color_full.r = bg_list_color.r - 10;
	scroll_bar_color_full.g = bg_list_color.g - 10;
	scroll_bar_color_full.b = bg_list_color.b - 10;

	scroll_bar_color_current.r = bg_list_color.r - 30;
	scroll_bar_color_current.g = bg_list_color.g - 30;
	scroll_bar_color_current.b = bg_list_color.b - 30;

	scroll_bar_rect_full.x = this->list_rect.x + this->list_rect.w - (int)(scroll_bar_width / scale);
	scroll_bar_rect_full.y = this->list_rect.y;
	scroll_bar_rect_full.w = (int)(scroll_bar_width / scale);
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
	SDL_Rect text_rect = { (int)(item_rect.x + 5 / scale), item_rect.y, tex_size.x, tex_size.y};
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
	item_rect.y = list_rect.y + (int)items.size() * this->item_rect.h;
	std::unique_ptr<ItemData> item(new ItemData(ren, item_name, item_rect, font, text_color, scale));
	items.push_back(std::move(item));
	total_height = (int)items.size() * this->item_rect.h;
	total_height < this->list_rect.h ? scroll_bar_rect_current.h = this->list_rect.h : scroll_bar_rect_current.h = 
		(int)(((float)this->list_rect.h / total_height) * list_rect.h);
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
			total_height = (int)items.size() * this->item_rect.h;
			total_height < this->list_rect.h ? scroll_bar_rect_current.h = this->list_rect.h : scroll_bar_rect_current.h =
				(int)(((float)this->list_rect.h / total_height) * list_rect.h);
			if (item == items.end())
				break;
		}
		if (item_found == true)
		{
			item->get()->item_rect.y -= item_rect.h;
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

bool List::setScrollBarPointed(SDL_Point point)
{
	if (point.x > scroll_bar_rect_current.x && point.x < scroll_bar_rect_current.x + scroll_bar_rect_current.w && point.y > scroll_bar_rect_current.y &&
		point.y < scroll_bar_rect_current.y + scroll_bar_rect_current.h)
	{
		is_scroll_bar_pointed = true;
		scroll_bar_click = point;
	}
	else
		is_scroll_bar_pointed = false;
	return is_scroll_bar_pointed;
}

void List::setScrollBarPointed(bool is_scroll_bar_pointed)
{
	this->is_scroll_bar_pointed = is_scroll_bar_pointed;
}

void List::scrollBarMove(SDL_Point point)
{
	if (getScrollBarPointed() == false)
		return;
	int shift = point.y - scroll_bar_click.y; // scroll bar shift
	if (scroll_bar_rect_current.y + shift >= scroll_bar_rect_full.y && scroll_bar_rect_current.y + scroll_bar_rect_current.h + shift <=
		scroll_bar_rect_full.y + scroll_bar_rect_full.h)
	{
		scroll_bar_rect_current.y += shift;
		int items_shift = (int)(shift / (float)scroll_bar_rect_full.h * total_height);
		for (auto& item : items)
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
	if (point.x > list_rect.x && point.x < list_rect.x + list_rect.w - scroll_bar_rect_full.w && point.y > list_rect.y && point.y < list_rect.y + list_rect.h)
		for (auto& item : items)
			if (point.x > item->item_rect.x && point.x < item->item_rect.x + item->item_rect.w && point.y > item->item_rect.y &&
				point.y < item->item_rect.y + item->item_rect.h)
			{
				selected_item = item;
				return selected_item->item_name;
			}
	return "";
}

void List::clearSelectedItem()
{
	selected_item = nullptr;
}

void List::clear()
{
	selected_item = nullptr;
	items.clear();
}

bool List::getScrollBarPointed()
{ 
	if (visible) 
		return is_scroll_bar_pointed; 
	return false; 
};

// TextOutput class
TextOutput::TextOutput(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color) : ren(ren), font(font), 
text(text), scale(scale), x(x), y(y), text_color(text_color), type(0)
{
	this->x /= scale;
	this->y /= scale;
};

TextOutput::TextOutput(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color) : 
	ren(ren), font(font), text(text), scale(scale), x(x), y(y), text_color(text_color), bg_color(bg_color), type(1)
{
	this->x /= scale;
	this->y /= scale;
};

void TextOutput::destroyTexture()
{
	SDL_DestroyTexture(text_texture);
}

// TextOutputSingleLine class
TextOutputSingleLine::TextOutputSingleLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color) :
	TextOutput(ren, font, text, scale, x, y, text_color) 
{
	generateTexture();
};

TextOutputSingleLine::TextOutputSingleLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, 
	SDL_Color bg_color) : TextOutput(ren, font, text, scale, x, y, text_color, bg_color) 
{
	generateTexture();
};

TextOutputSingleLine::~TextOutputSingleLine() 
{
	destroyTexture();
};

void TextOutputSingleLine::generateTexture()
{
	destroyTexture();
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
	generateTexture();
};

TextOutputMultiLine::TextOutputMultiLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color, int width) :
	TextOutput(ren, font, text, scale, x, y, text_color, bg_color), width(width) 
{
	this->width /= scale;
	generateTexture();
};

TextOutputMultiLine::~TextOutputMultiLine() 
{
	destroyTexture();
};

void TextOutputMultiLine::generateTexture()
{
	destroyTexture();
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
WinMainMenu::WinMainMenu(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings, const Uint8* keys):
	Window(ren, windows, res, settings, keys)
{
	std::unique_ptr<Image_Button> btnNewGame(new Image_Button(ren, { 810, 470, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnNewGame"], settings->getScale(), [&]() {
			std::unique_ptr<WinGame>winGame(new WinGame(this->ren, this->windows, this->resources, this->settings, this->keys));
			this->windows.clear();
			this->windows.push_back(std::move(winGame));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnNewGame));

	std::unique_ptr<Image_Button> btnLoadGame(new Image_Button(ren, { 810, 540, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnLoadGame"], settings->getScale(), [&]() {
			std::unique_ptr<WinLoadGame>winLoadGame(new WinLoadGame(this->ren, this->windows, this->resources, this->settings, this->keys));
			this->windows.push_back(std::move(winLoadGame));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnLoadGame));

	std::unique_ptr<Image_Button> btnSettings(new Image_Button(ren, { 810, 610, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnSettings"], settings->getScale(), [&]() {	
			std::unique_ptr<WinSettings>winSettings(new WinSettings(this->ren, this->windows, this->resources, this->settings, this->keys));
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
WinGame::WinGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings, const Uint8* keys) :
	Window(ren, windows, res, settings, keys), character_info_output(ren, res, settings)
{
	// world creating
	world = new World(res, settings, 0, 6);
	game->world = this->world;

	// adding random characters
	for (int i = 0; i < 1 + rand() % 3; i++)
		world->addCharacter();
	int entity_num = 1;
	for (auto& entity : world->entities)
		if (typeid(*entity.second) == typeid(Character))
		{
			Character& character = dynamic_cast<Character&>(*entity.second);
			character.setX(100);
			character.setY(entity_num * 100);
			entity_num++;
		}

	// gui initialization
	std::unique_ptr<Image>bg_image(new Image(ren, res->gui_textures["background2"].texture, res->gui_textures["background2"].dstrect, 1.0f));
	images.push_back(std::move(bg_image));

	std::string day = res->lang["WinGame.day"];
	day += " " + std::to_string(world->getDay());
	TextOutputSingleLine* text_day = new TextOutputSingleLine(ren, res->fonts["calibri32"], day, settings->getScale(), 5, 5, { 60, 60, 200, 0 });
	texts["day"].reset(text_day);

	std::string time = std::to_string(world->getHour()) + ":00";
	TextOutputSingleLine* text_time = new TextOutputSingleLine(ren, res->fonts["calibri32"], time, settings->getScale(), 5, 45, { 60, 60, 200, 0 });
	texts["time"].reset(text_time);

	std::string resources = res->lang["WinGame.resources"];
	resources += ": " + std::to_string(world->getResources());
	TextOutputSingleLine* text_resources = new TextOutputSingleLine(ren, res->fonts["calibri32"], resources, settings->getScale(), 5, 85, { 60, 60, 200, 0 });
	texts["resources"].reset(text_resources);

	std::string food = res->lang["WinGame.food"];
	food += ": " + std::to_string(world->getFood());
	TextOutputSingleLine* text_food = new TextOutputSingleLine(ren, res->fonts["calibri32"], food, settings->getScale(), 5, 125, { 60, 60, 200, 0 });
	texts["food"].reset(text_food);
	
	// actions
	Image_Button* btnActions(new Image_Button(ren, { 300, 1020, 300, 60 }, { 60, 60, 200, 0 }, res->fonts["calibri32"],
		res->lang["WinGame.btnActions"], settings->getScale(), [&]() {
			buttons["searchResources"]->invertVisibility();
			buttons["searchFood"]->invertVisibility();
		}, res->gui_textures["button1"].texture));
	buttons["Actions"].reset(btnActions);

	Image_Button* btnSearchResources(new Image_Button(ren, { 300, 960, 300, 60 }, { 60, 60, 200, 0 }, res->fonts["calibri32"], 
		res->lang["WinGame.btnSearchResources"], settings->getScale(), [&]() {world->searchResources(); }, res->gui_textures["button1"].texture));
	btnSearchResources->setVisibility(false);
	buttons["searchResources"].reset(btnSearchResources);

	Image_Button* btnSearchFood(new Image_Button(ren, { 300, 900, 300, 60 }, { 60, 60, 200, 0 }, res->fonts["calibri32"], res->lang["WinGame.btnSearchFood"],
		settings->getScale(), [&]() {world->searchFood(); }, res->gui_textures["button1"].texture));
	btnSearchFood->setVisibility(false);
	buttons["searchFood"].reset(btnSearchFood);

	// characters
	Image_Button* btnCharacters(new Image_Button(ren, { 0, 1020, 300, 60 }, { 60, 60, 200, 0 }, res->fonts["calibri32"], res->lang["WinGame.btnCharacters"],
		settings->getScale(), [&]() {
			characters_list->invertVisibility();
			characters_list->clearSelectedItem();
		}, res->gui_textures["button1"].texture));
	buttons["Characters"].reset(btnCharacters);

	List* characters_list(new List(ren, { 0, 500, 300, 520 }, { 0, 0, 300, 60 }, { 200, 120, 80, 0 }, { 200, 110, 90, 0 }, { 170, 70, 100, 0 }, { 60, 60, 200, 0 },
		settings->getScale(), res->fonts["calibri32"], 15));
	characters_list->setVisibility(false);
	this->characters_list.reset(characters_list);
};

WinGame::~WinGame() {};

void WinGame::handleEvents()
{
	if (SDL_GetTicks64() >= update_timer && update_status == true)
		update();

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
		{
			std::unique_ptr<WinGameMenu>winSaveGame(new WinGameMenu(ren, windows, resources, settings, keys));
			windows.push_back(std::move(winSaveGame));
			break;
		}
		}
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			if (character_info_output.visible && character_info_output.isPointed(point_click))
			{
				if (character_info_output.btnClose->isPointed(point_click))
				{
					character_info_output.btnClose->onClick();
					characters_list->clearSelectedItem();
				}
			}
			else
			{
				for (auto& btn : buttons)
					if (btn.second->isPointed(point_click))
						btn.second->onClick();
				if (characters_list->getVisibility())
				{
					std::string selected_character_name = "";
					if (!characters_list->setScrollBarPointed(point_click))
						selected_character_name = characters_list->getClickedItem(point_click);

					if (selected_character_name != "") // changing character to output info about him
					{
						for (auto& entity : world->entities)
							if (typeid(*entity.second) == typeid(Character))
								if (dynamic_cast<Character&>(*entity.second).getName() == selected_character_name)
								{
									character_info_output.setCharacter(std::dynamic_pointer_cast<Character>(entity.second));
									character_info_output.visible = true;
									break;
								}
					}
					else
						characters_list->clearSelectedItem();
				}
			}

			break;
		}
		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			characters_list->setScrollBarPointed(false);
			break;
		}
	}
	case SDL_MOUSEMOTION:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			characters_list->scrollBarMove(point_click);
		}
	}
	}
	if (keys[SDL_SCANCODE_M])
	{
		for (auto& entity : world->entities)
			if (typeid(*entity.second) == typeid(Character))
			{
				Character& character = dynamic_cast<Character&>(*entity.second);
				character.addX(10 / settings->getScale());
			}
	}
	if (keys[SDL_SCANCODE_N])
	{
		for (auto& entity : world->entities)
			if (typeid(*entity.second) == typeid(Character))
			{
				Character& character = dynamic_cast<Character&>(*entity.second);
				character.addX(-10 / settings->getScale());
			}
	}
	if (keys[SDL_SCANCODE_O])
	{
		for (auto& entity : world->entities)
			if (typeid(*entity.second) == typeid(Character))
			{
				Character& character = dynamic_cast<Character&>(*entity.second);
				character.setWeaponAngle(character.getWeaponAngle() + 5);
			}
	}
	if (keys[SDL_SCANCODE_P])
	{
		for (auto& entity : world->entities)
			if (typeid(*entity.second) == typeid(Character))
			{
				Character& character = dynamic_cast<Character&>(*entity.second);
				character.setWeaponAngle(character.getWeaponAngle() - 5);
			}
	}
}

void WinGame::update()
{
	update_timer += 50; // warning: "update timer = SDL_GetTicks64() + 50" will lost the time if outside anywhere SDL_Delay is used
	world->update();
	character_info_output.update();

	if (world->trigger.day)
	{
		std::string day = (std::string)resources->lang["WinGame.day"] + " " + std::to_string(world->getDay());
		texts["day"]->setText(day);
		texts["day"]->generateTexture();
		world->trigger.day = false;
	}
	if (world->trigger.time)
	{
		int num_minutes = world->getMinutes();
		std::string str_minutes;
		if (num_minutes < 10)
			str_minutes = "0" + std::to_string(num_minutes);
		else
			str_minutes = std::to_string(num_minutes);
		int num_hour = world->getHour();
		std::string str_hour;
		if (num_hour < 10)
			str_hour = "0" + std::to_string(num_hour);
		else
			str_hour = std::to_string(num_hour);
		texts["time"]->setText(str_hour + ":" + str_minutes);
		texts["time"]->generateTexture();
		world->trigger.time = false;
	}
	if (world->trigger.resources)
	{
		std::string resources = (std::string)this->resources->lang["WinGame.resources"] + ": " + std::to_string(world->getResources());
		texts["resources"]->setText(resources);
		texts["resources"]->generateTexture();
		world->trigger.resources = false;
	}
	if (world->trigger.food)
	{
		std::string food = (std::string)this->resources->lang["WinGame.food"] + ": " + std::to_string(world->getFood());
		texts["food"]->setText(food);
		texts["food"]->generateTexture();
		world->trigger.food = false;
	}

	// check if characters are alive
	for (auto entity = world->entities.begin(); entity != world->entities.end(); entity++)
		if (typeid(*entity->second) == typeid(Character))
			if (dynamic_cast<Character&>(*entity->second).trigger.dead)
			{
				world->entities.erase(entity);
				world->trigger.characters_list_changed = true;
			}

	// trigger to update list of characters
	if (world->trigger.characters_list_changed)
	{
		world->trigger.characters_list_changed = false;
		characters_list->clear();
		for (auto& entity : world->entities)
			if (typeid(*entity.second) == typeid(Character))
			{
				Character& character = dynamic_cast<Character&>(*entity.second);
				characters_list->addItem(character.getName());
			}
	}
}

void WinGame::render()
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);

	for (auto& image : images)
		image->render();

	// render characters
	for (auto& entity : world->entities)
		if (typeid(*entity.second) == typeid(Character))
		{
			Character& character = dynamic_cast<Character&>(*entity.second);
			SDL_FRect character_dstrect = character.getDestRect() / settings->getScale();
			SDL_Rect weapon_dstrect = character.getWeapon()->getDestRect() / settings->getScale();
			
			if (character.getAngle() == 0)
			{
				SDL_RenderCopyF(ren, character.getTexture(), 0, &character_dstrect);
				if (weapon_dstrect.w < character_dstrect.w)
					weapon_dstrect.x = character_dstrect.x + character_dstrect.w - weapon_dstrect.w / 1.5f;
				else
					weapon_dstrect.x = character_dstrect.x;
				weapon_dstrect.y = character_dstrect.y + (character_dstrect.h - weapon_dstrect.h) / 2;
				SDL_RenderCopyEx(ren, character.getWeapon()->getTextureSimple(), 0, &weapon_dstrect, character.getWeaponAngle(), NULL, SDL_FLIP_NONE);
			}
			else if (character.getAngle() == 180)
			{
				SDL_RenderCopyExF(ren, character.getTexture(), 0, &character_dstrect, 0, NULL, SDL_FLIP_HORIZONTAL);
				weapon_dstrect.x = character_dstrect.x - weapon_dstrect.w / 3.0f;
				weapon_dstrect.y = character_dstrect.y + (character_dstrect.h - weapon_dstrect.h) / 2;
				SDL_RenderCopyEx(ren, character.getWeapon()->getTextureSimple(), 0, &weapon_dstrect, -character.getWeaponAngle(), NULL, 
					SDL_FLIP_HORIZONTAL);
			}
		}
		else if (typeid(*entity.second) == typeid(Zombie))
		{
			Zombie& zombie = dynamic_cast<Zombie&>(*entity.second);
			SDL_FRect zombie_dstrect = zombie.getDestRect() / settings->getScale();
			if (zombie.getAngle() == 0)
			{
				SDL_RenderCopyF(ren, zombie.getTexture(), 0, &zombie_dstrect);
			}
			else if (zombie.getAngle() == 180)
			{
				SDL_RenderCopyExF(ren, zombie.getTexture(), 0, &zombie_dstrect, 0, NULL, SDL_FLIP_HORIZONTAL);
			}
		}

	for (auto& btn : buttons)
		btn.second->render();

	characters_list->render();

	for (auto& text : texts)
		text.second->render();

	character_info_output.render();
}

WinGame::CharacterInfoOutput::CharacterInfoOutput(SDL_Renderer* ren, Resources* res, Settings* settings):
	ren(ren), res(res), settings(settings)
{
	float scale = settings->getScale();

	Image_Button* btnClose = new Image_Button(ren, { bg_rect.x + bg_rect.w - 30, bg_rect.y, 30, 30 }, { 0, 0, 0, 0 }, nullptr, "",
		scale, [&]() {removeCharacter(); }, res->gui_textures["button_close"].texture);
	this->btnClose.reset(btnClose);

	text_name = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 10, bg_rect.y + 20, { 60, 200, 60, 0 });
	
	text_consumption = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 20, bg_rect.y + 130, { 60, 170, 60, 0 });
	text_speed_per_second = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 20, bg_rect.y + 180, { 60, 170, 60, 0 });
	text_strength = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 20, bg_rect.y + 230, { 60, 170, 60, 0 });

	text_combat_title = new TextOutputSingleLine(ren, res->fonts["calibri32"], res->lang["WinGame.cio.combat_skill"], scale, bg_rect.x + 50,
		bg_rect.y + 540, {200, 60, 60, 0});
	text_combat_lvl = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 5, bg_rect.y + 580, { 200, 60, 60, 0 });
	text_combat_xp = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 25, bg_rect.y + 625, { 200, 60, 60, 0 });
	text_combat_xp_upgrade = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 225, bg_rect.y + 625, {200, 60, 60, 0});

	text_survival_title = new TextOutputSingleLine(ren, res->fonts["calibri32"], res->lang["WinGame.cio.survival_skill"], scale, bg_rect.x + 50,
		bg_rect.y + 720, { 30, 200, 30, 0 });
	text_survival_lvl = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 5, bg_rect.y + 760, { 30, 200, 30, 0 });
	text_survival_xp = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 25, bg_rect.y + 805, { 30, 200, 30, 0 });
	text_survival_xp_upgrade = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 225, bg_rect.y + 805, { 30, 200, 30, 0 });

	text_max_health = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 1150, bg_rect.y + 370, {30, 200, 30, 0});
	text_health = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 1150, bg_rect.y + 810, { 30, 200, 30, 0 });

	text_max_satiety = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 1250, bg_rect.y + 370, { 250, 127, 30, 0 });
	text_satiety = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 1250, bg_rect.y + 810, { 250, 127, 30, 0 });

	SDL_Rect rect_icon_combat = { (int)(310 / scale), (int)(640 / scale), res->icons_textures["combat"].dstrect.w, res->icons_textures["combat"].dstrect.h };
	icon_combat = new Image(ren, res->icons_textures["combat"].texture, rect_icon_combat, 1.0f);
	SDL_Rect rect_icon_survival = { (int)(310 / scale), (int)(820 / scale), res->icons_textures["survival"].dstrect.w, res->icons_textures["survival"].dstrect.h };
	icon_survival = new Image(ren, res->icons_textures["survival"].texture, rect_icon_survival, 1.0f);
	SDL_Rect rect_icon_health = { (int)(1415 / scale), (int)(670 / scale), res->icons_textures["health"].dstrect.w, res->icons_textures["health"].dstrect.h };
	icon_health = new Image(ren, res->icons_textures["health"].texture, rect_icon_health, 1.0f);
	SDL_Rect rect_icon_satiety = { (int)(1515 / scale), (int)(670 / scale), res->icons_textures["satiety"].dstrect.w, res->icons_textures["satiety"].dstrect.h };
	icon_satiety = new Image(ren, res->icons_textures["satiety"].texture, rect_icon_satiety, 1.0f);

	// general weapon data
	weapon_name = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 450, bg_rect.y + 200, { 30, 200, 30, 0 });
	weapon_max_distance = new TextOutputSingleLine(ren, res->fonts["calibri32"], "", scale, bg_rect.x + 450, bg_rect.y + 250, { 30, 200, 30, 0 });
	weapon_icon = new Image(ren, nullptr, { bg_rect.x + 450, bg_rect.y + 100, 0, 0 }, scale);
	SDL_Rect rect_icon_max_distance = { (int)((bg_rect.x + 400) / scale), (int)((bg_rect.y + 250) / scale), res->icons_textures["shot_max_distance"].dstrect.w,
		res->icons_textures["shot_max_distance"].dstrect.h };
	icon_max_distance = new Image(ren, res->icons_textures["shot_max_distance"].texture, rect_icon_max_distance, 1.0f);

	bg_rect /= scale;
	rect_combat_border /= scale;
	rect_combat_bg /= scale;
	rect_combat_dynamic /= scale;
	rect_survival_border /= scale;
	rect_survival_bg /= scale;
	rect_survival_dynamic /= scale;

	rect_health_border /= scale;
	rect_health_bg /= scale;
	rect_health_dynamic /= scale;
	rect_satiety_border /= scale;
	rect_satiety_bg /= scale;
	rect_satiety_dynamic /= scale;
}

WinGame::CharacterInfoOutput::~CharacterInfoOutput()
{
	text_name->~TextOutputSingleLine();
	text_consumption->~TextOutputSingleLine();
	text_speed_per_second->~TextOutputSingleLine();
	text_strength->~TextOutputSingleLine();

	text_combat_title->~TextOutputSingleLine();
	text_combat_lvl->~TextOutputSingleLine();
	text_combat_xp->~TextOutputSingleLine();
	text_combat_xp_upgrade->~TextOutputSingleLine();

	text_survival_title->~TextOutputSingleLine();
	text_survival_lvl->~TextOutputSingleLine();
	text_survival_xp->~TextOutputSingleLine();
	text_survival_xp_upgrade->~TextOutputSingleLine();

	text_max_health->~TextOutputSingleLine();
	text_health->~TextOutputSingleLine();
	text_max_satiety->~TextOutputSingleLine();
	text_satiety->~TextOutputSingleLine();

	icon_health->~Image();
	icon_satiety->~Image();
	icon_combat->~Image();
	icon_survival->~Image();

	removeWeapon();
	weapon_icon->~Image();
	weapon_name->~TextOutputSingleLine();
	weapon_max_distance->~TextOutputSingleLine();
}

void WinGame::CharacterInfoOutput::update()
{
	if (character == nullptr)
		return;
	if (character->trigger.name)
	{
		text_name->setText(character->getName());
		text_name->generateTexture();
		character->trigger.name = false;
	}
	if (character->trigger.max_health)
	{
		text_max_health->setText(std::to_string(character->getMaxHealth()));
		text_max_health->generateTexture();
		character->trigger.max_health = false;
	}
	if (character->trigger.health)
	{
		text_health->setText(std::to_string(character->getHealth()));
		text_health->generateTexture();
		float health_ratio = ((float)character->getHealth() / character->getMaxHealth());
		rect_health_dynamic.h = (int)(rect_health_bg.h * health_ratio);
		rect_health_dynamic.y = rect_health_bg.y + (rect_health_bg.h - rect_health_dynamic.h);
		color_health_dynamic.r = (int)(255 * (1.0f - health_ratio));
		color_health_dynamic.g = (int)(255 * health_ratio);
		character->trigger.health = false;
	}
	if (character->trigger.max_satiety)
	{
		text_max_satiety->setText(std::to_string(character->getMaxSatiety()));
		text_max_satiety->generateTexture();
		character->trigger.max_satiety = false;
	}
	if (character->trigger.satiety)
	{
		text_satiety->setText(std::to_string(character->getSatiety()));
		text_satiety->generateTexture();
		rect_satiety_dynamic.h = (int)(rect_satiety_bg.h * ((float)character->getSatiety() / character->getMaxSatiety()));
		rect_satiety_dynamic.y = rect_satiety_bg.y + (rect_satiety_bg.h - rect_satiety_dynamic.h);
		character->trigger.satiety = false;
	}
	if (character->trigger.consumption)
	{
		std::string consumption = (std::string)res->lang["WinGame.cio.consumption"] + ": " + std::to_string(character->getConsumption());
		text_consumption->setText(consumption);
		text_consumption->generateTexture();
		character->trigger.consumption = false;
	}
	if (character->trigger.speed_per_second)
	{
		std::string speed = (std::string)res->lang["WinGame.cio.speed_per_second"] + ": " + FloatToStr(character->getSpeedPerTick() * 20, 1);
		text_speed_per_second->setText(speed);
		text_speed_per_second->generateTexture();
		character->trigger.speed_per_second = false;
	}
	if (character->trigger.strength)
	{
		std::string strength = (std::string)res->lang["WinGame.cio.strength"] + ": " + std::to_string(character->getStrength());
		text_strength->setText(strength);
		text_strength->generateTexture();
		character->trigger.strength = false;
	}
	if (character->trigger.weapon)
	{
		setWeapon();
		character->trigger.weapon = false;
	}
	if (character->trigger.combat_lvl)
	{
		text_combat_lvl->setText(std::to_string(character->getCombatLvl()));
		text_combat_lvl->generateTexture();

		text_combat_xp_upgrade->setText(std::to_string(character->getCombatXpUpgrade()));
		text_combat_xp_upgrade->generateTexture();
		character->trigger.combat_lvl = false;
	}
	if (character->trigger.combat_xp)
	{
		text_combat_xp->setText(std::to_string(character->getCombatXp()));
		text_combat_xp->generateTexture();
		rect_combat_dynamic.w = (int)(rect_combat_bg.w * ((float)character->getCombatXp() / character->getCombatXpUpgrade())); // combat xp line
		character->trigger.combat_xp = false;
	}
	if (character->trigger.survival_lvl)
	{
		text_survival_lvl->setText(std::to_string(character->getSurvivalLvl()));
		text_survival_lvl->generateTexture();

		text_survival_xp_upgrade->setText(std::to_string(character->getSurvivalXpUpgrade()));
		text_survival_xp_upgrade->generateTexture();
		character->trigger.survival_lvl = false;
	}
	if (character->trigger.survival_xp)
	{
		text_survival_xp->setText(std::to_string(character->getSurvivalXp()));
		text_survival_xp->generateTexture();
		rect_survival_dynamic.w = (int)(rect_survival_bg.w * ((float)character->getSurvivalXp() / character->getSurvivalXpUpgrade())); // survival xp line
		character->trigger.survival_xp = false;
	}
	if (character->trigger.dead)
		btnClose->onClick();
}

void WinGame::CharacterInfoOutput::render()
{
	if (visible == false || character == nullptr)
		return;
	SDL_SetRenderDrawColor(ren, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderFillRect(ren, &bg_rect);
	btnClose->render();

	// xp static borders
	SDL_SetRenderDrawColor(ren, 139, 45, 173, 0);
	SDL_RenderFillRect(ren, &rect_combat_border);
	SDL_RenderFillRect(ren, &rect_survival_border);

	// xp static background
	SDL_SetRenderDrawColor(ren, 255, 184, 71, 0);
	SDL_RenderFillRect(ren, &rect_combat_bg);
	SDL_RenderFillRect(ren, &rect_survival_bg);

	// xp dynamic background
	SDL_SetRenderDrawColor(ren, 179, 85, 213, 0);
	SDL_RenderFillRect(ren, &rect_combat_dynamic);
	SDL_RenderFillRect(ren, &rect_survival_dynamic);

	text_name->render();
	text_consumption->render();
	text_speed_per_second->render();
	text_strength->render();
	
	text_combat_title->render();
	text_combat_lvl->render();
	text_combat_xp->render();
	text_combat_xp_upgrade->render();

	text_survival_title->render();
	text_survival_lvl->render();
	text_survival_xp->render();
	text_survival_xp_upgrade->render();

	// static borders
	SDL_SetRenderDrawColor(ren, 139, 45, 173, 0);
	SDL_RenderFillRect(ren, &rect_health_border);
	SDL_RenderFillRect(ren, &rect_satiety_border);

	// static background
	SDL_SetRenderDrawColor(ren, 255, 184, 71, 0);
	SDL_RenderFillRect(ren, &rect_health_bg);
	SDL_RenderFillRect(ren, &rect_satiety_bg);

	// dynamic background
	SDL_SetRenderDrawColor(ren, color_health_dynamic.r, color_health_dynamic.g, color_health_dynamic.b, color_health_dynamic.a);
	SDL_RenderFillRect(ren, &rect_health_dynamic);
	SDL_SetRenderDrawColor(ren, 201, 115, 45, 0);
	SDL_RenderFillRect(ren, &rect_satiety_dynamic);

	text_max_health->render();
	text_health->render();

	text_max_satiety->render();
	text_satiety->render();

	// render icons
	icon_combat->render();
	icon_survival->render();
	icon_health->render();
	icon_satiety->render();

	// weapon info render
	if (weapon_type != -1)
	{
		weapon_name->render();
		weapon_icon->render();

		weapon_max_distance->render();
		icon_max_distance->render();
	}
	if (weapon_type == 0)
	{
		weapon.single_shot.shot_damage->render();
		weapon.single_shot.icon_shot_damage->render();

		weapon.single_shot.shot_accuracy->render();
		weapon.single_shot.icon_shot_accuracy->render();

		weapon.single_shot.shot_timeout->render();
		weapon.single_shot.icon_shot_timeout->render();
	}
	else if (weapon_type == 1)
	{
		weapon.burst_fire.shot_damage->render();
		weapon.burst_fire.icon_shot_damage->render();

		weapon.burst_fire.shot_accuracy->render();
		weapon.burst_fire.icon_shot_accuracy->render();

		weapon.burst_fire.shot_timeout->render();
		weapon.burst_fire.icon_shot_timeout->render();

		weapon.burst_fire.burst_length->render();
		weapon.burst_fire.icon_burst_length->render();

		weapon.burst_fire.burst_timeout->render();
		weapon.burst_fire.icon_burst_timeout->render();
	}
	else if (weapon_type == 2)
	{
		weapon.shotgun.pellet_damage->render();
		weapon.shotgun.icon_pellet_damage->render();

		weapon.shotgun.pellet_accuracy->render();
		weapon.shotgun.icon_pellet_accuracy->render();

		weapon.shotgun.pellet_count->render();
		weapon.shotgun.icon_pellet_count->render();

		weapon.shotgun.shot_timeout->render();
		weapon.shotgun.icon_shot_timeout->render();
	}
}

void WinGame::CharacterInfoOutput::setCharacter(std::shared_ptr<Character>character)
{
	removeCharacter();
	this->character = character;
	this->character->trigger.name = true;
	this->character->trigger.max_health = true;
	this->character->trigger.health = true;
	this->character->trigger.max_satiety = true;
	this->character->trigger.satiety = true;
	this->character->trigger.consumption = true;
	this->character->trigger.speed_per_second = true;
	this->character->trigger.strength = true;
	this->character->trigger.weapon = true;
	this->character->trigger.combat_lvl = true;
	this->character->trigger.combat_xp = true;
	this->character->trigger.survival_lvl = true;
	this->character->trigger.survival_xp = true;
	update();
}

void WinGame::CharacterInfoOutput::removeCharacter()
{
	character = nullptr;
	visible = false;
}

bool WinGame::CharacterInfoOutput::isPointed(SDL_Point point)
{
	if (point.x > bg_rect.x && point.x < bg_rect.x + bg_rect.w && point.y > bg_rect.y && point.y < bg_rect.y + bg_rect.h)
		return true;
	return false;
}

void WinGame::CharacterInfoOutput::setWeapon()
{
	removeWeapon();
	Weapon* weapon = character->getWeapon();
	float scale = settings->getScale();

	// general data for all types of weapons
	this->weapon_name->setText(weapon->getName());
	this->weapon_name->generateTexture();
	this->weapon_max_distance->setText(std::to_string(weapon->getMaxDistance()));
	this->weapon_max_distance->generateTexture();
	this->weapon_icon->setTexture(weapon->getTextureSimple());

	// special data for each type
	if (typeid(*weapon) == typeid(Weapon_SingleShot))
	{
		weapon_type = 0;
		Weapon_SingleShot& wpn_single_shot = dynamic_cast<Weapon_SingleShot&>(*weapon);

		this->weapon.single_shot.shot_damage = new TextOutputSingleLine(ren, res->fonts["calibri32"], std::to_string(wpn_single_shot.getShotDamage()), 
			scale, 750, 400, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_shot_damage = { (int)(700 / scale), (int)(400 / scale), res->icons_textures["shot_damage"].dstrect.w, 
			res->icons_textures["shot_damage"].dstrect.h };
		this->weapon.single_shot.icon_shot_damage = new Image(ren, res->icons_textures["shot_damage"].texture, rect_icon_shot_damage, 1.0f);

		this->weapon.single_shot.shot_accuracy = new TextOutputSingleLine(ren, res->fonts["calibri32"],
			std::to_string(wpn_single_shot.getShotAccuracy()), scale, 750, 450, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_shot_accuracy = { (int)(700 / scale), (int)(450 / scale), res->icons_textures["shot_accuracy"].dstrect.w,
		res->icons_textures["shot_accuracy"].dstrect.h };
		this->weapon.single_shot.icon_shot_accuracy = new Image(ren, res->icons_textures["shot_accuracy"].texture, rect_icon_shot_accuracy, 1.0f);

		this->weapon.single_shot.shot_timeout = new TextOutputSingleLine(ren, res->fonts["calibri32"], FloatToStr(wpn_single_shot.getShotTimeout(), 1), 
			scale, 750, 500, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_shot_timeout = { (int)(700 / scale), (int)(500 / scale), res->icons_textures["shot_timeout"].dstrect.w,
			res->icons_textures["shot_timeout"].dstrect.h };
		this->weapon.single_shot.icon_shot_timeout = new Image(ren, res->icons_textures["shot_timeout"].texture, rect_icon_shot_timeout, 1.0f);
	}
	else if (typeid(*weapon) == typeid(Weapon_BurstFire))
	{
		weapon_type = 1;
		Weapon_BurstFire& wpn_burst_fire = dynamic_cast<Weapon_BurstFire&>(*weapon);

		this->weapon.burst_fire.shot_damage = new TextOutputSingleLine(ren, res->fonts["calibri32"], std::to_string(wpn_burst_fire.getShotDamage()), 
			scale, 750, 400, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_shot_damage = { (int)(700 / scale), (int)(400 / scale), res->icons_textures["shot_damage"].dstrect.w,
			res->icons_textures["shot_damage"].dstrect.h };
		this->weapon.burst_fire.icon_shot_damage = new Image(ren, res->icons_textures["shot_damage"].texture, rect_icon_shot_damage, 1.0f);

		this->weapon.burst_fire.shot_accuracy = new TextOutputSingleLine(ren, res->fonts["calibri32"], std::to_string(wpn_burst_fire.getShotAccuracy()), 
			scale, 750, 450, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_shot_accuracy = { (int)(700 / scale), (int)(450 / scale), res->icons_textures["shot_accuracy"].dstrect.w,
			res->icons_textures["shot_accuracy"].dstrect.h };
		this->weapon.burst_fire.icon_shot_accuracy = new Image(ren, res->icons_textures["shot_accuracy"].texture, rect_icon_shot_accuracy, 1.0f);

		this->weapon.burst_fire.shot_timeout = new TextOutputSingleLine(ren, res->fonts["calibri32"], FloatToStr(wpn_burst_fire.getShotTimeout(), 1), 
			scale, 750, 500, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_shot_timeout = { (int)(700 / scale), (int)(500 / scale), res->icons_textures["shot_timeout"].dstrect.w,
			res->icons_textures["shot_timeout"].dstrect.h };
		this->weapon.burst_fire.icon_shot_timeout = new Image(ren, res->icons_textures["shot_timeout"].texture, rect_icon_shot_timeout, 1.0f);

		this->weapon.burst_fire.burst_length = new TextOutputSingleLine(ren, res->fonts["calibri32"],std::to_string(wpn_burst_fire.getBurstLength()), 
			scale, 750, 550, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_burst_length = { (int)(700 / scale), (int)(550 / scale), res->icons_textures["burst_length"].dstrect.w,
			res->icons_textures["burst_length"].dstrect.h };
		this->weapon.burst_fire.icon_burst_length = new Image(ren, res->icons_textures["burst_length"].texture, rect_icon_burst_length, 1.0f);

		this->weapon.burst_fire.burst_timeout = new TextOutputSingleLine(ren, res->fonts["calibri32"], FloatToStr(wpn_burst_fire.getBurstTimeout(), 1), 
			scale, 750, 600, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_burst_timeout = { (int)(700 / scale), (int)(600 / scale), res->icons_textures["burst_timeout"].dstrect.w,
			res->icons_textures["burst_timeout"].dstrect.h };
		this->weapon.burst_fire.icon_burst_timeout = new Image(ren, res->icons_textures["burst_timeout"].texture, rect_icon_burst_timeout, 1.0f);
	}
	else if (typeid(*weapon) == typeid(Weapon_Shotgun))
	{
		weapon_type = 2;
		Weapon_Shotgun& wpn_shotgun = dynamic_cast<Weapon_Shotgun&>(*weapon);

		this->weapon.shotgun.pellet_damage = new TextOutputSingleLine(ren, res->fonts["calibri32"], std::to_string(wpn_shotgun.getPelletDamage()), 
			scale, 750, 400, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_pellet_damage = { (int)(700 / scale), (int)(400 / scale), res->icons_textures["shot_damage"].dstrect.w,
			res->icons_textures["shot_damage"].dstrect.h };
		this->weapon.shotgun.icon_pellet_damage = new Image(ren, res->icons_textures["shot_damage"].texture, rect_icon_pellet_damage, 1.0f);

		this->weapon.shotgun.pellet_accuracy = new TextOutputSingleLine(ren, res->fonts["calibri32"], std::to_string(wpn_shotgun.getPelletAccuracy()), 
			scale, 750, 450, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_pellet_accuracy = { (int)(700 / scale), (int)(450 / scale), res->icons_textures["shot_accuracy"].dstrect.w,
			res->icons_textures["shot_accuracy"].dstrect.h };
		this->weapon.shotgun.icon_pellet_accuracy = new Image(ren, res->icons_textures["shot_accuracy"].texture, rect_icon_pellet_accuracy, 1.0f);

		this->weapon.shotgun.pellet_count = new TextOutputSingleLine(ren, res->fonts["calibri32"], std::to_string(wpn_shotgun.getPelletCount()), 
			scale, 750, 500, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_pellet_count = { (int)(700 / scale), (int)(500 / scale), res->icons_textures["pellet_count"].dstrect.w,
			res->icons_textures["pellet_count"].dstrect.h };
		this->weapon.shotgun.icon_pellet_count = new Image(ren, res->icons_textures["pellet_count"].texture, rect_icon_pellet_count, 1.0f);

		this->weapon.shotgun.shot_timeout = new TextOutputSingleLine(ren, res->fonts["calibri32"], FloatToStr(wpn_shotgun.getShotTimeot(), 1), 
			scale, 750, 550, { 30, 200, 30, 0 });
		SDL_Rect rect_icon_shot_timeout = { (int)(700 / scale), (int)(550 / scale), res->icons_textures["shot_timeout"].dstrect.w,
			res->icons_textures["shot_timeout"].dstrect.h };
		this->weapon.shotgun.icon_shot_timeout = new Image(ren, res->icons_textures["shot_timeout"].texture, rect_icon_shot_timeout, 1.0f);
	}
}

void WinGame::CharacterInfoOutput::removeWeapon()
{
	if (weapon_type == 0)
	{
		weapon.single_shot.shot_damage->~TextOutputSingleLine();
		weapon.single_shot.icon_shot_damage->~Image();

		weapon.single_shot.shot_accuracy->~TextOutputSingleLine();
		weapon.single_shot.icon_shot_accuracy->~Image();

		weapon.single_shot.shot_timeout->~TextOutputSingleLine();
		weapon.single_shot.icon_shot_timeout->~Image();
	}
	else if (weapon_type == 1)
	{
		weapon.burst_fire.shot_damage->~TextOutputSingleLine();
		weapon.burst_fire.icon_shot_damage->~Image();

		weapon.burst_fire.shot_accuracy->~TextOutputSingleLine();
		weapon.burst_fire.icon_shot_accuracy->~Image();

		weapon.burst_fire.shot_timeout->~TextOutputSingleLine();
		weapon.burst_fire.icon_shot_timeout->~Image();

		weapon.burst_fire.burst_length->~TextOutputSingleLine();
		weapon.burst_fire.icon_burst_length->~Image();

		weapon.burst_fire.burst_timeout->~TextOutputSingleLine();
		weapon.burst_fire.icon_burst_timeout->~Image();


	}
	else if (weapon_type == 2)
	{
		weapon.shotgun.pellet_damage->~TextOutputSingleLine();
		weapon.shotgun.icon_pellet_damage->~Image();

		weapon.shotgun.pellet_accuracy->~TextOutputSingleLine();
		weapon.shotgun.icon_pellet_accuracy->~Image();

		weapon.shotgun.pellet_count->~TextOutputSingleLine();
		weapon.shotgun.icon_pellet_count->~Image();

		weapon.shotgun.shot_timeout->~TextOutputSingleLine();
		weapon.shotgun.icon_shot_timeout->~Image();
	}
}
// class WinGameMenu
WinGameMenu::WinGameMenu(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings, const Uint8* keys) :
	Window(ren, windows, res, settings, keys)
{
	std::unique_ptr<Image_Button>btnSaveGame(new Image_Button(ren, { 810, 420, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinGameMenu.btnSaveGame"], settings->getScale(), [&]() {
			std::unique_ptr<WinSaveGame>winSaveGame(new WinSaveGame(this->ren, this->windows, this->resources, this->settings, this->keys));
			this->windows.push_back(std::move(winSaveGame));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnSaveGame));

	std::unique_ptr<Image_Button>btnLoadGame(new Image_Button(ren, { 810, 500, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinGameMenu.btnLoadGame"], settings->getScale(), [&]() {
			std::unique_ptr<WinLoadGame>winLoadGame(new WinLoadGame(this->ren, this->windows, this->resources, this->settings, this->keys));
			this->windows.push_back(std::move(winLoadGame));
		}, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnLoadGame));

	std::unique_ptr<Image_Button>btnMainMenu(new Image_Button(ren, { 810, 580, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinGameMenu.btnMainMenu"], settings->getScale(), [&]() {
			std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(this->ren, this->windows, this->resources, this->settings, this->keys));
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
WinSaveGame::WinSaveGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings, const Uint8* keys):
	Window(ren, windows, res, settings, keys)
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
WinLoadGame::WinLoadGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings, const Uint8* keys) :
	Window(ren, windows, res, settings, keys)
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
			if (!saves_list->setScrollBarPointed(point_click))
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
			saves_list->setScrollBarPointed(false);
			break;
		}
	}
	case SDL_MOUSEMOTION:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
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
			std::unique_ptr<WinDelSave>winDelSave(new WinDelSave(ren, windows, resources, settings, keys, saves_list));
			windows.push_back(std::move(winDelSave));
		}
}

// class WinDelSave
WinDelSave::WinDelSave(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings, const Uint8* keys,
	std::unique_ptr<List>& saves_list): Window(ren, windows, res, settings, keys), saves_list(saves_list)
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
WinSettings::WinSettings(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings, const Uint8* keys) :
	Window(ren, windows, res, settings, keys)
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
			this->lang_list->invertVisibility();
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

	std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(ren, windows, game->resources, this->settings, keys));
	windows.clear();
	windows.push_back(std::move(winMainMenu));
}