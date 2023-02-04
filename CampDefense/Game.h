#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <memory>
#include <map>
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

    std::vector<std::unique_ptr<Window>> windows;
    Resources* resources;
    World* world;

    SDL_Window* win;
    SDL_Renderer* ren;
    float scale;

private:
    bool status;
};