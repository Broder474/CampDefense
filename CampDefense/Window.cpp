#include "Window.h"

// Button class
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
	SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const*>((wchar_t*)text.c_str()), text_color);
	if (text_texture == nullptr)
		SDL_DestroyTexture(text_texture);
	text_texture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);

	// remember size of text texture
	SDL_Point tex_size;
	SDL_QueryTexture(text_texture, nullptr, nullptr, &tex_size.x, &tex_size.y);
	tex_size.x /= scale;
	tex_size.y /= scale;
	text_rect = {dstrect.x + (dstrect.w - tex_size.x) / 2, dstrect.y + (dstrect.h - tex_size.y) / 2, tex_size.x, tex_size.y};
};

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
void Image::render()
{
	SDL_RenderCopy(ren, texture, nullptr, &dstrect);
}

// WinMainMenu class
WinMainMenu::WinMainMenu(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources): 
	Window(ren, scale, windows, resources)
{
	std::unique_ptr<Image_Button> btnNewGame(new Image_Button(ren, { 810, 470, 300, 60 }, { 180, 180, 180, 0 }, resources->fonts["calibri64"], L"New game", scale,
		this, &WinMainMenu::BtnNewGame, resources->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnNewGame));
	std::unique_ptr<Image_Button> btnLoadGame(new Image_Button(ren, { 810, 540, 300, 60 }, { 180, 180, 180, 0 }, resources->fonts["calibri64"], L"Load game", scale,
		this, &WinMainMenu::BtnLoadGame, resources->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnLoadGame));
	std::unique_ptr<Image_Button> btnSettings(new Image_Button(ren, { 810, 610, 300, 60 }, { 180, 180, 180, 0 }, resources->fonts["calibri64"], L"Settings", scale,
		this, &WinMainMenu::BtnSettings, resources->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnSettings));
	std::unique_ptr<Image_Button> btnExit(new Image_Button(ren, { 810, 680, 300, 60 }, { 180, 180, 180, 0 }, resources->fonts["calibri64"], L"Exit", scale,
		this, &WinMainMenu::BtnExit, resources->gui_textures["button1"].texture));
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
	windows.clear();
	std::unique_ptr<WinGame>winGame(new WinGame(ren, scale, windows, resources));
	windows.push_back(std::move(winGame));
}

void WinMainMenu::BtnLoadGame()
{

}

void WinMainMenu::BtnSettings()
{

}

void WinMainMenu::BtnExit()
{
	windows.clear();
}

// WinGame class
WinGame::WinGame(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources) : 
	Window(ren, scale, windows, resources) 
{
	std::unique_ptr<Image>bg_image(new Image(ren, resources->gui_textures["background2"].texture, resources->gui_textures["background2"].dstrect));
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
			std::unique_ptr<WinGameMenu>winSaveGame(new WinGameMenu(ren, scale, windows, resources));
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

// class WinSaveGame
WinGameMenu::WinGameMenu(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources) :
	Window(ren, scale, windows, resources)
{
	std::unique_ptr<Image_Button>btnSaveGame(new Image_Button(ren, { 810, 420, 300, 60 }, { 180, 180, 180, 0 }, resources->fonts["calibri64"], L"Save game",
		scale, this, &WinGameMenu::BtnSaveGame, resources->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnSaveGame));
	std::unique_ptr<Image_Button>btnLoadGame(new Image_Button(ren, { 810, 500, 300, 60 }, { 180, 180, 180, 0 }, resources->fonts["calibri64"], L"Load game",
		scale, this, &WinGameMenu::BtnLoadGame, resources->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnLoadGame));
	std::unique_ptr<Image_Button>btnMainMenu(new Image_Button(ren, { 810, 580, 300, 60 }, { 180, 180, 180, 0 }, resources->fonts["calibri64"], L"Main menu",
		scale, this, &WinGameMenu::BtnMainMenu, resources->gui_textures["button1"].texture));
	buttons.push_back(std::move(btnMainMenu));
	SDL_Rect bg_rect = { 260 / scale, 140 / scale, 1400 / scale, 800 / scale };
	std::unique_ptr<Image>bg_image(new Image(ren, resources->gui_textures["background1"].texture, bg_rect));
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
	SDL_SetRenderDrawColor(ren, 150, 50, 180, 255);

	for (int i = 0; i < images.size(); i++)
		images.at(i)->render();

	for (int i = 0; i < buttons.size(); i++)
		buttons.at(i)->render();
}

void WinGameMenu::BtnSaveGame()
{

}

void WinGameMenu::BtnLoadGame()
{

}
void WinGameMenu::BtnMainMenu()
{
	windows.clear();
	std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(ren, scale, windows, resources));
	windows.push_back(std::move(winMainMenu));
}