#include "program-11.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main() {
  SDL_Window* window = NULL;
  SDL_Surface* screenSurface = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  window =
      SDL_CreateWindow("SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  screenSurface = SDL_GetWindowSurface(window);

  SDL_FillRect(screenSurface, NULL,
               SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

  SDL_Event e = {0};
  bool happy = true;

  while (happy) {
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_MOUSEMOTION:
          printf("(%d, %d)\n", e.motion.x, e.motion.y);
          Uint32 pixelColor =
              SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0x00);
          if (e.motion.x >= 0 && e.motion.x < screenSurface->w &&
              e.motion.y >= 0 && e.motion.y < screenSurface->h) {
            Uint32* pixels = (Uint32*)screenSurface->pixels;
            pixels[(e.motion.y * screenSurface->w) + e.motion.x] = pixelColor;
          }

          break;
        case SDL_QUIT:
          happy = false;
          break;
        default:
          break;
      }
      SDL_UpdateWindowSurface(window);
    }
    SDL_Delay(1000 / 120);  // 120 fps
  }

  SDL_DestroyWindow(window);

  SDL_Quit();
}