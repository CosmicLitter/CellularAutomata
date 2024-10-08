#include "../libs/imgui/imgui.h"
#include "../libs/imgui/imgui_impl_sdl2.h"
#include "../libs/imgui/imgui_impl_sdlrenderer2.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "logger/logger.h"
#include <SDL.h>
#include <chrono>
#include <iostream>
#include <thread>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

/*
================================================================================
== Development Globals                                                        ==
================================================================================
*/
// Grid
const int CELL_SIZE     = 10;
const int GRID_WIDTH    = 128;
const int GRID_HEIGHT   = 72;

// Window dimensions
const int SCREEN_WIDTH    = (GRID_WIDTH * CELL_SIZE) + 1;
const int SCREEN_HEIGHT   = (GRID_HEIGHT * CELL_SIZE) + 1;

// Framerate control
using Ticks                   = uint64_t;
const int FPS                 = 60;
const Ticks TICKS_PER_SECOND  = 1000;

/*
================================================================================
== Main                                                                       ==
================================================================================
*/
int main(int argc, char *argv[]) {
  // Handle command line arguments
  if (argc > 1) {
    std::cout << argc - 1 << " arguments passed:\n";
    for (int i = 1; i < argc; ++i) {
      std::cout << "  " << argv[i] << '\n';
    }
  }

  // Initialize SDL window & renderer
  SDL_Window *window      = nullptr;
  SDL_Renderer *renderer  = nullptr;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0) {
    Logger::Crit("Failed to initialize SDL.", Logger::CheckSDLError);
    return -1;
  }

  // Unsure if this would be useful. IME stands for input method editor, and this hint enables the input of special characters when entering text
  // SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

  const char *WINDOW_TITLE = "Cellular Automata";
  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    Logger::Crit("Failed to create window.", Logger::CheckSDLError);
    return -1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    Logger::Crit("Failed to create renderer.", Logger::CheckSDLError);
    return -1;
  }

  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  Logger::Info(std::string("Current SDL Renderer: ") + info.name);

  // Setup ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  // ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // ImGui platform backends
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  // Example ImGui windows
  bool show_demo_window     = false;
  bool show_another_window  = false;

  // ImGui managed state
  ImVec4 grid_background_color    = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
  ImVec4 grid_line_color          = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
  ImVec4 grid_cursor_color        = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
  // ImVec4 cell_selected_color   = ImVec4(0.9f, 0.9f, 0.9f, 1.00f);

  // Grid cursor starts in center of screen
  SDL_Rect grid_cursor = {(GRID_WIDTH - 1) / 2 * CELL_SIZE + 1, (GRID_HEIGHT - 1) / 2 * CELL_SIZE + 1, CELL_SIZE -1, CELL_SIZE -1};

  // Main loop
  using namespace std::chrono;
  const auto frame_duration = milliseconds(TICKS_PER_SECOND / FPS);
  bool is_running = true;
  while (is_running) {
    auto frame_start = steady_clock::now();

    // Event Handling
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch (event.type) {
        case SDL_QUIT: {
          Logger::Info("SDL_QUIT message received");
          is_running = false;
        } break;
        case SDL_KEYDOWN: {
          switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: {
              Logger::Info("Escape key pressed. Exiting");
              is_running = false;
            } break;
            default: {
              Logger::Info("Key press: " + std::to_string(event.key.keysym.scancode));
            }
          }
        } break;
        default: {
          Logger::Info("Event: " + std::to_string(event.type));
        }
      }
    }

    // Start ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Show demo window
    if (show_demo_window) {
      ImGui::ShowDemoWindow(&show_demo_window);
    }

    // A custom ImGui window
    {
      // static float f = 0.0f;
      // static int counter = 0;
      ImGui::Begin("Hello, world!");

      ImGui::Text("Basic Config");
      ImGui::Checkbox("Demo Window", &show_demo_window);
      // ImGui::Checkbox("Another Window", &show_another_window);

      // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui::ColorEdit3("grid background color",
                        (float *)&grid_background_color);
      ImGui::ColorEdit3("grid line color", (float *)&grid_line_color);
      ImGui::ColorEdit3("cell cursor color", (float *)&grid_cursor_color);
      // ImGui::ColorEdit3("cell selected color", (float
      // *)&cell_selected_color);

      // if (ImGui::Button("Count"))
      //   counter++;
      // ImGui::SameLine();
      // ImGui::Text("Counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    // Another window
    if (show_another_window) {
      ImGui::Begin("Another Window", &show_another_window);
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        show_another_window = false;
      ImGui::End();
    }

    // Render
    ImGui::Render();
    SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

    SDL_SetRenderDrawColor(renderer, (Uint8)(grid_background_color.x * 255), (Uint8)(grid_background_color.y * 255), (Uint8)(grid_background_color.z * 255), (Uint8)(grid_background_color.w * 255));

    SDL_RenderClear(renderer);

    // Grid Lines
    SDL_SetRenderDrawColor(renderer, (Uint8)(grid_line_color.x * 255), (Uint8)(grid_line_color.y * 255), (Uint8)(grid_line_color.z * 255), (Uint8)(grid_line_color.w * 255));

    for (int x = 0; x < 1 + GRID_WIDTH * CELL_SIZE; x += CELL_SIZE) {
      SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT - 1);
    }

    for (int y = 0; y < 1 + GRID_HEIGHT * CELL_SIZE; y += CELL_SIZE) {
      SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH - 1, y);
    }

    // Draw cursor
    SDL_SetRenderDrawColor(renderer, (Uint8)(grid_cursor_color.x * 255), (Uint8)(grid_cursor_color.y * 255), (Uint8)(grid_cursor_color.z * 255), (Uint8)(grid_cursor_color.w * 255));
    SDL_RenderFillRect(renderer, &grid_cursor);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);

    // Frame control
    auto frame_end = steady_clock::now();
    auto frame_time = frame_end - frame_start;
    if (frame_time < frame_duration) {
      std::this_thread::sleep_for(frame_duration - frame_time);
    }
  }

  // ImGui Cleanup
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  // SDL Cleanup
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
