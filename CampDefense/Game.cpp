#include "Game.h"

Game::Game()
{
    TTF_Init();
    win = SDL_CreateWindow("Camp Defense", 0, 0, 1920, 1080, SDL_WINDOW_FULLSCREEN_DESKTOP);
    ren = SDL_CreateRenderer(win, -1, 0);
    setStatus(true);

    json settings;
    std::ifstream in_settings("settings.json");
    bool settings_loaded = false;
    if (in_settings.is_open())
        if (bool is_file_correct = json::accept(in_settings))
        {
            in_settings.close();
            in_settings.open("settings.json");
            settings = json::parse(in_settings);

            // check if necessary data has correct type
            if (settings["scale"].is_number_float() && settings["lang"].is_string())
                settings_loaded = true;

            in_settings.close();
        }
    if (settings_loaded == false)
    {
        in_settings.close();
        settings["lang"] = "en";

        // get screen scale
        SDL_DisplayMode display_mode;
        SDL_GetCurrentDisplayMode(0, &display_mode);
        1920 / (float)display_mode.w > 1080 / (float)display_mode.h ? settings["scale"] = 1920 / (float)display_mode.w : settings["scale"] = 1080 / 
            (float)display_mode.h;

        // save current settings to file
        saveSettings(&settings);
    }
    this->settings.parse_json(settings);

    loadResources();

    // main menu initialization
    std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(ren, windows, resources, &this->settings));
    windows.push_back(std::move(winMainMenu));
}
Game::~Game() {}

void Game::loop()
{
    while (getStatus())
    {
        SDL_Delay(16); // 60 tacts per second

        windows.back()->handleEvents(); // handle events from top window

        if (windows.size() > 0)
        {
            for (auto& win : windows)
                win->render(); // render all windows
            SDL_RenderPresent(ren);
        }
        else
            setStatus(false);
    }
    clean();
    SDL_Quit();
}

void Game::clean()
{
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);
}

void Game::saveSettings(json* settings)
{
    std::ofstream out_settings("settings.json", std::ios::trunc);
    out_settings << *settings;
    out_settings.close();
}

void Game::loadResources()
{
    std::string lang_file = "resources/lang/" + this->settings.getLang() + ".json";
    if (resources != nullptr)
        resources->~Resources();
    resources = new Resources(ren, &this->settings, "resources/data/weapons.json", "resources/data/gui.json", lang_file.c_str());
}