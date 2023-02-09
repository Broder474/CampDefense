#include <vector>
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <map>
#include <functional>
#include <filesystem>
#include "Functions.h"
#include "World.h"
#include "Resources.h"

namespace fs = std::filesystem;

class Button
{
	using CallBack = std::function<void()>;
public:
	template<class O, class M>
	Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::wstring text, float scale, O* object, M method);
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
	Image(SDL_Renderer* ren, SDL_Texture* texture, SDL_Rect dstrect, float scale);
	~Image() {};
	void render();
private:
	SDL_Renderer* ren;
	SDL_Texture* texture;
	SDL_Rect dstrect;
	float scale;
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

class List
{
	class ItemData;
public:
	List(SDL_Renderer* ren, SDL_Rect list_rect, SDL_Rect item_rect, SDL_Color bg_list_color, SDL_Color bg_item_color, SDL_Color selected_item_color,
		SDL_Color text_color, float scale, TTF_Font* font, int scroll_bar_width);
	~List();
	void addItem(std::wstring item_name);
	void removeItem(std::wstring item_name);
	void render();
	void scrollBarMove(SDL_Point point);
	void setScrollBarPointed(SDL_Point point);
	void setScrollBarPointed(bool isScrollBarPointed);
	bool getScrollBarPointed() { return is_scroll_bar_pointed; };
	std::wstring getClickedItem(SDL_Point point);

	std::shared_ptr<ItemData> selected_item = nullptr;
	std::vector<std::shared_ptr<ItemData>>items;
private:
	class ItemData
	{
	public:
		ItemData(SDL_Renderer* ren, std::wstring item_name, SDL_Rect item_rect, TTF_Font* font, SDL_Color text_color, float scale);
		~ItemData();
		bool isPointed(int x, int y);

		std::wstring item_name = L"";
		SDL_Rect item_rect{ 0, 0, 0, 0 };
		SDL_Texture* text_texture = nullptr;
		SDL_Rect text_rect{ 0, 0, 0, 0 };
	};
	SDL_Renderer* ren;
	SDL_Rect list_rect{ 0, 0, 0, 0 };
	SDL_Rect item_rect{ 0, 0, 0, 0 };
	SDL_Color bg_list_color{ 0, 0, 0, 0 };
	SDL_Color bg_item_color{ 0, 0, 0, 0 };
	SDL_Color selected_item_color{ 0, 0, 0, 0 };
	SDL_Color text_color{ 0, 0, 0, 0 };

	SDL_Color scroll_bar_color_full{ 0, 0, 0, 0 };
	SDL_Rect scroll_bar_rect_full{ 0, 0, 0, 0 };

	SDL_Color scroll_bar_color_current{ 0, 0, 0, 0 };
	SDL_Rect scroll_bar_rect_current{ 0, 0, 0, 0 };
	float scale = 0;
	TTF_Font* font = nullptr;
	int total_height = 0;
	bool is_scroll_bar_pointed = false;
	SDL_Point scroll_bar_click = { 0, 0 };
};

class TextOutput
{
public:
	TextOutput(SDL_Renderer* ren, TTF_Font* font, std::wstring text, float scale, int x, int y, SDL_Color text_color);
	TextOutput(SDL_Renderer* ren, TTF_Font* font, std::wstring text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color);
	virtual ~TextOutput() { SDL_DestroyTexture(text_texture); };

	virtual void render() = 0;
	virtual void generateTexture(int type) = 0;
protected:
	float scale = 0;
	SDL_Renderer* ren = nullptr;
	TTF_Font* font = nullptr;
	std::wstring text = L"";
	int x = 0;
	int y = 0;
	SDL_Color text_color = { 0, 0, 0, 0 };
	SDL_Color bg_color = { 0, 0, 0, 0 };
	SDL_Texture* text_texture = nullptr;
	SDL_Rect text_dstrect = { 0, 0, 0, 0 };
};

class TextOutputSingleLine : public TextOutput
{
public:
	TextOutputSingleLine(SDL_Renderer* ren, TTF_Font* font, std::wstring text, float scale, int x, int y, SDL_Color text_color);
	TextOutputSingleLine(SDL_Renderer* ren, TTF_Font* font, std::wstring text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color);
	~TextOutputSingleLine() override;

	void render() override;
	void generateTexture(int type) override;
};

class TextOutputMultiLine : public TextOutput
{
public:
	TextOutputMultiLine(SDL_Renderer* ren, TTF_Font* font, std::wstring text, float scale, int x, int y, SDL_Color text_color, int width);
	TextOutputMultiLine(SDL_Renderer* ren, TTF_Font* font, std::wstring text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color, 
		int width);
	~TextOutputMultiLine() override;

	void render() override;
	void generateTexture(int type) override;
private:
	int width = 0;
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

class WinSaveGame : public Window
{
public:
	WinSaveGame(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources);
	~WinSaveGame() override;

	void handleEvents() override;
	void render() override;
private:
	std::vector<std::unique_ptr<Button>> buttons;
	std::vector<std::unique_ptr<Image>> images;

	void BtnSaveGame();
	void BtnDeleteSave();
};

class WinLoadGame : public Window
{
public:
	WinLoadGame(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources);
	~WinLoadGame() override;

	void handleEvents() override;
	void render() override;

	std::unique_ptr<List> saves_list = nullptr;
private:
	std::vector<std::unique_ptr<Button>> buttons;

	void BtnLoadGame();
	void BtnDeleteSave();
	void LoadSaves();
};

class WinDelSave : public Window
{
public:
	WinDelSave(SDL_Renderer* ren, float scale, std::vector<std::unique_ptr<Window>>& windows, Resources* resources, std::unique_ptr<List>& saves_list);
	~WinDelSave() override;

	void handleEvents() override;
	void render() override;

private:
	std::vector<std::unique_ptr<Button>> buttons;
	std::unique_ptr<List>& saves_list;
	std::vector<std::unique_ptr<TextOutput>> static_text;

	void BtnDelete();
	void BtnCancel();
};