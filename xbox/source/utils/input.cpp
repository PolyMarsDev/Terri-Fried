#include "input.h"

SDL_GameController *gameController;

void input_init() {
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
    gameController = SDL_GameControllerOpen(0);
}

void input_poll() {
    SDL_GameControllerUpdate();
}

int input_button_held(SDL_GameControllerButton button) {
    return SDL_GameControllerGetButton(gameController, button);
}

// TODO: Cleanup (better solution rather than having duplicates?)
uint8_t old_buttons_down[SDL_CONTROLLER_BUTTON_MAX];
uint8_t old_buttons_release[SDL_CONTROLLER_BUTTON_MAX];

int input_button_down(SDL_GameControllerButton button) {
    int ret = 0;
    int new_button = SDL_GameControllerGetButton(gameController, button);
    
    // logic
    if(new_button && !old_buttons_down[button])
        ret = 1;
    
    old_buttons_down[button] = new_button;
    
    return ret;
}

int input_button_release(SDL_GameControllerButton button) {
    int ret = 0;
    int new_button = SDL_GameControllerGetButton(gameController, button);
    
    // logic
    if(!new_button && old_buttons_release[button])
        ret = 1;
    
    old_buttons_release[button] = new_button;
    
    return ret;
}

int input_axis(SDL_GameControllerAxis axis) {
    int res = SDL_GameControllerGetAxis(gameController, axis);
    
    // HACK: Set an axes "deadzone" to prevent the player trajectory occasionally drifting
    if(res > -7500 && res < 7500)
        res = 0;
    
    return res;
}

void input_free() {
    SDL_GameControllerClose(gameController);
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}
