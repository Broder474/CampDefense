#include <vector>
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <map>
#include <functional>
#include "Functions.h"
#include "World.h"
#include "Resources.h"

class Button
{
	using CallBack = std::function<void()>;
public:
	template<class O, class M>
	Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::wstring text, float scale, O* object, M method) :
		ren(ren), dstrect(dstrect), text_color(text_color), onClick(onClick), font(font), text(text), scale(scale)
	{
		this->dstrect.x /= scale;
		this->dstrect.y /= scale;
		this->dstrect.w /= scale;
		this->dstrect.h /= scale;
		generateTexture();
		setOnClick(object, method);
	};
	virtual	~Button()
	{
		SDL_DestroyTexture(text_texture);
	};

	virtual void render();
	void generateTexture();
	bool isPointed(SDL_Point point);

	template<class O, class M>
	void setOnClick(O* object, M method) { onClick = std::bind(method, object); }

	CallBack onClick;
protected:
	SDL_Renderer* ren;
	SDL_Rect dstrect;
	std::wstring text;
	TTF_Font* font;
	SDL_Rect text_rect;
	SDL_Color text_color;
	SDL_Texture* text_texture;
	float scale;
};

class Image_Button : public Button
{
public:
	template<class O, class M>
	Image_Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::wstring text, float scale, O* object, M method, SDL_Texture* bg_texture) :
		Button(ren, dstrect, text_color, font, text, scale, object, method), bg_texture(bg_texture) {};
	~Image_Button() override;
	void render() override;
private:
	SDL_Texture* bg_texture;
};

class Color_Button : public Button
{
public:
	template<class O, class M>
	Color_Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::wstring text, float scale, O* object, M method, SDL_Color bg_color) :
		Button(ren, dstrect, text_color, font, text, scale, object, method), bg_color(bg_color) {};
	~Color_Button() override;
	void render() override;
private:
	SDL_Color bg_color;
};

class Image
{
public:
	Image(SDL_Renderer* ren, SDL_Texture* texture, SDL_Rect dstrect): ren(ren), texture(texture), dstrect(dstrect) {};
	~Image() {};
	void render();
private:
	SDL_Renderer* ren;
	SDL_Texture* texture;
	SDL_Rect dstrect;
};

class Window
{
public:
	Window(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources) : ren(ren), scale(scale),
		windows(windows), resources(resources)  {};
	virtual ~Window() = 0 {};

	virtual void handleEvents() = 0;
	virtual void render() = 0;

protected:
	std::vector<std::unique_ptr<Window>>& windows;
	Resources* resources;
	SDL_Renderer* ren;
	float scale;
};

class WinMainMenu : public Window
{
public:
	WinMainMenu(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources);
	~WinMainMenu() override;

	void handleEvents() override;
	void render() override;

private:
	std::vector<std::unique_ptr<Button>> buttons;

	void BtnNewGame();
	void BtnLoadGame();
	void BtnSettings();
	void BtnExit();
};

class WinGame : public Window
{
public:
	WinGame(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources);
	~WinGame() override;
	
	void handleEvents() override;
	void render() override;
private:
	std::vector<std::unique_ptr<Image>> images;
};

class WinGameMenu : public Window
{
public:
	WinGameMenu(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources);
	~WinGameMenu() override;

	void handleEvents() override;
	void render() override;
private:
	std::vector<std::unique_ptr<Button>> buttons;
	std::vector<std::unique_ptr<Image>> images;

	void BtnSaveGame();
	void BtnLoadGame();
	void BtnMainMenu();
};