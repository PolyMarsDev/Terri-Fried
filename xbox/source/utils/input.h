#ifndef __INPUT_H__
#define __INPUT_H__

#include <SDL.h>

void input_init();
void input_poll();
void input_free();

int input_button_held(SDL_GameControllerButton button);
int input_button_down(SDL_GameControllerButton button);
int input_button_release(SDL_GameControllerButton button);
int input_axis(SDL_GameControllerAxis axis);

#endif
