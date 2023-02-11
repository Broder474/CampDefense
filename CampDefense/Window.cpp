#include "Game.h"
#include <fstream>

// Button class
template<class O, class M>
Button::Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::string text, float scale, O* object, M method):
	ren(ren), dstrect(dstrect), text_color(text_color), onClick(onClick), font(font), text(text), scale(scale)
{
	this->dstrect.x /= scale;
	this->dstrect.y /= scale;
	this->dstrect.w /= scale;
	this->dstrect.h /= scale;
	generateTexture();
	setOnClick(object, method);
}

void Button::render()
{
	SDL_RenderCopy(ren, text_texture, 0, &text_rect); // render only text
}

bool Button::isPointed(SDL_Point point)
{
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
	SDL_Point tex_size = { 0, 0 };
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
	SDL_RenderCopy(ren, bg_texture, nullptr, &dstrect);
	Button::render();
};

// Color_Button class
Color_Button::~Color_Button() {};

void Color_Button::render() 
{
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
	SDL_RenderCopy(ren, text_texture, nullptr, &text_dstrect);
}

// WinMainMenu class
WinMainMenu::WinMainMenu(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings):
	Window(ren, windows, res, settings)
{
	std::unique_ptr<Image_Button> btnNewGame(new Image_Button(ren, { 810, 470, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnNewGame"], settings->getScale(), this, & WinMainMenu::BtnNewGame, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnNewGame));
	std::unique_ptr<Image_Button> btnLoadGame(new Image_Button(ren, { 810, 540, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnLoadGame"], settings->getScale(), this, & WinMainMenu::BtnLoadGame, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnLoadGame));
	std::unique_ptr<Image_Button> btnSettings(new Image_Button(ren, { 810, 610, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnSettings"], settings->getScale(), this, & WinMainMenu::BtnSettings, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnSettings));
	std::unique_ptr<Image_Button> btnExit(new Image_Button(ren, { 810, 680, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinMainMenu.btnExit"], settings->getScale(), this, & WinMainMenu::BtnExit, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnExit));
};

WinMainMenu::~WinMainMenu() {};

void WinMainMenu::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

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
			for (int btn = 0; btn < buttons.size(); btn++)
				if (buttons[btn]->isPointed({ event.button.x, event.button.y }))
					buttons[btn]->onClick(); 
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


	for (int btn = 0; btn < buttons.size(); btn++)
		buttons[btn]->render();
};

void WinMainMenu::BtnNewGame()
{
	std::unique_ptr<WinGame>winGame(new WinGame(ren, windows, resources, settings));
	windows.clear();
	windows.push_back(std::move(winGame));
}

void WinMainMenu::BtnLoadGame()
{
	std::unique_ptr<WinLoadGame>winLoadGame(new WinLoadGame(ren, windows, resources, settings));
	windows.clear();
	windows.push_back(std::move(winLoadGame));
}

void WinMainMenu::BtnSettings()
{
	std::unique_ptr<WinSettings>winSettings(new WinSettings(ren, windows, resources, settings));
	windows.clear();
	windows.push_back(std::move(winSettings));
}

void WinMainMenu::BtnExit()
{
	windows.clear();
}

// WinGame class
WinGame::WinGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings) : 
	Window(ren, windows, res, settings) 
{
	std::unique_ptr<Image>bg_image(new Image(ren, res->gui_textures["background2"].texture, res->gui_textures["background2"].dstrect, 1.0f));
	images.push_back(std::move(bg_image));
};

WinGame::~WinGame() {};

void WinGame::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

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
	}
}

void WinGame::render()
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);

	for (int i = 0; i < images.size(); i++)
		images.at(i)->render();
}

// class WinGameMenu
WinGameMenu::WinGameMenu(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* res, Settings* settings) :
	Window(ren, windows, res, settings)
{
	std::unique_ptr<Image_Button>btnSaveGame(new Image_Button(ren, { 810, 420, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinGameMenu.btnSaveGame"], settings->getScale(), this, & WinGameMenu::BtnSaveGame, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnSaveGame));
	std::unique_ptr<Image_Button>btnLoadGame(new Image_Button(ren, { 810, 500, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinGameMenu.btnLoadGame"], settings->getScale(), this, &WinGameMenu::BtnLoadGame, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnLoadGame));
	std::unique_ptr<Image_Button>btnMainMenu(new Image_Button(ren, { 810, 580, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"],
		res->lang["WinGameMenu.btnMainMenu"], settings->getScale(), this, &WinGameMenu::BtnMainMenu, res->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnMainMenu));
	std::unique_ptr<Image>bg_image(new Image(ren, res->gui_textures["background1"].texture, { 260, 140, 1400, 800 }, settings->getScale()));
	images.push_back(std::move(bg_image));
}

WinGameMenu::~WinGameMenu() {};

void WinGameMenu::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

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
			for (int btn = 0; btn < buttons.size(); btn++)
				if (buttons[btn]->isPointed({ event.button.x, event.button.y }))
					buttons[btn]->onClick();
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
	for (int i = 0; i < images.size(); i++)
		images.at(i)->render();

	for (int i = 0; i < buttons.size(); i++)
		buttons.at(i)->render();
}

void WinGameMenu::BtnSaveGame()
{
	std::unique_ptr<WinSaveGame>winSaveGame(new WinSaveGame(ren, windows, resources, settings));
	windows.pop_back();
	windows.push_back(std::move(winSaveGame));
}

void WinGameMenu::BtnLoadGame()
{
	std::unique_ptr<WinLoadGame>winLoadGame(new WinLoadGame(ren, windows, resources, settings));
	windows.pop_back();
	windows.push_back(std::move(winLoadGame));
}
void WinGameMenu::BtnMainMenu()
{
	std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(ren, windows, resources, settings));
	windows.clear();
	windows.push_back(std::move(winMainMenu));
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
			for (int btn = 0; btn < buttons.size(); btn++)
				if (buttons[btn]->isPointed({ event.button.x, event.button.y }))
					buttons[btn]->onClick();
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
	for (int i = 0; i < images.size(); i++)
		images.at(i)->render();
	for (int i = 0; i < buttons.size(); i++)
		buttons.at(i)->render();
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
			saves_list->setScrollBarPointed({ event.button.x, event.button.y });
			saves_list->getClickedItem({ event.button.x, event.button.y });

			for (int btn = 0; btn < buttons.size(); btn++)
				if (buttons[btn]->isPointed({ event.button.x, event.button.y }))
					buttons[btn]->onClick();
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
					saves_list->scrollBarMove({event.button.x, event.button.y});
		}
	}
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(ren, windows, resources, settings));
			windows.clear();
			windows.push_back(std::move(winMainMenu));
			break;
		}
	}
};

void WinLoadGame::render() 
{
	SDL_SetRenderDrawColor(ren, 40, 30, 70, 255);
	SDL_RenderClear(ren);

	saves_list->render();
	for (int i = 0; i < buttons.size(); i++)
		buttons.at(i)->render();
};

void WinLoadGame::BtnLoadGame() {};

void WinLoadGame::LoadSaves()
{
	fs::path saves_dir = fs::current_path();
	saves_dir += "/saves";
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
		res->lang["WinDelSave.btnCancel"], settings->getScale(), this, &WinDelSave::BtnCancel, res->gui_textures["button1"].texture));
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
			for (int btn = 0; btn < buttons.size(); btn++)
				if (buttons[btn]->isPointed({ event.button.x, event.button.y }))
					buttons[btn]->onClick();
			break;
		}
		break;
	}
	}
};

void WinDelSave::render()
{
	for (int text = 0; text < static_text.size(); text++)
		static_text[text]->render();

	for (int btn = 0; btn < buttons.size(); btn++)
		buttons[btn]->render();
};

void WinDelSave::BtnDelete()
{
	fs::path path = "saves/" + saves_list->selected_item->item_name + ".zomb";
	fs::remove(path);
	saves_list->removeItem(saves_list->selected_item->item_name);
	windows.pop_back();
}

void WinDelSave::BtnCancel()
{
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
	for (auto lang = res->lang_list.begin(); lang != res->lang_list.end(); lang++)
		lang_list->addItem(lang->second);
	lang_list->setVisibility(false);

	Image_Button* btnLang(new Image_Button(ren, { 200, 100, 300, 60 }, { 90, 90, 140, 0 }, res->fonts["calibri32"], res->lang_list[settings->getLang()],
		settings->getScale(), this, &WinSettings::BtnLang, res->gui_textures["button2"].texture));
	this->btnLang.reset(btnLang);
}

WinSettings::~WinSettings() {};

void WinSettings::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

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
			lang_list->setScrollBarPointed({ event.button.x, event.button.y });
			std::string selected_lang = lang_list->getClickedItem({ event.button.x, event.button.y });
			if (selected_lang != "")
				btnLang->setText(selected_lang);
			if (btnLang->isPointed({ event.button.x, event.button.y }))
				btnLang->onClick();

			for (int btn = 0; btn < buttons.size(); btn++)
				if (buttons[btn]->isPointed({ event.button.x, event.button.y }))
					buttons[btn]->onClick();
			break;
		}
		break;
	}
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(ren, windows, resources, settings));
			windows.clear();
			windows.push_back(std::move(winMainMenu));
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

	for (int btn = 0; btn < buttons.size(); btn++)
		buttons[btn]->render();
}

void WinSettings::BtnSave()
{
	std::string full_lang = btnLang->getText();
	std::string short_lang = "";
	for (auto it = resources->lang_list.begin(); it != resources->lang_list.end(); it++)
		if (full_lang == it->second)
		{
			settings->setLang(it->first);
			break;
		}

	// settings saving
	std::ofstream out_settings("settings.json", std::ios::trunc);
	out_settings << settings->convert_to_json();
	out_settings.close();

	extern Game* game;
	game->loadResources();

	std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(ren, windows, game->resources, settings));
	windows.clear();
	windows.push_back(std::move(winMainMenu));
}

void WinSettings::BtnLang()
{
	lang_list->setVisibility(!lang_list->getVisibility());
}