#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include <string>
#include "nlohmann/json.hpp"
#include "Window.h"

using json = nlohmann::json;

class Game
{
public:
    Game();
    ~Game();

    void loop();
    bool getStatus() { return status; }
    void setStatus(bool status) { this->status = status; }
    void clean();

    void saveSettings(json* settings);
    void loadResources();

    std::vector<std::unique_ptr<Window>> windows;
    Resources* resources = nullptr;
    Settings settings;
    World* world = nullptr;

    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;
    const Uint8* keys = nullptr;

private:
    bool status;
};