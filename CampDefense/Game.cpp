#include "Game.h"

Game::Game()
{
    TTF_Init();
    win = SDL_CreateWindow("Camp Defense", 0, 0, 1920, 1080, SDL_WINDOW_FULLSCREEN_DESKTOP);
    ren = SDL_CreateRenderer(win, -1, 0);
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    1920 / (float)display_mode.w > 1080 / (float)display_mode.h ? scale = 1920 / (float)display_mode.w : scale = 1080 / (float)display_mode.h;
    setStatus(true);

    // loading resources
    resources = new Resources(ren, scale, "resources/data/weapons.json", "resources/data/gui.json");

    // main menu initialization
    std::unique_ptr<WinMainMenu>winMainMenu(new WinMainMenu(ren, scale, windows, resources));
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
            for (int win = 0; win < windows.size(); win++)
                windows.at(win)->render(); // render all windows
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