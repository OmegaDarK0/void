#include <stdio.h>
#include <SDL2/SDL.h>

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Log("Hello World!");
    SDL_Quit();
    return 0;
}
