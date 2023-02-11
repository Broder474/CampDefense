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
	Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::string text, float scale, O* object, M method);
	virtual	~Button()
	{
		SDL_DestroyTexture(text_texture);
	};

	virtual void render();
	void generateTexture();
	bool isPointed(SDL_Point point);

	template<class O, class M>
	void setOnClick(O* object, M method) { onClick = std::bind(method, object); }
	void setText(std::string text);
	std::string getText() { return text; }

	CallBack onClick;
protected:
	SDL_Renderer* ren;
	SDL_Rect dstrect;
	std::string text;
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
	Image_Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::string text, float scale, O* object, M method, SDL_Texture* bg_texture) :
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
	Color_Button(SDL_Renderer* ren, SDL_Rect dstrect, SDL_Color text_color, TTF_Font* font, std::string text, float scale, O* object, M method, SDL_Color bg_color) :
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
	Window(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* resources, Settings* settings) : ren(ren), settings(settings),
		windows(windows), resources(resources)  {};
	virtual ~Window() = 0 {};

	virtual void handleEvents() = 0;
	virtual void render() = 0;

protected:
	std::vector<std::unique_ptr<Window>>& windows;
	Resources* resources;
	SDL_Renderer* ren;
	Settings* settings;
};

class List
{
	class ItemData;
public:
	List(SDL_Renderer* ren, SDL_Rect list_rect, SDL_Rect item_rect, SDL_Color bg_list_color, SDL_Color bg_item_color, SDL_Color selected_item_color,
		SDL_Color text_color, float scale, TTF_Font* font, int scroll_bar_width);
	~List();
	void addItem(std::string item_name);
	void removeItem(std::string item_name);
	void render();
	void scrollBarMove(SDL_Point point);
	void setScrollBarPointed(SDL_Point point);
	void setScrollBarPointed(bool isScrollBarPointed);
	bool getScrollBarPointed() { return is_scroll_bar_pointed; };
	std::string getClickedItem(SDL_Point point);
	bool getVisibility() { return visible; }
	void setVisibility(bool visible) { this->visible = visible; }

	std::shared_ptr<ItemData> selected_item = nullptr;
	std::vector<std::shared_ptr<ItemData>>items;
private:
	class ItemData
	{
	public:
		ItemData(SDL_Renderer* ren, std::string item_name, SDL_Rect item_rect, TTF_Font* font, SDL_Color text_color, float scale);
		~ItemData();
		bool isPointed(int x, int y);

		std::string item_name = "";
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
	bool visible = true;
};

class TextOutput
{
public:
	TextOutput(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color);
	TextOutput(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color);
	virtual ~TextOutput() { SDL_DestroyTexture(text_texture); };

	virtual void render() = 0;
	virtual void generateTexture(int type) = 0;
protected:
	float scale = 0;
	SDL_Renderer* ren = nullptr;
	TTF_Font* font = nullptr;
	std::string text = "";
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
	TextOutputSingleLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color);
	TextOutputSingleLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color);
	~TextOutputSingleLine() override;

	void render() override;
	void generateTexture(int type) override;
};

class TextOutputMultiLine : public TextOutput
{
public:
	TextOutputMultiLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, int width);
	TextOutputMultiLine(SDL_Renderer* ren, TTF_Font* font, std::string text, float scale, int x, int y, SDL_Color text_color, SDL_Color bg_color, 
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
	WinMainMenu(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* resources, Settings* settings);
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
	WinGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* resources, Settings* settings);
	~WinGame() override;
	
	void handleEvents() override;
	void render() override;
private:
	std::vector<std::unique_ptr<Image>> images;
};

class WinGameMenu : public Window
{
public:
	WinGameMenu(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* resources, Settings* settings);
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
	WinSaveGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* resources, Settings* settings);
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
	WinLoadGame(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* resources, Settings* settings);
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
	WinDelSave(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* resources, Settings* settings, 
		std::unique_ptr<List>& saves_list);
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

class WinSettings : public Window
{
public:
	WinSettings(SDL_Renderer* ren, std::vector<std::unique_ptr<Window>>& windows, Resources* resources, Settings* settings);
	~WinSettings() override;

	void handleEvents() override;
	void render() override;

	void BtnSave();
	void BtnLang();

private:
	std::vector<std::unique_ptr<Button>> buttons;

	std::unique_ptr<List> lang_list = nullptr;
	std::unique_ptr<Image_Button> btnLang = nullptr;
};