#include "SDL_video.h"
#include "logger/logger.h"
#include <SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main(int argc, char *argv[]) {
  // Handle command line arguments
  if (argc > 1) {
    std::cout << argc - 1 << " arguments passed:\n";
    for (int i = 1; i < argc; ++i) {
      std::cout << "  " << argv[i] << '\n';
    }
  }

  SDL_Window *window = nullptr;
  SDL_Surface *surface = nullptr;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    Logger::Crit("Failed to initialize SDL.", Logger::CheckSDLError);
    return 1;
  }

  const char *WINDOW_TITLE = "Cellular Automata";
  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);

  surface = SDL_GetWindowSurface(window);
  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
  SDL_UpdateWindowSurface(window);

  SDL_Event event;
  bool is_running = true;

  while (is_running) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: {
          Logger::Info("SDL_QUIT event received");
          is_running = false;
        } break;
      }
    }
  }

  return 0;
}
