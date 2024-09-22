#include <SDL2/SDL.h>
#include "raycaster.h"

SDL_Window* window = NULL;

void handle_input(Map* map, float* distances, int* running) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    float move_speed = 2.0f;
    float rotation_speed = 0.05f;

    if (state[SDL_SCANCODE_W]) {
        map->player.x += cos(map->player.rotation) * move_speed;
        map->player.y += sin(map->player.rotation) * move_speed;
    }
    if (state[SDL_SCANCODE_S]) {
        map->player.x -= cos(map->player.rotation) * move_speed;
        map->player.y -= sin(map->player.rotation) * move_speed;
    }
    if (state[SDL_SCANCODE_A]) {
        map->player.rotation -= rotation_speed;
    }
    if (state[SDL_SCANCODE_D]) {
        map->player.rotation += rotation_speed;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = 0;
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Raycasting Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Map map;
    map.player.x = SCREEN_WIDTH / 2;
    map.player.y = SCREEN_HEIGHT / 2;
    map.player.rotation = 0.0f;
    // Example sector with walls
    Line lines[] = {
        {100, 100, 300, 100},
        {300, 100, 300, 300},
        {300, 300, 100, 300},
        {100, 300, 100, 100}
    };
    Sector sector = {4, lines};
    map.total_sectors = 1;
    map.sectors = &sector;

    float distances[NUM_RAYS];

    int running = 1;
    while (running) {
        handle_input(&map, distances, &running);
        cast_rays(&map, distances);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_scene(&map, distances);
        draw_minimap(&map, distances);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

