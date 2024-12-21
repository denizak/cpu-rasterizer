#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>

#include <rasterizer/renderer.hpp>

using namespace rasterizer;

int main()
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Surface * draw_surface = nullptr;

  int width = 800;
  int height = 600;

  SDL_Window * window = SDL_CreateWindow("Tiny rasterizer",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    width, height,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

  int mouse_x = 0;
  int mouse_y = 0;

  using clock = std::chrono::high_resolution_clock;
  auto last_frame_start = clock::now();

  bool running = true;
  while(running)
  {
    for(SDL_Event event; SDL_PollEvent(&event);) switch(event.type)
    {
      case SDL_WINDOWEVENT:
        switch(event.window.event)
        {
          case SDL_WINDOWEVENT_RESIZED:
            width = event.window.data1;
            height = event.window.data2;
            break;
        }
        break;
      case SDL_QUIT:
        running = false;
        break;
      case SDL_MOUSEMOTION:
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
        break;
      case SDL_WINDOWEVENT_RESIZED:
        if(draw_surface)
          SDL_FreeSurface(draw_surface);
        draw_surface = nullptr;
        width = event.window.data1;
        height = event.window.data2;
        break;
    }

    if (!running)
      break;

    auto now = clock::now();
    float dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_frame_start).count();
    last_frame_start = now;

    std::cout << dt << std::endl;

    if (!draw_surface)
    {
      draw_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
      SDL_SetSurfaceBlendMode(draw_surface, SDL_BLENDMODE_NONE);
    }

    image_view color_buffer
    {
      .pixels = (color4ub *)draw_surface->pixels,
      .width = (std::uint32_t)width,
      .height = (std::uint32_t)height,
    };

    clear(color_buffer, {0.8f, 0.9f, 1.f, 1.f});

    vector3f vertices[] =
    {
      {100.f, 100.f, 0.f},
      {200.f, 100.f, 0.f},
      {100.f, 200.f, 0.f},
    };

    draw(color_buffer,
         draw_command {
           .mesh = {
             .positions = vertices,
             .vertex_count = 3,
             .color = {1.f, 0.f, 0.f, 1.f},
           }
         }
    );

    SDL_Rect rect{.x = 0, .y = 0, .w = width, .h = height};
    SDL_BlitSurface(draw_surface, &rect, SDL_GetWindowSurface(window), &rect);

    SDL_UpdateWindowSurface(window);
  }
}
