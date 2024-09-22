#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL2/SDL.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define NUM_RAYS 120
#define FOV (M_PI / 3) // 60 degrees field of view
#define MAX_DISTANCE 1000.0f

typedef struct {
	float x, y;
} Point;

typedef struct {
    float x, y;
	float rotation;
} Player;

typedef struct {
    float x1, y1, x2, y2;
} Line;

typedef struct {
    int total_lines;
    Line* lines;
} Sector;

typedef struct {
    Player player;
    float rotation;
    int total_sectors;
    Sector* sectors;
} Map;

extern SDL_Renderer* renderer;

Map load_map(const char *filename);
void cast_rays(Map* map, float* distances);
void draw_scene(Map* map, float* distances);
void draw_minimap(Map* map, float* distances);
Point find_intersection(Line* line, float ray_x, float ray_y, Player player, float* distance);
void handle_input(Map* map, float* distances, int* running);

#endif // RAYCASTER_H

